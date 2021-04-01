#include <arch/x86-pc/io/vga.h>
#include <lib/libc.h>

#include "colorforth.h"

#define HEAP_SIZE	(1024 * 100)	// 100 Kb
#define STACK_SIZE	42

#define FORTH_TRUE -1      // In Forth world -1 means true
#define FORTH_FALSE 0

typedef void (*FUNCTION_EXEC)(void);

/*
 * Stack macros
 */
#define stack_push(x) *(tos++) = x
#define stack_pop()   *(--tos)
#define nos           tos[-1]	// Next On Stack
#define rpush(x)      *(++rtos) = x
#define rpop()        *(rtos--)
#define start_of(x)   (&x[0])

/* Data stack */
cell_t stack[STACK_SIZE];
cell_t *tos = start_of(stack);	// Top Of Stack

/* Return stack */
unsigned long rstack[STACK_SIZE];
unsigned long *rtos = start_of(rstack);

/*
 * Global variables
 */
unsigned long *code_here;
unsigned long *h;			// Code is inserted here
bool_t         selected_dictionary;
extern cell_t *blocks;			// Manage looping over the code contained in blocks
unsigned long *IP;			// Instruction Pointer
bool_t is_hex = FALSE;

/*
 * Prototypes
 */
static void ignore(const cell_t word);
static void interpret_forth_word(const cell_t word);
static void interpret_big_number(const cell_t number);
static void create_word(cell_t word);
static void compile_word(const cell_t word);
static void compile_big_number(const cell_t number);
static void compile_number(const cell_t number);
static void compile_macro(const cell_t word);
static void interpret_number(const cell_t number);
static void variable_word(const cell_t word);

/* Word extensions (0), comments (9, 10, 11, 15), compiler feedback (13)
 * and display macro (14) are ignored. */
void (*color_word_action[16])() =
{                ignore,                            /*   0  */
                 interpret_forth_word,              /*   1  INTERPRET_WORD_TAG    */
	         interpret_big_number,              /*   2  */
                 create_word,                       /*   3  */
                 compile_word,                      /*   4  */
                 compile_big_number,                /*   5  */
	         compile_number,                    /*   6  */
                 compile_macro,                     /*   7  */
                 interpret_number,                  /*   8  INTERPRET_NUMBER_TAG  */
	         ignore,                            /*   9  */
                 ignore,                            /*  10  */
                 ignore,                            /*  11  */
                 variable_word,                     /*  12  */
                 ignore,                            /*  13  */
                 ignore,                            /*  14  */
                 ignore};                           /*  15  */

/*
 * Built-in words
 */
void comma(void)
{
}

void load(void)
{
	cell_t n = stack_pop();
	run_block(n);
}

void loads(void)
{
	cell_t i, j;

	j = stack_pop();
	i = stack_pop();

	// Load blocks, excluding shadow blocks
	for (; i <= j; i += 2)
	{
		stack_push(i);
		load();
	}
}

void forth(void)
{
	selected_dictionary = FORTH_DICTIONARY;
}

void macro(void)
{
	selected_dictionary = MACRO_DICTIONARY;
}

//----------------------------------------------------------------------------
void dup(void)
{
	cell_t a = stack_pop();
	stack_push( a );
	stack_push( a );
}

void zap(void)
{
	cell_t a = stack_pop();
}

void nip(void)
{
	cell_t a = stack_pop();
	           stack_pop();
	stack_push( a );
}

void swap(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push( a );
	stack_push( b );
}

void over(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push( b );
	stack_push( a );
	stack_push( b );
}

void leap(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	cell_t c = stack_pop();
	stack_push( c );
	stack_push( b );
	stack_push( a );
	stack_push( c );
}


//----------------------------------------------------------------------------
void add(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push(a + b);
}

void sub(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push(b - a);
}

void mul(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push(a * b);
}

void divide(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push(b / a);
}

//----------------------------------------------------------------------------
void is_equal(void)
{
	cell_t a = stack_pop();
	cell_t b = stack_pop();
	stack_push( a == b );
}

void is_zero(void)
{
	cell_t a = stack_pop();
	stack_push( a == 0 );
}


//----------------------------------------------------------------------------

void dot_s(void)
{
	erase_stack();
	vga_set_position(0, 22);
	vga_set_attributes(FG_YELLOW | BG_BLACK);

	int nb_items = tos - start_of(stack);

	printf("\nStack [%d]: ", nb_items);

	for (int i = 0; i < nb_items; i++)
	{
		if (is_hex)
			printf("%x ", stack[i]);
		else
			printf("%d ", stack[i]);
	}

	printf("\n");
}

void dot(void)
{
	printf("%d ", (int)stack_pop());
	printf("a: %d ", see_a);
	printf("b: %d ", see_b);
	printf("c. %d ", see_c);
}

/*
 * Helper functions
 */
void
dispatch_word(const cell_t word)
{
	uint8_t color = (int)word & 0x0000000f;
	(*color_word_action[color])(word);
}

void
run_block(const cell_t n)
{
	unsigned long start = n * 256;     // Start executing block from here...
	unsigned long limit = (n+1) * 256; // ...to this point.

	for (unsigned long i = start; i < limit-1; i++)
	{
		dispatch_word(blocks[i]);
	}
}

word_t forth_dictionary[128] =
{
	{.name = 0xfc000000, .code_address = comma},
	{.name = 0xa1ae0000, .code_address = load},
	{.name = 0xa1ae0800, .code_address = loads},
	{.name = 0xb1896400, .code_address = forth},
	{.name = 0x8ac84c00, .code_address = macro},
	{.name = 0xea000000, .code_address = dot},
	{.name = 0xf6000000, .code_address = add},
	{.name = 0xee000000, .code_address = divide},
        
        {.name = 0xe6000000, .code_address = sub},      // tools/pack.py "-"  
        {.name = 0xfa000000, .code_address = mul},
        
        {.name = 0xc19b1000, .code_address = dup},
        {.name = 0xecb88000, .code_address = zap},
        {.name = 0x67c40000, .code_address = nip},
        {.name = 0x85d71000, .code_address = swap},
        {.name = 0x3c282000, .code_address = over},
        {.name = 0xa22e2000, .code_address = leap},
        
        {.name = 0x4cf4fe00, .code_address = is_equal},         // a b eql?
        {.name = 0xec827fc0, .code_address = is_zero},          // a   zero?
	{0, 0},
};

word_t macro_dictionary[32] =
{
	{0, 0}
};

word_t
lookup_word(cell_t name, const bool_t force_dictionary)
{
	name &= 0xfffffff0; // Don't care about the color byte

	if (force_dictionary == FORTH_DICTIONARY)
	{
		for (int i = 0; forth_dictionary[i].name; i++)
		{
			if (name == forth_dictionary[i].name)
				return forth_dictionary[i];
		}
	}
	else
	{
		for (int i = 0; macro_dictionary[i].name; i++)
		{
			if (name == macro_dictionary[i].name)
				return macro_dictionary[i];
		}

	}

	return (word_t){0, 0};
}

static void
execute(const word_t word)
{
	IP = word.code_address;
	((FUNCTION_EXEC)word.code_address)();
}

/*
 * Colorful words handling
 */
static void
ignore(const cell_t word)
{
	(void)word; // Avoid an useless warning and do nothing!
}

static void
interpret_forth_word(const cell_t word)
{
	word_t found_word = lookup_word(word, FORTH_DICTIONARY);

	if (found_word.name)
	{
		execute(found_word);
	}
}

static void
interpret_big_number(const cell_t number)
{
	(void)number;
}

static void
compile_word(const cell_t word)
{
	(void)word;
}

static void
compile_number(const cell_t number)
{
	(void)number;
}

static void
compile_big_number(const cell_t number)
{
	(void)number;
}

static void
interpret_number(const cell_t number)
{
	stack_push(number >> 5);
}

static void
compile_macro(const cell_t word)
{
	(void)word;
}

static void
create_word(cell_t word)
{
	(void)word;
}

static void
variable_word(const cell_t word)
{
	(void)word;
}

/*
 * Initializing and deinitalizing colorForth
 */
void
colorforth_initialize(void)
{
	code_here = malloc(HEAP_SIZE);

	if (!code_here)
	{
		panic("Error: Not enough memory!\n");
	}

	h = code_here;

	// Init stack
	memset(stack, 0, STACK_SIZE);

	// FORTH is the default dictionary
	forth();
}
