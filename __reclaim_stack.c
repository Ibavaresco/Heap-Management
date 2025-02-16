//==============================================================================
// This file is part of "Heap Management For Small Microcontrollers".
// v1.05 (2009-06-29)
// isaacbavaresco@yahoo.com.br
//==============================================================================
/*
 Copyright (c) 2007-2009, Isaac Marino Bavaresco
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Neither the name of the author nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//==============================================================================
#include <heap.h>
#include <heap_config.h>
//==============================================================================

// Annex the original system stack to the heap.
//
// Should be called only once, inside a thread, after the call to the functions
// heapinit() and vTaskStartScheduler(), when the original stack is no more in use.
//
// This function works only for the combination PIC18 microcontrollers +
// MPLAB-C18 compiler + FreeRTOS (www.freertos.org).

// TIP: If you make a careful linker-script, you may join the stack to the heap
// as one contiguous block, thus optimizing the available memory.
// Define the stack just before the heap, with nothing between them. 
// When you call '__reclaim_stack', it calls 'free' passing the stack as a block
// to free, and because 'malloc' allocates from the end of the heap, the beginning
// of the heap contains free memory, and 'free' joins the stack to this free memory.

//         before                            after         
//                                                          
// |                     |           |                     |
// +---------------------+           +---------------------+
// |                     |           |                     |   lower
// |        stack        |           |                     | addresses
// |                     |           |                     |
// +---------------------+           |      free heap      |
// |                     |           |                     |
// |      free heap      |           |                     |
// |                     |           |                     |
// +- - - - - - - - - - -+           +- - - - - - - - - - -+
// |                     |           |                     |
// |      used heap      |           |      used heap      |  higher
// |                     |           |                     | addresses
// +---------------------+           +---------------------+
// |                     |           |                     |

// NEW for v1.04: If the macro ALLOCATE_FROM_BEGINNING is defined, then the
// correct placement of the sections is as follow:

//         before                            after         
//                                                          
// |                     |           |                     |
// +---------------------+           +---------------------+
// |                     |           |                     |   lower
// |      used heap      |           |      used heap      | addresses
// |                     |           |                     |
// +- - - - - - - - - - -+           +- - - - - - - - - - -+
// |                     |           |                     |
// |      free heap      |           |                     |
// |                     |           |                     |
// +---------------------+           |      free heap      |
// |                     |           |                     |
// |        stack        |           |                     |  higher
// |                     |           |                     | addresses
// +---------------------+           +---------------------+
// |                     |           |                     |


// The standard library implementation of __reclaim_stack() do nothing.
// The user should implement it, because there is no way to know the stack size
// at run time or when building the library.

#if			STACK_SIZE > 0
void __reclaim_stack( void )
	{
	((struct _AllocBlock RAM *)&_stack)->Length	= STACK_SIZE - sizeof( sizeram_t );
	free( &((struct _AllocBlock RAM *)&_stack)->Next );
	}
#endif	//	STACK_SIZE > 0
//==============================================================================