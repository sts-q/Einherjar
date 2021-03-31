#include <arch/x86/io-ports.h>
#include <arch/x86-pc/io/vga.h>
#include <arch/x86/interrupts/irq.h>

#include "keyboard.h"

/*
==============================================================================
sts-q
my strange keyboard layout:

normal
      "  s  u  g  q  v  p  c  d  w
       a  o  i  L  f  b  T  e  n  r  +
         y  x  z  h  j  k  m  ,  .  -

shift
      '  ^  ~  @  o  o  {  [  ]  }
       !  ?  &  $  %  o  (  )  <  >  *
         \  |  /  o  o  #  =  ;  :  _

meta
                        7  8  9                ( is on    p  c  d        )
                         4  5  6               ( is on     t  e  n       )
                           1  2  3  0          ( is on       m  ,  .  -  )

Shift needes to be pressed once before each shift-char.

Meta is like traditional Caps-lock.


==============================================================================
*/

char keymap[] = {
/* 
normal shift   ctrl    meta    
*/
0x0,    0x0,    0x0,    0x0,
0x1B,  0x1B,   0x1B,   0x1B,    /*  esc (0x01)  */
'1',    'o',    '1',    'o',
'2',    'o',    '2',    'o',
'3',    'o',    '3',    'o',
'4',    'o',    '4',    'o',
'5',    'o',    '5',    'o',
'6',    'o',    '6',    'o',
'7',    'o',    '7',    'o',
'8',    'o',    '8',    'o',
'9',    'o',    '9',    'o',
'0',    'o',    '0',    'o',
'o',    'o',    '-',    'o',
'o',    'o',    '=',    'o',  
0x08,  0x08,   0x7F,   0x08,    /*  backspace   */
0x09,  0x09,   0x09,   0x09,    /*  tab */
'"',   0x27,    'q',    'o',    /*  (0x10) */
's',    '^',    'w',    'o',
'u',    '~',    'e',    'o',
'g',    '@',    'r',    'o',
'q',    'o',    't',    'o',
'v',    'o',    'z',    'o',  
'p',    '{',    'u',    '7',
'c',    '[',    'i',    '8',
'd',    ']',    'o',    '9',
'w',    '}',    'p',    'o',
'o',    'o',    '[',    'o',
'o',    'o',    ']',    'o',
0x0A,   0x0A,  0x0A,   0x0A,    /*  enter   */
0xFF,  0xFF,   0xFF,   0xFF,    /*  (0x1d)  ctrl    */
'a',    '!',    'a',    'o',    /*  (0x1E)  */
'o',    '?',    's',    'o',
'i',    '&',    'd',    'o',    /*  (0x20)  */
'l',    '$',    'f',    'o', 
'f',    '%',    'g',    'o',
'b',    'o',    'h',    'o',
't',    '(',    'j',    '4',
'e',    ')',    'k',    '5',
'n',    '<',    'l',    '6',
'r',    '>',    ';',    'o',
'+',    '*',   0x27,   0x27,  
'`',    '~',    '`',    'o',  
0xFF,  0xFF,   0xFF,   0xFF,    /*  Lshift  (0x2a)  */  
'o',    'o',   '\\',    'o',
'y',   '\\',    'y',    'o',
'x',    '|',    'x',    'o',
'z',    '/',    'c',    'o',
'h',    'o',    'v',    'o',
'j',    'o',    'b',    'o',    /*  (0x30)  */
'k',    '#',    'n',    'o',
'm',    '=',    'm',    '1',
0x2C,   ';',   0x2C,    '2',    /*  ,;      */
0x2E,   ':',   0x2E,    '3',    /*  .:      */
'-',    '_',   0x2F,    '0', 
0x10,   0x10,   66,   66,       /*  Rshift  (0x36)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x37)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x38)  Meta  */        
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x39)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x3a)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x3b)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x3c)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x3d)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x3e)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x3f)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x40)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x41)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x42)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x43)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x44)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x45)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x46)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x47)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x48)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x49)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x4a)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x4b)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x4c)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x4d)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x4e)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x4f)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x50)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x51)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x52)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x53)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x54)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x55)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x56)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x57)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x58)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x59)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x5a)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x5b)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x5c)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x5d)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x5e)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x5f)  */
0xFF,   0xFF,   0xFF,   0xFF,   /*  (0x60)  */
0xFF,   0xFF,   0xFF,   0xFF    /*  (0x61)  */
};

static struct console *terminal;

void keyboard_interrupt_handler(int number)
{
	uchar_t status, scancode;

	(void)number; // Avoid a useless warning ;-)

	do
	{
		status = inb(KEYBOARD_COMMAND_PORT);
	} while((status & 0x01) == 0);

	scancode = inb(KEYBOARD_DATA_PORT);

	if (scancode < 0x80)
	{
		console_add_character(terminal, scancode);
	}
}

char keyboard_get_keymap(uchar_t i)
{
	return keymap[i*4];
}

char keyboard_get_keymap_shifted(uchar_t i)
{
	return keymap[i*4 + 1];
}

char keyboard_get_keymap_meta(uchar_t i)
{
	return keymap[i*4 + 3];
}

void keyboard_setup(struct console *term)
{
	terminal = term;
	x86_irq_set_routine(IRQ_KEYBOARD, keyboard_interrupt_handler);
}
