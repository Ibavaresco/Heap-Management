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
#if			defined __18CXX	// PIC18 family with Microchip MPLAB-C18 compiler

	#include <p18cxxx.h>
	#define	INT_MASK 0xc0

#elif		defined _PIC14	// PIC16 family with Hi-Tech PICC compiler

	#include <pic.h>
	#define	INT_MASK 0x80

#elif		defined _PIC18	// PIC18 family with Hi-Tech PICC-18 compiler

	#include <pic18.h>
	#define	INT_MASK 0xc0

#else	//	defined _PIC18
	#error "Architecture not supported!"
#endif	//	defined __18CXX
//==============================================================================
#include <alloc.h>
#include <heap.h>
#include <heap_config.h>
//==============================================================================
#if			USING_FREE_RTOS == 1

void		vTaskSuspendAll			( void );
signed char	xTaskResumeAll			( void );
#define		DISABLE_INTERRUPTS()	vTaskSuspendAll()
#define		RESTORE_INTERRUPTS()	xTaskResumeAll()

#else	//	USING_FREE_RTOS == 1

#define		DISABLE_INTERRUPTS()	{Aux = INTCON & INT_MASK; INTCON &= ~INT_MASK;}
#define		RESTORE_INTERRUPTS()	{INTCON |= Aux;}

#endif	//	USING_FREE_RTOS == 1
//==============================================================================
void free( S_CLASS void RAM *r )
	{
	struct _AllocBlock RAM *p, *q;
#if			USING_FREE_RTOS != 1
	unsigned char Aux;
#endif	//	USING_FREE_RTOS != 1

	// We will not free a NULL pointer
	if( r == NULL )
		// Finished
		return;

	// Make 'r' point to the true begining of the allocated block
	r = (unsigned char RAM *)r - sizeof __freelist.Length;

	DISABLE_INTERRUPTS();

	// Then we iterate through the free list to find the point where the block will be inserted
	for( p = __freelist.Next, q = &__freelist; p != NULL && p < (struct _AllocBlock RAM *)r; q = p, p = p->Next )
		;	// Empty statement

	// Make the block point to its successor in the free list
	(( struct _AllocBlock RAM *)r)->Next	= p;
	// Make the predecessor block point to the block being freed
	q->Next								= r;

	// If the block is contiguous to its successor, then we merge them
	if( (ptrdiffram_t)r + ((struct _AllocBlock RAM *)r)->Length + sizeof __freelist.Length == (ptrdiffram_t)p )
		{
		// Make the block point to the successor of its successor
		((struct _AllocBlock RAM *)r)->Next		 = p->Next;
		// Extend the block to the end of its successor
		((struct _AllocBlock RAM *)r)->Length	+= p->Length + sizeof __freelist.Length;
		}

	// If the predecessor block is contiguous to this block, then we merge them
	if( (ptrdiffram_t)q + ((struct _AllocBlock RAM *)q)->Length + sizeof __freelist.Length == (ptrdiffram_t)r )
		{
		// Make the predecessor block point to the current block's successor
		((struct _AllocBlock RAM *)q)->Next		 = ((struct _AllocBlock RAM *)r)->Next;
		// Extend the predecessor block to the end of the current block
		((struct _AllocBlock RAM *)q)->Length	+= ((struct _AllocBlock RAM *)r)->Length + sizeof __freelist.Length;
		}

	RESTORE_INTERRUPTS();
	}
//==============================================================================