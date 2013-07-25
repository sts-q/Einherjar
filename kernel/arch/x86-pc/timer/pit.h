#ifndef _PIT_H_
#define _PIT_H_

/**
 * @file pit.h
 * @author Konstantin Tcholokachvili
 * @date 2013
 * @license MIT License
 *
 * @see [en] i82C54's datasheet
 * @see [en] http://www.osdever.net/bkerndev/Docs/pit.htm
 * @see [en] http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs and the PIT.html
 *
 * Programmable Interrupt Timer
 */

#include <arch/all/types.h>
#include <arch/x86-all/registers.h>

/** 
 * Changes timer interrupt frequency from the default one (18.222 Hz)
 * 
 * @param frequency Frequency at which interrupts whill be raised
 * @return status Status indicating the success of operation
 */
uint16_t x86_pit_set_frequency(uint32_t frequency);

/**
* Timer's interrupt handler called periodically
*
* @param id
*/
void timer_interrupt_handler(struct regs *r);

#endif // _PIT_H_

