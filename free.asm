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
#define	LOCALS_SIZE	6
DISABLE_INTERRUPTS macro
		call	vTaskSuspendAll
		endm

RESTORE_INTERRUPTS macro
		call	xTaskResumeAll
		endm
;-------------------------------------------------------------------------------
#else	; USING_FREE_RTOS == 1
;-------------------------------------------------------------------------------
#define	LOCALS_SIZE	7
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
#define	r	0
#define	p	4
#define	q	6
#define Aux2	8
#define Aux	10

		extern	__freelist
;===============================================================================
		global	free
;15:		void free( void ram *r )

free:		movff	FSR2L, POSTINC1
		movff	FSR2H, POSTINC1
		movff	FSR1L,FSR2L
		movff	FSR1H,FSR2H

		subfsr	2,4

;16:			{
;17:			struct _AllocBlock ram	*p, *q;
;			unsigned short		Aux2;
;			unsigned char		Aux;

		addfsr	1,LOCALS_SIZE
;18:
;19:			if( r == NULL )

		movf	[r+0],w
		iorwf	[r+1],w
;20:				return;
		bnz	NotNULL
		bra	Finish
;21:		
;22:			r = (unsigned char *)r - 2;
NotNULL:	movlw	0x02
 		subwf	[r+0],f
		movlw	0
		subwfb	[r+1],f

		DISABLE_INTERRUPTS
;23:		
;24:			
;25:			for( p = __freelist.Next, q = &__freelist; p != NULL && p < (struct _AllocBlock ram *)r; q = p, p = p->Next )

		movlw	low __freelist
		movwf	[q+0]
		movlw	high __freelist
		movwf	[q+1]

		movff	__freelist+2,FSR0L	; p = __freelist.Next;
		movff	__freelist+3,FSR0H

Loop:		movf	FSR0L,w,ACCESS		; p != NULL
		iorwf	FSR0H,w,ACCESS
		bz	xa630

		movf	[r+0],w			; && p < (struct _AllocBlock ram *)r;
		subwf	FSR0L,w,ACCESS
		movf	[r+1],w
		subwfb	FSR0H,w,ACCESS
		bc	xa630

		movf	FSR0L,w,ACCESS		; q = p;
		movwf	[q+0]
		movf	FSR0H,w,ACCESS
		movwf	[q+1]

		addfsr	0,2			; p = p->Next );
		movf	POSTINC0,w,ACCESS
		movff	INDF0,FSR0H
		movwf	FSR0L,ACCESS

		bra	Loop
;26:				;	/* Empty statement */

xa630:		movf	FSR0L,w
		movwf	[p+0]
		movf	FSR0H,w
		movwf	[p+1]

;29:			q->Next					= r;
		movsf	[q+0],FSR0L
		movsf	[q+1],FSR0H
		addfsr	0,2
		movsf	[r+0],POSTINC0
		movsf	[r+1],POSTDEC0

;28:			((struct _AllocBlock ram *)r)->Next	= p;
		movsf	[r+0],FSR0L
		movsf	[r+1],FSR0H
		addfsr	0,2
		movsf	[p+0],POSTINC0
		movsf	[p+1],POSTDEC0

;31:			if( (unsigned short)r + ((struct _AllocBlock ram *)r)->Length + 2 == (unsigned short)p )

		;movsf	[r+0],FSR0L
		;movsf	[r+1],FSR0H
		subfsr	0,2

		movf	POSTINC0,w,ACCESS
		addwf	[r+0],w
		movwf	PRODL,ACCESS
		movf	POSTDEC0,w,ACCESS
		addwfc	[r+1],w
		movwf	PRODH,ACCESS

		movlw	0x02
		addwf	PRODL,w,ACCESS
		xorwf	[p+0],w
		bnz	xa68e

		movlw	0x00
		addwfc	PRODH,w,ACCESS
		xorwf	[p+1],w
		bnz	xa68e

;32:				{
;34:				((struct _AllocBlock ram *)r)->Length	+= p->Length + 2;
;33:				((struct _AllocBlock ram *)r)->Next	 = p->Next;

		movsf	[p+0],FSR0L		; FSR0 = p;
		movsf	[p+1],FSR0H

		movlw	0x02			; Aux2 = p->Length + 2;
		addwf	POSTINC0,w,ACCESS
		movwf	[Aux2+0]
		movlw	0x00
		addwfc	POSTINC0,w,ACCESS
		movwf	[Aux2+1]

		movff	POSTINC0,PRODL		; PROD = p->Next;
		movff	POSTINC0,PRODH

		movsf	[r+0],FSR0L		; FSR = r;
		movsf	[r+1],FSR0H

		movf	[Aux2+0],w		; r->Length += Aux2;
		addwf	POSTINC0,f,ACCESS
		movf	[Aux2+1],w
		addwfc	POSTINC0,f,ACCESS

		movff	PRODL,POSTINC0		; r->Next = PROD;
		movff	PRODH,POSTINC0
;35:				}
;36:		
;37:			if( (unsigned short)q + ((struct _AllocBlock ram *)q)->Length + 2 == (unsigned short)r )

xa68e:		movsf	[q+0],FSR0L		; FSR0 = q;
		movsf	[q+1],FSR0H

		movf	POSTINC0,w,ACCESS	; PROD = q->Length;
		addwf	[q+0],w
		movwf	PRODL,ACCESS
		movf	POSTDEC0,w,ACCESS
		addwfc	[q+1],w
		movwf	PRODH,ACCESS

		movlw	0x02			; if( PROD + 2 == r )
		addwf	PRODL,w,ACCESS		;    {
		xorwf	[r+0],w
		bnz	Epilog

		movlw	0x00
		addwfc	PRODH,w,ACCESS
		xorwf	[r+1],w
		bnz	Epilog

;38:				{

;40:				((struct _AllocBlock ram *)q)->Length	+= ((struct _AllocBlock ram *)r)->Length + 2;
;39:				((struct _AllocBlock ram *)q)->Next	 = ((struct _AllocBlock ram *)r)->Next;

		movsf	[r+0],FSR0L		; FSR0 = r;
		movsf	[r+1],FSR0H

		movlw	0x02			; Aux2 = r->Length + 2;
		addwf	POSTINC0,w,ACCESS
		movwf	[Aux2+0]
		movlw	0x00
		addwfc	POSTINC0,w,ACCESS
		movwf	[Aux2+1]

		movff	POSTINC0,PRODL		; PROD = r->Next;
		movff	POSTINC0,PRODH

		movsf	[q+0],FSR0L		; FSR0 = q;
		movsf	[q+1],FSR0H

		movf	[Aux2+0],w		; q->Length += Aux2;
		addwf	POSTINC0,f,ACCESS
		movf	[Aux2+1],w
		addwfc	POSTINC0,f,ACCESS

		movff	PRODL,POSTINC0		; q->Next = PROD;
		movff	PRODH,POSTINC0
;41:				}
;42:			}


Epilog:		RESTORE_INTERRUPTS
Finish:		subfsr	1,LOCALS_SIZE + 1

		movff	POSTDEC1,FSR2H
		movff	INDF1,FSR2L
		return	0
;===============================================================================
		end
;===============================================================================