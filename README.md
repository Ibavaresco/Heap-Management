Heap management for small microcontrollers
<p>
<p>by Isaac Marino Bavaresco
<p>
<p>    Specially suited for PIC18 with FreeRTOS, but works with Microchip MPLAB-C18, Hi-Tech PICC and Hi-Tech PICC-18
<p>
<p>malloc.c 	Implements the function "malloc()".
<p>malloc.asm 	An optional substitution for the file "malloc.c". It is much more optimized, but works only for PIC18 in extended mode with Microchip MPLAB-18.
<p>free.c 	Implements the function "free()".
<p>free.asm 	An optional substitution for the file "free.c". It is much more optimized, but works only for PIC18 in extended mode with Microchip MPLAB-C18.
<p>__freelist.c 	Declares the free blocks list head.
<p>__heap.c 	Declares the heap data area.
<p>__reclaim_stack.c 	Useful only when used with PIC18 and FreeRTOS.
<p>alloc.h 	Header with defines and prototypes. Should be included in the application files.
<p>heap.h 	Header used by the heap functions.
<p>main.c 	Demo application.
<p>heap_config.h 	Defines for configuring the heap parameters
