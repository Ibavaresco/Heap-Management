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
void RAM *malloc( S_CLASS sizeram_t Length )
	{
	struct _AllocBlock RAM *p, *q;
#if			USING_FREE_RTOS != 1
	unsigned char Aux;
#endif	//	USING_FREE_RTOS != 1

	// We will not allocate 0 bytes.
	if( Length == 0 )
		// The allocation failed.
		return NULL;
	
//	// The length should always be even. (Not for 14 or 16 bit (program word) PICs).
//	Length = ( Length + 1 ) & -2;

#if			defined __18CXX || defined _PIC18
	// The length should be at least 2. (Not for 14 bit (program word) PICs)
	if( Length < sizeof __freelist.Next )
		Length = sizeof __freelist.Next;
#endif	//	defined __18CXX || defined _PIC18

	DISABLE_INTERRUPTS();

	// Then we iterate through the free list to find a suitable block.
	for( p = __freelist.Next, q = &__freelist; p != NULL && p->Length < Length; q = p, p = p->Next )
		;	// Empty statement

	// If p is NULL is because there is no suitable block.
	if( p == NULL )
		{
		RESTORE_INTERRUPTS();
		// The allocation failed.
		return NULL;
		}

	// If the block length is not enough to be splitted, we allocate the whole block.
	if( p->Length < Length + sizeof( struct _AllocBlock ))
		{
		// Remove the block from the free list.
		q->Next = p->Next;
		}
	// We have to split the block.
	else
#if			ALLOCATE_FROM_BEGINNING == 1
		{
		// Make the previous block point to the remaining of the block being split.
		q->Next		= (struct _AllocBlock RAM *)( (unsigned char RAM *)p + Length + sizeof __freelist.Length );
		// Make q point to the remaining of the block being split.
		q			= q->Next;
		// Set the lenght of the remaining of the block being split.
		q->Length	= p->Length - Length - sizeof __freelist.Length;
		// Make the remaining of the block being split point to where the original block pointed.
		q->Next		= p->Next;
		// Set the new block length.
		p->Length	= Length;
		}
#else	//	ALLOCATE_FROM_BEGINNING == 1
		{
		// Make q point to the begining of the new block.
		q = (struct _AllocBlock RAM *)( (unsigned char RAM *)p + p->Length - Length );
		// Reduce the original block length.
		p->Length -= Length + sizeof __freelist.Length;
		// Set the new block length.
		p = q;
		p->Length = Length;
		}
#endif	//	ALLOCATE_FROM_BEGINNING == 1

	RESTORE_INTERRUPTS();

	// Return the address of the data area of the block.
	return (unsigned char RAM *)p + sizeof __freelist.Length;
	}
//==============================================================================
