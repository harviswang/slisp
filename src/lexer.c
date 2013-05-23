/*	$Id: lexer.c,v 1.9 2001/08/10 15:18:08 sandro Exp $	*/

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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slisp.h"
#include "extern.h"

FILE *input_file;
unsigned int lineno;

#define LEX_BUF_MAX 128

static int lex_buf[LEX_BUF_MAX];
static int *lex_bufp;

#ifdef HAVE_READLINE

__inline__ int xgetc()
{
	if(lex_bufp > lex_buf) {
		return(*--lex_bufp);
	} else {
		if(input_file != stdin)
			return(fgetc(input_file));
		else
			return(getchl()); 
	}
}

#else

#define xgetc()	((lex_bufp > lex_buf) ? *--lex_bufp :  fgetc(input_file))

#endif


#define xungetc(c)	*lex_bufp++ = c

char *token_buffer;
static int token_buffer_max;

void init_lex(void)
{
	lineno = 1;
	token_buffer_max = 10;
	token_buffer = (char *)xmalloc(token_buffer_max);
	lex_bufp = lex_buf;
}

void done_lex(void)
{  
	free(token_buffer);
}

static char *extend_buf(char *p)
{
	int off = p - token_buffer;
	
	token_buffer_max += 10;
	token_buffer = (char *)xrealloc(token_buffer, token_buffer_max);
	
	return token_buffer + off;
}

__inline__ int gettoken(void)
{
	register char *p;
	register int c;

	for (;;) {
	a:
		c = xgetc();
		
		switch (c) {
			case '\n':
				++lineno;
				break;
				
			case ' ':
			case '\f':
			case '\t':
			case '\v':
			case '\r':
				break;
				
			case ';': /* Comment: ';'.*'\n' */
				while ((c = xgetc()) != '\n')
					;
				++lineno;
				break;
				
			case '?': /* Character: "?". */
				c = xgetc();
				sprintf(token_buffer, "%d", c);
				return INTEGER;
				
			case '-': /* Minus sign: "-". */
				c = xgetc();
				if (!isdigit(c)) {
					xungetc(c);
					c = '-';
					goto got_id;
				}
				xungetc(c);
				c = '-';
				
				/* FALLTRHU */
			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
				/* Integer: [0-9]+ */
				/* Double : [0-9]+\.[0-9]+ */
				/* Note that 123a = 123 cause it ignores chars */
				p = token_buffer;
				do {
					if (p - token_buffer >= token_buffer_max)
						p = extend_buf(p);
					*p++ = c;
					c = xgetc();
				} while (isdigit(c));

				if(isalpha(c)) {
					xungetc(c);
					goto a;
				} else if(c!= '.') {
					xungetc(c);
					*p='\0';
#ifdef HAVE_GMP
					return NGMP;
#else
					return INTEGER;
#endif				    	
				} else {
					*p++='.';
					c=xgetc();
					if(isdigit(c)) {
						do {
							if (p - token_buffer >= token_buffer_max)
								p = extend_buf(p);
							
							*p++ = c;
							c = xgetc();
						} while (isdigit(c));
						xungetc(c);
						*p = '\0';
#ifdef HAVE_GMP
						return NGMP;
#else
						return DOUBLE;
#endif
						
					} 
				}
		got_id:
			case '_': case '+': case '*': case '/': case '%':
			case '<': case '>': case '=': case '&': case '$': case ':':
			case '^': case '#': case '\\': case '@': case '~': case '|':
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
			case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
			case 's': case 't': case 'u': case 'v': case 'w': case 'x':
			case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
			case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
			case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
			case 'Y': case 'Z':
				/*Identifier:[-/+*%<>=&#^$a-zA-Z_][-/+*%<>=&a-zA-Z_0-9]* */
				p = token_buffer;
				do {
					if (p - token_buffer >= token_buffer_max)
						p = extend_buf(p);
					
					*p++ = c;
					c = xgetc();
				} while (isalnum(c) || strchr("_-+*/%<>=&", c) != NULL);
				xungetc(c);
				*p = '\0';
				return IDENTIFIER;
				
			case '"':
				/* String: "\""([^"]|"\\".)*"\"" */
				p = token_buffer;
				while ((c = xgetc()) != '"' && c != EOF) {
					if (p - token_buffer >= token_buffer_max)
						p = extend_buf(p);
					if (c == '\\') {
						
						c = xgetc();
						
						switch (c) {
							case '\n': 
								++lineno;
								break;
							case 'a':
								*p++ = '\a';
								break;
							case 'b':
								*p++ = '\b';
								break;
							case 'f':
								*p++ = '\f';
								break;
							case 'n':
								*p++ = '\n';
								break;
							case 'r':
								*p++ = '\r';
								break;
							case 't':
								*p++ = '\t';
								break;
							case 'v':
								*p++ = '\v';
								break;
							default:
								*p++ = c;
					}
					} else {
						if (c == '\n')
							++lineno;
						*p++ = c;
					}
				}
				*p = '\0';
				return CSTRING;
				
			default:
				return c;
		}
		
	}
	
}

