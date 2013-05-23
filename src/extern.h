/*	$Id: extern.h,v 1.9 2001/08/10 15:18:08 sandro Exp $	*/

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
 */

/* main.c */
#ifndef EXTERN_H

extern int opt_verbose;
extern int opt_lint;
extern int getchl(void);
extern char *buffer;

/* object.c */

extern objectp nil;
extern objectp t;

extern int free_objs;
extern int used_objs;
extern void init_objects(void);
extern objectp new_object(int type);
extern objectp search_object_identifier(char *);
extern objectp search_object_string(char *);
extern objectp search_object_cstring(char *);
extern objectp search_object_integer(int );
extern objectp search_object_double(double);
extern void set_object(objectp name, objectp value);
extern objectp get_object(objectp name);
extern void dump_objects(char *fname);
extern void print_obj_lists(void);
extern void garbage_collect(void);
extern void print_objects(void);
extern char* stripwhite(char *);

/* parser.c */

extern objectp parse_object(int);

/* lexer.c */

extern FILE *input_file;
extern char *token_buffer;
extern unsigned int lineno;

extern void init_lex(void);
extern void done_lex(void);
extern int gettoken(void);

/* funcs.c */
extern objectp F_eval(objectp);
extern void free_object(objectp);
extern void princ_object(FILE *, objectp);
extern void print_list(void);
extern int pro_file(char *);
extern objectp eval(objectp); 

/* helpers.c */
extern int isprime(unsigned int);
extern void dec2bin(int);
extern int bin2dec(char *);
extern int modexp(int ,int ,int );
extern int num_string(char *);
extern int gcd(int ,int );
extern int JACOBI(int , int );
extern int inverse(int , int);
extern char *randomstring(void);
extern void handler(int);
extern void usage(void);
extern void print_logo(void);
extern unsigned int phi(unsigned int);
extern int down(const void *, const void *);
extern int up(const void *, const void *);
extern void sort_strings(char **, int, int);
extern void handsig(int);

#define EXTERN_H
#endif
