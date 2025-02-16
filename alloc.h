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
#ifndef		__ALLOC_H__
#define		__ALLOC_H__
//==============================================================================
#include <stddef.h>

#if			defined __18CXX	// PIC18 family with Microchip MPLAB-C18 compiler

#define	RAM		far ram
#define	S_CLASS	auto

#elif		defined _PIC14	// PIC16 family with Hi-Tech PICC compiler

#define	RAM		 bank2
#define	S_CLASS
typedef unsigned char sizeram_t;
typedef unsigned char ptrdiffram_t;

#elif		defined _PIC18	// PIC18 family with Hi-Tech PICC-18 compiler

#define	RAM
#define	S_CLASS
typedef unsigned short sizeram_t;
typedef unsigned short ptrdiffram_t;

#else	//	defined _PIC18

#error "Architecture not supported!"

#endif	//	defined __18CXX
/*=========================================================================*//**
\brief Initializes the heap.

Must be called before any call to other functions that use the heap.
*//*==========================================================================*/
void		heapinit( void );

/*=========================================================================*//**
\brief Allocates a block of RAM memory in the heap.

\param len Length of the block in bytes.

\return In case of success returns a pointer to the allocated block,
in case of failure, returns ::NULL.
*//*==========================================================================*/
void RAM *malloc( S_CLASS sizeram_t len );

/*=========================================================================*//**
\brief Frees a block of RAM memory previously allocated by the function ::malloc.

\param r Pointer to the block to free.
*//*==========================================================================*/
void		free( S_CLASS void RAM *r );

//##############################################################################
#if			defined __18CXX
//##############################################################################

/*=========================================================================*//**
\brief Annex the original system stack to the heap.

Should be called only once, inside a thread, after the call to the functions
heapinit() and vTaskStartScheduler(), when the original stack is no more in use.



This function works only for the combination PIC18 microcontrollers +
MPLAB-C18 compiler + FreeRTOS (www.freertos.org).
*//*==========================================================================*/
void		__reclaim_stack( void );

//##############################################################################
#endif	//	defined __18CXX
//##############################################################################

//==============================================================================
#endif	//	__ALLOC_H__
//==============================================================================