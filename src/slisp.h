/*	$Id: slisp.h,v 1.8 2001/08/10 15:18:08 sandro Exp $	*/

/*
 * Copyright (c) 1997-2001 Sandro Sigala.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Changes made by HLG
 */

/* Tokens. */
#ifndef SLISP_H

#define IDENTIFIER		1
#define INTEGER			2
#define NGMP			3
#define DOUBLE			4
#define CSTRING			5

/* Lisp objects. */
#define OBJ_NIL			0
#define OBJ_T			1
#define OBJ_INTEGER		10
#define OBJ_IDENTIFIER		11
#define OBJ_NGMP		12
#define OBJ_CSTRING		14
#define OBJ_DOUBLE		13
#define OBJ_CONS		20

typedef struct object *objectp;
/* An object...
   Always have:
    a type (integer)
    a garbage-collector flag (is recyclable or not)
    a constant flag (integer)
    a value (depends of type) maybe one of:
	identifier
	string
	integer
	double
	cons (have 2 elements)
	    car (another object)
	    cdr (another object)
	
	next object
*/
struct object {
		/* The type of the object. */
	int	type;
		/* Flag used during garbage collect. */
	int	gc;

	char	*doc;
	union {
			/* The identifier value. */
		char	*id;
			/* The string value. */
		char	*s;
			/* The integer value. */
		int	i;
			/* Double value */
		double  d;
			/* The cons cell value. */
		struct {
				/* The CAR of the object. */
			objectp	car;
				/* The CDR of the object. */
			objectp	cdr;
		} c;

	} value;
	
  objectp next;
};

#define SLISP_H
#endif
