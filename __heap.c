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
#if			defined	__18CXX	// PIC18 family with Microchip MPLAB-C18 compiler

// For using more than 256 bytes, the linker-script must be changed.
unsigned char	RAM		__heap[HEAP_SIZE];

#elif		defined	_PIC14	// PIC16 family with Hi-Tech PICC compiler

// Works for PIC16F876A and PIC16F877A, using all of bank2 and bank3 (this is the
// maximum one can get from a PIC16).
// Please note that with these definitions, the heap is composed of two non-
// contiguous areas.
// For other PIC16 uC, change the definitions.
unsigned char	RAM		__heap[94];
unsigned char	bank3	__heap2[96];

#elif		defined	_PIC18	// PIC18 family with Hi-Tech PICC-18 compiler

// Hi-Tech PICC-18 is smart, you may use almost all of the RAM memory.
unsigned char	RAM		__heap[3000];

#else	//	defined	_PIC18
	#error "Architecture not supported!"
#endif	//	defined	__18CXX
//==============================================================================
void heapinit( void )
	{
#if			defined __18CXX || defined _PIC18

	((struct _AllocBlock RAM *)__heap)->Length	= sizeof __heap - sizeof __freelist.Length;
	((struct _AllocBlock RAM *)__heap)->Next	= NULL;

#elif		defined _PIC14

	__heap[0]	= sizeof __heap - sizeof __freelist.Length;
	__heap[1]	= (sizeram_t)__heap2;
	__heap2[0]	= sizeof __heap2 - sizeof __freelist.Length;
	__heap2[1]	= NULL;

#endif	//	defined _PIC14

	__freelist.Length		= 0;
	__freelist.Next			= (struct _AllocBlock RAM *)__heap;
	}
//==============================================================================