/**
 * @file segment.h
 * @author Konstantin Tcholokachvili
 * @date 2013
 */

#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#define NULL_SEGMENT 		0 // Unused by the processor
#define KERNEL_CODE_SEGMENT	1
#define KERNEL_DATA_SEGMENT 	2
#define USER_CODE_SEGMENT 	3
#define USER_DATA_SEGMENT 	4
#define KERNEL_TSS_SEGMENT	5 // Kernel TSS for CPL3 -> CPL0 privilege change

/**
 * Builds a value for a segment register
 */
#define X86_BUILD_SEGMENT_REGISTER_VALUE(descriptor_privilege_level, in_ldt, segment_index) \
  ( (((descriptor_privilege_level) & 0x3)	<< 0) \
	| (((in_ldt)?1:0)            		<< 2) \
	| ((segment_index)               	<< 3) )
	

#endif // _SEGMENT_H_
