;===============================================================================
; This file is part of "Heap Management For Small Microcontrollers".
; v1.05 (2009-06-29)
; isaacbavaresco@yahoo.com.br
;===============================================================================
; Copyright (c) 2007-2009, Isaac Marino Bavaresco
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Neither the name of the author nor the
;       names of its contributors may be used to endorse or promote products
;       derived from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;===============================================================================
#include <P18CXXX.INC>
;===============================================================================
		radix	decimal
;===============================================================================
#include <heap_config.h>
;===============================================================================
#if	  USING_FREE_RTOS == 1
;-------------------------------------------------------------------------------
		extern	vTaskSuspendAll
		extern	xTaskResumeAll
#define	LOCALS_SIZE	4
DISABLE_INTERRUPTS macro
		call	vTaskSuspendAll
		endm

RESTORE_INTERRUPTS macro
		call	xTaskResumeAll
		endm
;-------------------------------------------------------------------------------
#else	; USING_FREE_RTOS == 1
;-------------------------------------------------------------------------------
#define	LOCALS_SIZE	5
DISABLE_INTERRUPTS macro
;			Aux	 = INTCON & 0xc0;
		movlw	0xc0
		andwf	INTCON,w,ACCESS
		movwf	[Aux+0]
;			INTCON	&= 0x3f;
		movlw	0x3f
		andwf	INTCON,f,ACCESS
		endm

RESTORE_INTERRUPTS macro
;			INTCON	|= Aux;
		movf	[Aux+0],w
		iorwf	INTCON,f,ACCESS
		endm
;-------------------------------------------------------------------------------
#endif	; USING_FREE_RTOS == 1
;===============================================================================
ALLOC		code
;===============================================================================
#define Length	0
#define	p	4
#define q	6
#define	Aux	8

		extern	__freelist
		extern	vTaskSuspendAll
		extern	xTaskResumeAll
;===============================================================================
		global	malloc
;		void ram *malloc( sizeram_t Length )

malloc:		movff	FSR2L,POSTINC1
		movff	FSR2H,POSTINC1
		movff	FSR1L,FSR2L
		movff	FSR1H,FSR2H

		subfsr	2,4

;			{
;			struct _AllocBlock ram	*p,*q;
;		#if		USING_FREE_RTOS != 1
;			unsigned char Aux;
;		#endif	//	USING_FREE_RTOS != 1

		addfsr	1,LOCALS_SIZE

;			// We will not allocate 0 bytes.
;			if( Length == 0 )
		movf	[Length+0],w
		iorwf	[Length+1],w
		bnz	NotZero
;				// The allocation failed.
;				return NULL;
		clrf	PRODL,ACCESS
		clrf	PRODH,ACCESS
		bra	Ret

NotZero:	
;		//	// The length should always be even. (Not for 14 or 16 bit (program word) PICs).
;		//	Length = ( Length + 1 ) & -2;
;
;		infsnz	[Length+0],f
;		incf	[Length+1],f
;		bcf	[Length+0],0

;		#if		defined __18CXX || defined _PIC18
;			// The length should be at least 2. (Not for 14 bit (program word) PICs)
;			if( Length < sizeof __freelist.Next )
;				Length = sizeof __freelist.Next;
;		#endif	//	defined __18CXX || defined _PIC18

		movlw	2
		subwf	[Length+0],w
		movlw	0
		subwfb	[Length+1],w
		bc	TwoOrMore

		movlw	2
		movwf	[Length+0]

TwoOrMore:

;			DISABLE_INTERRUPTS();
		DISABLE_INTERRUPTS

;			// Then we iterate trough the free list to find a suitable block.
;			for( q = &__freelist,p = __freelist.Next; p != NULL && p->Length < Length; q = p, p = p->Next )
;				;	// Empty statement
 
		movlw	low __freelist		; for( q = &__freelist,
		movwf	[q+0]
		movlw	high __freelist
		movwf	[q+1]
 
		movff	__freelist+2,FSR0L	; p = __freelist.Next;
		movff	__freelist+3,FSR0H
 
Loop:		movf	FSR0L,w,ACCESS		; p != NULL
		iorwf	FSR0H,w,ACCESS
		bz	Fail

		movf	[Length+0],w		; && p->Length < Length;
		subwf	POSTINC0,w,ACCESS
		movf	[Length+1],w
		subwfb	POSTDEC0,w,ACCESS
		bc	Found

		movf	FSR0L,w,ACCESS		; q = p,
		movwf	[q+0]
		movf	FSR0H,w,ACCESS
		movwf	[q+1]

		addfsr	0,2			; p = p->Next );
		movf	POSTINC0,w,ACCESS
		movff	INDF0,FSR0H
		movwf	FSR0L,ACCESS

		bra	Loop

;			// If p is NULL is because there is no suitable block.
;			if( p == NULL )
;				{
;				RESTORE_INTERRUPTS();
;				// The allocation failed.
;				return NULL;
;				}

#if	  USING_FREE_RTOS == 1
Fail:		clrf	[p+0]
		clrf	[p+1]
#else	; USING_FREE_RTOS == 1
Fail:		clrf	PRODL,ACCESS
		clrf	PRODH,ACCESS
#endif	; USING_FREE_RTOS == 1
		bra	Epilog

;			// If the block length is not enough to be splitted, we allocate the whole block.
;			if( p->Length < Length + sizeof( struct _AllocBlock ))
;				{

Found:		movf	FSR0L,w
		movwf	[p+0]
		movf	FSR0H,w
		movwf	[p+1]

		movlw	0x04
		addwf	[Length+0],w
		movwf	PRODL,ACCESS
		movlw	0x00
		addwfc	[Length+1],w
		movwf	PRODH,ACCESS

		movf	PRODL,w,ACCESS
		subwf	POSTINC0,w,ACCESS
		movf	PRODH,w,ACCESS
		subwfb	POSTDEC0,w,ACCESS
		bc	SplitBlock

;				// Remove the block from the free list.
;				q->Next = p->Next;
 
AllocateWhole:	addfsr	0,2

		movff	POSTINC0,PRODL
		movff	POSTDEC0,PRODH
 
 		movsf	[q+0],FSR0L
		movsf	[q+1],FSR0H
		addfsr	0,2

		movff	PRODL,POSTINC0
		movff	PRODH,POSTDEC0

		bra	Finish
;				}
;			// We have to split the block.
;			else
;				{
#if		ALLOCATE_FROM_BEGINNING == 1

;				// Make the previous block point to the remaining of the block being split.
;				q->Next		= (struct _AllocBlock RAM *)( (unsigned char RAM *)p + Length + sizeof __freelist.Length );

SplitBlock:	movsf	[q+0],FSR0L
		movsf	[q+1],FSR0H
		addfsr	0,2

		movf	[p+0],w
		addwf	[Length+0],w
		movwf	[q+0]

		movf	[p+1],w
		addwfc	[Length+1],w
		movwf	[q+1]

		movlw	2
		addwf	[q+0],f
		movlw	0
		addwfc	[q+1],f

;				// Make q point to the remaining of the block being split.
;				q			= q->Next;

		movsf	[q+0],POSTINC0
		movsf	[q+1],POSTDEC0

;				// Set the lenght of the remaining of the block being split.
;				q->Length	= p->Length - Length - sizeof __freelist.Length;

		movsf	[p+0],FSR0L
		movsf	[p+1],FSR0H

		movf	[Length+0],w
		subwf	POSTINC0,w,ACCESS
		movwf	PRODL,ACCESS

		movf	[Length+1],w
		subwfb	POSTDEC0,w,ACCESS
		movwf	PRODH,ACCESS

		movlw	2
		subwf	PRODL,f,ACCESS
		movlw	0
		subwfb	PRODH,f,ACCESS

		movsf	[q+0],FSR0L
		movsf	[q+1],FSR0H

		movff	PRODL,POSTINC0
		movff	PRODH,POSTDEC0

;				// Set the new block length.
;				p->Length	= Length;

		movsf	[p+0],FSR0L
		movsf	[p+1],FSR0H
		movsf	[Length+0],POSTINC0
		movsf	[Length+1],POSTINC0

;				// Make the remaining of the block being split point to where the original block pointed.
;				q->Next		= p->Next;

		movff	POSTINC0,PRODL
		movff	POSTDEC0,PRODH

		movsf	[q+0],FSR0L
		movsf	[q+1],FSR0H
		addfsr	0,2
		movff	PRODL,POSTINC0
		movff	PRODH,POSTDEC0


#else	;	ALLOCATE_FROM_BEGINNING == 1

;				// Make p point to the begining of the new block.
;				pTemp = (struct _AllocBlock ram *)( (unsigned char ram *)p + p->Length - Length );

SplitBlock:	movf	POSTINC0,w,ACCESS
		addwf	[p+0],f
		movf	POSTDEC0,w,ACCESS
		addwfc	[p+1],f

		movf	[Length+0],w
		subwf	[p+0],f
		movf	[Length+1],w
		subwfb	[p+1],f

;				// Reduce the original block length.
;				p->Length -= Length + sizeof __freelist.Length;

		movf	[Length+0],w
		subwf	POSTINC0,f,ACCESS
		movf	[Length+1],w
		subwfb	POSTDEC0,f,ACCESS

		movlw	0x02
		subwf	POSTINC0,f,ACCESS
		movlw	0x00
		subwfb	POSTDEC0,f,ACCESS


;				// Set the new block length.
;				p = pTemp;
		movsf	[p+0],FSR0L
		movsf	[p+1],FSR0H

;				p->Length = Length;
		movsf	[Length+0],POSTINC0
		movsf	[Length+1],POSTDEC0

#endif	;	ALLOCATE_FROM_BEGINNING == 1
;				}
;

;			RESTORE_INTERRUPTS();

;			// return the address of the data area of the block.
;			return (unsigned char RAM *)p + sizeof __freelist.Length;
 
;			}

#if	  USING_FREE_RTOS == 1
Finish:		movlw	0x02
		addwf	[p+0],f
		movlw	0x00
		addwfc	[p+1],f
Epilog:		RESTORE_INTERRUPTS
		movsf	[p+0],PRODL
		movsf	[p+1],PRODH
#else	; USING_FREE_RTOS == 1
Finish:		movlw	0x02
		addwf	[p+0],w
		movwf	PRODL,ACCESS
		movlw	0x00
		addwfc	[p+1],w
		movwf	PRODH,ACCESS
Epilog:		RESTORE_INTERRUPTS
#endif	; USING_FREE_RTOS == 1

Ret:		subfsr	1,LOCALS_SIZE + 1
		movff	POSTDEC1,FSR2H
		movff	INDF1,FSR2L
		return	0
;===============================================================================
		end
;===============================================================================