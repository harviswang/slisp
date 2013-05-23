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
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * HLG */

#ifndef FUNCS_H
#include "slisp.h"

typedef struct {
	char *name;
	objectp (*func)(objectp args);
} funcs;

typedef enum {	false,	true } bool;

#define FILES_N 8

FILE *fds[FILES_N]={NULL};
int used[FILES_N] = {0};

int count_list(objectp );
__inline__ objectp car(objectp );
objectp F_quit(objectp );
__inline__ objectp F_car(objectp );
__inline__ objectp cdr(objectp );
objectp time_f(objectp );
objectp F_rest(objectp );
__inline__ objectp F_cdr(objectp );
objectp F_used_obj(objectp );
objectp F_load(objectp );
objectp F_exp(objectp );
objectp  F_len(objectp );
objectp F_elt(objectp );
objectp F_cos(objectp );
objectp F_acos(objectp );
objectp F_sin(objectp );
objectp F_asin(objectp );
objectp F_tan(objectp );
objectp F_atan(objectp );
objectp F_log(objectp );
objectp F_primep (objectp );
objectp F_powm (objectp );
objectp F_floor(objectp );
objectp F_zerop(objectp );
objectp F_add(objectp );
objectp F_mul(objectp );
objectp F_div(objectp );
objectp F_sub(objectp );
objectp F_mod(objectp );
objectp F_gcd(objectp );
objectp F_sqrt(objectp );
objectp F_pow(objectp );
objectp F_even(objectp );
objectp F_odd(objectp );
objectp F_lt(objectp );
objectp F_gt(objectp );
objectp F_le(objectp );
objectp F_ge(objectp );
objectp F_eq(objectp );
objectp F_powm(objectp );
objectp F_primep(objectp );
objectp F_zerop(objectp );
objectp F_floor(objectp );
objectp F_lt(objectp );
objectp F_le(objectp );
objectp F_gt(objectp );
objectp F_ge(objectp );
objectp F_gcd(objectp );
objectp F_add(objectp );
objectp F_even(objectp );
objectp F_odd(objectp );
objectp F_div(objectp );
objectp F_mod(objectp );
objectp F_sqrt(objectp );
objectp F_pow(objectp );
objectp F_sub(objectp );
objectp F_mul(objectp );
objectp F_eq(objectp );
objectp F_bin2dec(objectp );
objectp F_dec2bin(objectp );
objectp F_atoi(objectp );
objectp F_pi(objectp );
objectp F_phi(objectp ); 
objectp F_inverse(objectp ); 
objectp F_ord(objectp );
objectp F_variance(objectp ); 
objectp F_qres(objectp );
objectp F_jacobi(objectp );
objectp F_res(objectp );
objectp F_divisors(objectp );
objectp F_log10(objectp );
objectp F_max(objectp );
objectp F_min(objectp );
void princ_object(FILE *, objectp );
objectp F_princ(objectp );
objectp F_prinf(objectp );
objectp F_atom(objectp );
objectp F_isnum(objectp );
objectp F_islong(objectp );
objectp F_cons(objectp );
objectp F_list(objectp );
objectp F_cmpn(objectp ); 
objectp F_cmpstr(objectp ); 
objectp F_compar(objectp );
objectp F_quote(objectp );
objectp bquote_cons(objectp );
objectp F_bquote(objectp );
objectp F_comma(objectp );
objectp F_and(objectp );
objectp F_or(objectp );
objectp F_not(objectp );
objectp F_cond(objectp );
objectp F_if(objectp );
objectp F_when(objectp );
objectp F_unless(objectp );
objectp F_while(objectp );
objectp F_progn(objectp );
objectp F_prog1(objectp );
objectp F_prog2(objectp );
objectp F_eqexpr(objectp );
objectp F_set(objectp );
objectp F_setq(objectp );
objectp F_let(objectp );
objectp F_defmacro(objectp );
__inline__ objectp caca(objectp , objectp );
objectp F_defun(objectp );
objectp eval_func(objectp , objectp , bool []);
objectp F_gc(objectp );
objectp F_date(objectp );
objectp F_exec(objectp );
objectp F_yacas(objectp );
objectp F_yacas(objectp );
objectp F_consp(objectp );
objectp F_open(objectp );
objectp F_close(objectp );
objectp F_rewind(objectp );
objectp F_tell(objectp );
objectp F_seek(objectp );
objectp F_read(objectp );
objectp F_write(objectp );
objectp F_dump_memory(objectp );
objectp concaten(objectp, objectp);
objectp F_lor(objectp );
objectp F_land(objectp );
objectp F_lxor(objectp );
objectp F_listf(objectp );
objectp F_sets(objectp );
objectp F_setmax(objectp );
objectp F_shows(objectp );
objectp F_showmax(objectp );
objectp F_lambda(objectp ); 
objectp F_values(objectp );
objectp F_areeq(objectp , objectp );
objectp F_eqlis(objectp );
objectp F_insert (objectp );
objectp F_mix(objectp );
objectp F_setprec(objectp ); 
objectp F_typeof(objectp );
objectp F_funcall(objectp );
objectp F_evale(objectp );
objectp F_evlis(objectp );
objectp F_dotimes(objectp);
objectp F_dotimes(objectp);
objectp eval_cons(objectp);
objectp eval(objectp);
objectp F_eval(objectp);
objectp F_makedoc(objectp);
objectp F_getdoc(objectp);
objectp F_reads(objectp);
objectp F_abs(objectp);
objectp F_labels(objectp);
#define FUNCS_N  125

funcs functions[FUNCS_N] = {
	{ "%", F_mod },
	{ "&dump-memory", F_dump_memory },
	{ "&show-objects", F_used_obj },
	{ "*", F_mul },
	{ "+", F_add},
	{ "-", F_sub},
	{ "/", F_div},
	{ "<", F_lt},
	{ "<=", F_le },
	{ "=", F_eqexpr },
	{ ">", F_gt },
	{ ">=", F_ge },
	{ "\\", F_funcall },
	{ "^", F_pow },
	{ "abs", F_abs },
	{ "acos", F_acos },
	{ "allqres", F_qres },
	{ "and", F_and },
	{ "asin", F_asin },
	{ "atan", F_atan },
	{ "atof", F_atoi },
	{ "atoi", F_atoi },
	{ "atom", F_atom },
	{ "backquote", F_bquote },
	{ "bin2dec", F_bin2dec },
	{ "car", F_car },
	{ "cdr", F_cdr },
	{ "comma", F_comma },
	{ "cond", F_cond },
	{ "cons", F_cons },
	{ "consp", F_consp },
	{ "cos", F_cos },
	{ "date", F_date },
	{ "dec2bin", F_dec2bin },
	{ "defmacro", F_defmacro },
	{ "defun", F_defun },
	{ "divof", F_divisors }, 
	{ "doc", F_getdoc },
	{ "dotimes", F_dotimes },
	{ "elt", F_elt },
	{ "eq", F_eqexpr },
	{ "eql", F_eqlis },
	{ "equal", F_eqexpr },
	{ "eval", F_eval },
	{ "eval*", F_evale },
	{ "evenp",F_even },
	{ "evlis", F_evlis },
	{ "exec", F_exec },
	{ "exp", F_exp },
	{ "expt", F_pow },
	{ "fclose", F_close },
	{ "floor", F_floor },
	{ "fopen", F_open },
	{ "fread", F_read },
	{ "frewind", F_rewind },
	{ "fseek", F_seek },
	{ "ftell", F_tell },
	{ "fwrite", F_write },
	{ "garbage-collect", F_gc },
	{ "gc", F_gc },
	{ "gcd", F_gcd},
	{ "if", F_if },
	{ "insert", F_insert },
	{ "invmodp", F_inverse },
	{ "islong", F_islong },
	{ "isnum", F_isnum },
	{ "jacobi", F_jacobi },
	{ "labels", F_labels },
	{ "len", F_len },
	{ "let", F_let },
	{ "list", F_list },
	{ "listfunctions", F_listf },
	{ "load", F_load },
	{ "log", F_log },
	{ "log10", F_log10 },
	{ "logand", F_land },
	{ "logor", F_lor },
	{ "logxor", F_lxor },
	{ "makedoc", F_makedoc },
	{ "max" , F_max},
	{ "min" , F_min},
	{ "mod" , F_mod },
	{ "not", F_not },
	{ "null", F_not },
	{ "numberp",F_isnum },
	{ "oddp", F_odd },
	{ "or", F_or },
	{ "ord", F_ord },
	{ "pair" , F_mix},
	{ "phi", F_phi },
	{ "pi", F_pi },
	{ "pow", F_pow },
	{ "powm", F_powm },
	{ "primep", F_primep },
	{ "princ", F_princ },
	{ "prinf", F_prinf },
	{ "prog1", F_prog1 },
	{ "prog2", F_prog2 },
	{ "progn", F_progn },
	{ "quit", F_quit },
	{ "quote", F_quote },
	{ "read", F_reads },
	{ "resmod", F_res },
	{ "rest", F_rest },
	{ "set", F_set },
	{ "set-max-params", F_setmax },
	{ "set-prec", F_setprec },
	{ "set-stack-size", F_sets },
	{ "setf", F_setq },
	{ "setq", F_setq },
	{ "show-max-params", F_showmax },
	{ "show-stack-size", F_shows },
	{ "sin", F_sin },
	{ "sort", F_compar },
	{ "sqrt", F_sqrt },
	{ "tan", F_tan },
	{ "time",time_f},
	{ "typeof", F_typeof },
	{ "unless", F_unless },
	{ "values", F_values },
	{ "variance", F_variance },
	{ "when", F_when },
	{ "while", F_while },
	{ "yacas", F_yacas },
	{ "zerop", F_zerop }

};

#define FUNCS_H
#endif
