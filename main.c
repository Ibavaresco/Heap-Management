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
/*
	This file is just an example of how to use "Heap Management For Small
	Microcontrollers".
*/
//==============================================================================
#include <alloc.h>
//==============================================================================
char RAM *v[64];
void main( void )
	{
	char i;
	char RAM *p, *q, *r;

	heapinit();

	while( 1 )
		{
		for( i = 0; i < sizeof v / sizeof v[0]; i++ )
			v[i] = malloc( 20 );
	
		p = malloc( 93 );
		q = malloc( 93 );
		r = malloc( 1 );
	
		for( i = 0; i < sizeof v / sizeof v[0]; i += 2 )
			free( v[i] );
	
		for( i = 1; i < sizeof v / sizeof v[0]; i += 2 )
			free( v[i] );
	
		free( q );
		free( p );
		free( r );
		}

	}
//==============================================================================
void		vTaskSuspendAll( void )
	{
	}
//==============================================================================
signed char	xTaskResumeAll( void )
	{
	return 0;
	}
//==============================================================================