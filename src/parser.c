/*	$Id: parser.c,v 1.8 2001/08/10 15:18:08 sandro Exp $	*/

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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "slisp.h"
#include "extern.h"

static int thistoken;
extern objectp F_eval(objectp);

static objectp parse_form(void)
{
	objectp p, first = NULL, prev = NULL;
	
	while ((thistoken = gettoken()) != ')' && thistoken != EOF) {
		/*
		 * Parse a dotted pair notation.
		 */
		if (thistoken == '.') {
			thistoken = gettoken();
			if (prev == NULL) 
				warn("unexpected `.'");
			
			prev->value.c.cdr = parse_object(1);
			/* This warning only happens when reading files */
			
			if ((thistoken = gettoken()) != ')')
				warn("expected `)'");
			break;
		}
		p = new_object(OBJ_CONS);
		
		if (first == NULL)
			first = p;
		if (prev != NULL)
			prev->value.c.cdr = p;
		
		p->value.c.car = parse_object(1);
		prev = p;
	};
	
	if (first == NULL)
		return nil;
	else
		return first;
}

static objectp parse_quote(void)
{
	objectp p;
	
	p = new_object(OBJ_CONS);
	
	p->value.c.car = new_object(OBJ_IDENTIFIER);
	p->value.c.car->value.id = xstrdup("quote");
	p->value.c.cdr = new_object(OBJ_CONS);
	p->value.c.cdr->value.c.car = parse_object(0);
	
	return p;
}

static objectp parse_backquote(void)
{
	objectp p;
	
	p = new_object(OBJ_CONS);
	
	p->value.c.car = new_object(OBJ_IDENTIFIER);
	p->value.c.car->value.id = xstrdup("backquote");
	p->value.c.cdr = parse_object(0);
	
	return p;
}

static objectp parse_comma(void)
{
	objectp p;
	
	p = new_object(OBJ_CONS);
	
	p->value.c.car = new_object(OBJ_IDENTIFIER);
	p->value.c.car->value.id = xstrdup("comma");
	p->value.c.cdr = new_object(OBJ_CONS);
	p->value.c.cdr->value.c.car = parse_object(0);
	
	return p;
}

objectp parse_object(int havetoken)
{
	objectp p = NULL;
	int i;
	double d;

	if (!havetoken)
		thistoken = gettoken();
	
	switch (thistoken) {
		case EOF:
			break;
		case '(':
			p = parse_form();
			break;
		case '\'':
			p = parse_quote();
			break;
		case '`':
			p = parse_backquote();
			break;
		case ',':
			p = parse_comma();
			break;
			
		case IDENTIFIER:
			if (!strcmp(token_buffer, "t"))
				p = t;
			else if (!strcmp(token_buffer, "nil")) {
				p = nil;
			} else {
				if ((p = search_object_identifier(token_buffer)) == NULL) {
					p = new_object(OBJ_IDENTIFIER);
					p->value.id = xstrdup(token_buffer);
				}
			}
			break;
			
		case INTEGER:
			i = (int) strtol(token_buffer, (char **) NULL, 0);
			if ((p = search_object_integer(i)) == NULL) {
				p = new_object(OBJ_INTEGER);
				p->value.i = i;
		}
			break;
			
		case DOUBLE:
			d = strtod(token_buffer, (char **)NULL);
			if ((p =(objectp) search_object_double(d)) == NULL) {
				p = new_object(OBJ_DOUBLE);
				p->value.d = d;
			}
			break;
#ifdef HAVE_GMP			
		case NGMP: 
			if ((p = search_object_string(token_buffer)) == NULL) {
				p = new_object(OBJ_NGMP);
				p->value.s = xstrdup(token_buffer);
			}
			break;
#endif
		case CSTRING:
			if ((p = search_object_cstring(token_buffer)) == NULL) {
				p = new_object(OBJ_CSTRING);
				p->value.s = xstrdup(token_buffer);
			}
			break;		

		default:
			printf("%d: unexpected character `%c'\n",lineno, thistoken);
			return nil;
	}
	
	return p;
}

