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
 * Changes made by HLG */

#include "config.h"


#ifdef HAVE_GMP

#ifdef BSD
#include "/usr/local/include/gmp.h"
#endif

#ifdef LINUX
#include "gmp.h"
#endif

#endif

#ifdef HAVE_YACAS

#ifdef BSD
#include "/usr/local/share/yacas/include/cyacas.h"
#endif

#ifdef LINUX
#include "/usr/share/yacas/include/cyacas.h"
#endif

#endif 

#ifdef LINUX
#include <strings.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "slisp.h"
#include "extern.h"
#include "funcs.h"

extern int breakn(void);
extern pid_t fork(void);
extern int snprintf(char *, size_t, const char *, ...);
objectp F_eval(objectp);
objectp F_progn(objectp);
objectp F_prog1(objectp);
objectp F_prog2(objectp);
objectp eval_cons(objectp);
int show_f(void);

extern int MAX_USED;

/* Maxium number of parameters allowed by */
/* let bindings,  and functions's arguments */
int MAX_PARAMS=64; 
/* GMP PRECISION */
int PRECISION=128;

int count_list(objectp p)
{
	register int i = 0;
	
	while (p != nil && p->type == OBJ_CONS)
		p = p->value.c.cdr, ++i;
	
	return i;
}

__inline__ objectp car(objectp p)
{
	if (p->type == OBJ_CONS)
		return p->value.c.car;
	
	if (opt_lint && p != nil && p != t)
		warnx("`CAR' warning: non cons object");
	
	return nil;
}

objectp F_quit(objectp args)
{
	printf("Bye!\n");
	exit(0);
	/* nice hack */
	return t;
}


/*
 * Syntax: (car expr)
 */

__inline__ objectp F_car(objectp args)
{
	return car(eval(car(args)));
}


__inline__ objectp cdr(objectp p)
{
	if (p->type == OBJ_CONS)
		return p->value.c.cdr;
	
	if (opt_lint && p != nil && p != t)
		warnx("`CDR' warning: non cons object");
	
	return nil;
}

objectp time_f(objectp args)
{
	objectp p;
	time_t  t0, t1; 
	clock_t c0, c1; 
	t0 = time(NULL);
	c0 = clock();
	
	p=F_eval(args);    
	
	t1 = time(NULL);
	c1 = clock();
	
	printf ("\treal:\t%ld\n",(long)(t1 - t0));
	printf ("\tuser:\t%.8f\n",(float)(c1 - c0)/CLOCKS_PER_SEC);
	princ_object(stdout,p);
	printf("\n");
	
	return t;
}	

objectp F_rest(objectp args)
{
	return(cdr(eval(car(args))));
}

/*
 * Syntax: (cdr expr)
 */

__inline__ objectp F_cdr(objectp args)
{
	return cdr(eval(car(args)));
}

/*
 * Syntax (&show-objects)
 */

objectp F_used_obj(objectp args)
{
	print_list();
	return t;
}

/*
 * Syntax (load "filename")
 */

objectp F_load(objectp args)
{
	objectp p;
	p=eval(car(args));
	
	if(pro_file(p->value.s) == -1) {
		input_file=stdin;
		return nil;
	}
	return t;
}
/*
 * Syntax(exp n)
 */

objectp F_exp(objectp args)
{
	objectp p,p1;
	double d,e;
	
	p = eval(car(args));
	d = e = 0;

#ifdef HAVE_GMP
	if (p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = exp(d);
	}
#else
	if(p->type == OBJ_INTEGER) {
		e =exp(p->value.i);
	} else if (p->type == OBJ_DOUBLE) {
		e=exp(p->value.d);
	}
#endif
	else if(opt_lint) {
		warnx("`EXP' warning: non numeric value");
		return nil;
	} else
		return nil;
	
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
	
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
	
#endif
	return p1;
	
}

/*
 * Syntax: (len list)
 */

objectp  F_len(objectp args)
{
	objectp p,p1;
	register int v = 0;
	
	p = eval(car(args));
	
	if(p->type == OBJ_CONS) {
		while(p!=nil) {
			p = cdr(p);
			++v;
		}
	} else if(p->type == OBJ_CSTRING) {
		v=strlen(p->value.s);
	} else 
		return nil;
	
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%d",v);
#else
	p1 = new_object(OBJ_INTEGER);
	p1->value.i = v;
#endif
	return p1;
	
	
}


/*
 * Syntax (elt list n)
 */
objectp F_elt(objectp args)
{
	objectp p,p1;
	register int v;
	
	p = eval(car(args));
	p1 = eval(car(cdr(args)));
	
#ifdef HAVE_GMP
	if(p1->type == OBJ_NGMP)
		v=strtol(p1->value.s, (char **) NULL,0);
#else
	if(p1->type == OBJ_INTEGER)
		v=p1->value.i;
#endif
	
	else if(opt_lint) {
		warnx("`ELT' warning: 2nd arg must be an integer");
		return nil;
	} else
		return nil;
	for( ; v > 0 ; v--) {
		if(p==nil)
			return nil;
		p=cdr(p);
	}
	if(p==nil)
		return nil;
	p=car(p);
	return p;
}

/*
 * Syntax: (cos n)
 */

objectp F_cos(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif
	
	p = eval(car(args));

#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE)
		e=cos(p->value.d);
	else if ( p->type == OBJ_INTEGER)
		e=cos(p->value.i);
#endif
	
#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = cos(d);
	}
#endif
	else if (opt_lint) { 
		warnx("`COS' warning: non numeric value");
		return nil;
	} else
		return nil;
	
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;
	
}
/*
 * Syntax: (acos n)
 */

objectp F_acos(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif

	p = eval(car(args));
#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE)
		e=acos(p->value.d);
	else if ( p->type == OBJ_INTEGER)
		e=acos(p->value.i);
#endif

#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = acos(d);
	}
#endif
	else if (opt_lint) {
		warnx("`ACOS' warning: non numeric value");
		return nil;
	} else 
		return nil;
	
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;

}

/*
 * Syntax: (sin n)
 */

objectp F_sin(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif

	p = eval(car(args));

#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE)
		e=sin(p->value.d);
	else if( p->type == OBJ_INTEGER)
		e=sin(p->value.i);
#endif
	
#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = sin(d);
	}
#endif
	else if(opt_lint) {
		warnx("`SIN' warning: non numeric value");
		return nil;
	} else
		return nil;
	
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;

}

/*
 * Syntax: (asin n)
 */

objectp F_asin(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif

	p = eval(car(args));


#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE) 
		e=asin(p->value.d);
	 else if( p->type == OBJ_INTEGER)
		e=asin(p->value.i);
#endif

#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = asin(d);
	}
#endif
	else if(opt_lint) {
		warnx("`ASIN' warning: non numeric value");
		return nil;
	} else 
		return nil;
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;

}

/*
 * Syntax: (tan n)
 */

objectp F_tan(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif
	
	p = eval(car(args));
	
#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE)
		e=tan(p->value.d);
	 else if( p->type == OBJ_INTEGER)
		e=tan(p->value.i);
#endif
	
#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = tan(d);
	}
#endif
	else if(opt_lint) {
		warnx("`TAN' warning: non numeric value");
		return nil;
	} else
		return nil;
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;

}

/*
 * Syntax: (atan n)
 */

objectp F_atan(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif

	p = eval(car(args));
	p1 = new_object(OBJ_DOUBLE);
#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE)
		e=atan(p->value.d);
	else if( p->type == OBJ_INTEGER)
		e=atan(p->value.i);
#endif
	
#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = atan(d);
	}
#endif
	else if(opt_lint) {
		warnx("`ATAN' warning: non numeric value");
		return nil;
	} else
		return nil;
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;

}
/*
 * Syntax: (log n)
 */

objectp F_log(objectp args)
{
	objectp p,p1;
	double e;
#ifdef HAVE_GMP
	double d;
#endif

	p = eval(car(args));

#ifndef HAVE_GMP
	if (p->type == OBJ_DOUBLE)
		e=log(p->value.d);
	else if( p->type == OBJ_INTEGER)
		e=log(p->value.i);
#endif
	
#ifdef HAVE_GMP
	if ( p->type == OBJ_NGMP) {
		d = strtod(p->value.s, (char **) NULL);
		e = log(d);
	}
#endif
	else if(opt_lint) {
		warnx("`LOG' warning: non numeric value");
		return nil;
	} else
		return nil;
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",e);
#else
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = e;
#endif
	return p1;

}

#ifdef HAVE_GMP

/*
 * Syntax: (primep n)
 */


objectp F_primep (objectp args)
{
	mpz_t a;
	objectp p;
	mpz_init(a);
	p=eval(car(args));
	
	if(p->type == OBJ_NGMP) {
		mpz_set_str(a,p->value.s,0);
		if(mpz_probab_prime_p(a,10) != 0)
			return t;
		else
			return nil;
		
	} else if (opt_lint) {
		warnx("`PRIMEP' warning: non integer/numeric operand");
		return nil;
	} else
		return nil;
}

/*
 * Syntax: (expmod a b n)
 * a^b (mod n)
 */

objectp F_powm (objectp args)
{
	objectp b,e,m,p;
	mpz_t base,expt,modulus,result;
	
	b = eval(car(args));
	e = eval(car(cdr(args)));
	m = eval(car(cdr(cdr(args))));
	
	mpz_init(base);
	mpz_init(expt);
	mpz_init(modulus); 
	
	if(b->type == OBJ_NGMP && e->type == OBJ_NGMP && m->type == OBJ_NGMP) {
		mpz_set_str(base,b->value.s,0);
		mpz_set_str(expt,e->value.s,0);
		mpz_set_str(modulus,m->value.s,0);
		
	} else {
		mpz_clear(base);
		mpz_clear(expt);
		mpz_clear(modulus); 
		return nil;
	}
	
	mpz_init(result);
	mpz_powm(result,base,expt, modulus);
	
	p=new_object(OBJ_NGMP);
	p->value.s = (char *) xcalloc(128,sizeof(char));	
	gmp_snprintf(p->value.s,128,"%Zd",result);
	
	mpz_clear(result);
	mpz_clear(base);
	mpz_clear(expt);
	mpz_clear(modulus); 
	
	return p;
}

/*
 * Syntax: (floor n)
 * [2.3] = 2
 */

objectp F_floor(objectp args)
{
	objectp p,p1;
	mpf_t a;
	p=eval(car(args));
	
	if (p->type == OBJ_NGMP) {
		mpf_init(a);
		mpf_set_str(a,p->value.s,0);
		mpf_floor(a,a);
		p1=new_object(OBJ_NGMP);
		
		p1->value.s=(char *) xcalloc(128,sizeof(char));
		gmp_snprintf(p1->value.s,128,"%Zd",a);
		mpf_clear(a);

		return p1;
		
	} else if(opt_lint) {
		warnx("`FLOOR' warning: non numeric value");
		return nil;
	} else 
		return nil;
}

/*
 * Syntax: (zerop n)
 */

objectp F_zerop(objectp args)
{
    mpf_t a,b;
    objectp p1 = car(args);

    if(p1->type == OBJ_NGMP) {
	    mpf_init(a);
	    mpf_init(b);
	    mpf_set_str(b,p1->value.s,0);
	    mpf_set_str(a,"0",0);
		
	    if(mpf_cmp(a,b) == 0) {
		    mpf_clear(a);
		    mpf_clear(b);
		    return t;
	    } else {
		    mpf_clear(a);
		    mpf_clear(b);
		    return nil;
	    }
    } else 
		return nil;
    
    return nil;
    
}


/*
 * Syntax: (+ expr)
 */

objectp F_add(objectp args)
{
	objectp p,p1;
	mpf_t a,b;
	mpf_set_default_prec(PRECISION);
	mpf_init(a);
	mpf_init(b);
	p = args;
	do {
		p1 = eval(car(p));
		
		if(p1->type == OBJ_NGMP) {
			mpf_set_str(b,p1->value.s,0);
			mpf_add(a,b,a);
		} else
			return nil;
		
		p=cdr(p);
		
	} while(p!=nil);
	
	p1=new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p1->value.s,PRECISION,"%.Ff",a);
	mpf_clear(a);
	mpf_clear(b);
	return p1;
}

/*
 * Syntax: (* expr)
 */

objectp F_mul(objectp args)
{
	objectp p,p1;
	mpf_t a,b;
	mpf_set_default_prec(PRECISION);	
	mpf_init(a);
	mpf_init(b);
	mpf_set_str(a,"1",0);
	p = args;
	
	do {
		p1 = eval(car(p));
		
		if(p1->type == OBJ_NGMP) {
			mpf_set_str(b,p1->value.s,0);
			mpf_mul(a,b,a);
		} else
			return nil;
		
		p=cdr(p);
	} while(p!=nil);
	
	p1=new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p1->value.s,PRECISION,"%.Ff",a);
	mpf_clear(a);
	mpf_clear(b);
	return p1;
	
}

/*
 * Syntax: (/ expr)
 */

objectp F_div(objectp args)
{
	objectp p,p1;
	mpf_t a,b;
	mpf_set_default_prec(PRECISION);
	mpf_init(a);
	mpf_init(b);
	p = args;

	do {
	    p1 = eval(car(p));
	    if(p1->type == OBJ_NGMP) {
			mpf_set_str(b,p1->value.s,0);
			if (p == args && cdr(p) != nil)
				mpf_set(a,b);
			else
				mpf_div(a,a,b);
		} else
			return nil;
		
		p=cdr(p);
	} while(p!=nil);
	
	p1=new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p1->value.s,PRECISION,"%.Ff",a);
	
	mpf_clear(a);
	mpf_clear(b);
	return p1;
}

/*
 * Syntax: (- expr)
 */

objectp F_sub(objectp args)
{
	objectp p,p1;
	mpf_t a,b;
	mpf_set_default_prec(PRECISION);
	mpf_init(a);
	mpf_init(b);
	p = args;
	mpf_set_str(a,"0",0);
	
	do {
		p1 = eval(car(p));
		if(p1->type == OBJ_NGMP ){
			mpf_set_str(b,p1->value.s,0);
			if (p == args && cdr(p) != nil)
				mpf_set(a,b);
			else
				mpf_sub(a,a,b);
		} else 
			return nil;
		
		
	    p=cdr(p);
	} while(p!=nil);
	
	p1=new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p1->value.s,PRECISION,"%.Ff",a);
	
	mpf_clear(a);
	mpf_clear(b);
	return p1;
	
}

/*
 * Syntax: (% expr)
 */

objectp F_mod(objectp args)
{
	objectp p1, p2,p;
	mpz_t a,b;
	mpz_init(a);
	mpz_init(b);
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	p = eval(car(args));
	
    if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
	    mpz_set_str(a,p1->value.s,0);
	    mpz_set_str(b,p2->value.s,0);

	} else {
		mpz_clear(a);
		mpz_clear(b);
		return nil;
	}
	
	mpz_mod(a,a,b);
	
	p=new_object(OBJ_NGMP);
	p->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p->value.s,PRECISION,"%Zd",a);

	mpz_clear(a);
	mpz_clear(b);
	return p;
}

/*
 * Syntax: (gcd a b)
 */

objectp F_gcd(objectp args)
{
	objectp p1, p2,p3;
	mpz_t a,b;
	mpz_init(a);
	mpz_init(b);
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	p3 = eval(car(args));	

   if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
	    mpz_set_str(a,p1->value.s,0);
	    mpz_set_str(b,p2->value.s,0);

	} else {
		mpz_clear(a);
		mpz_clear(b);
		return nil;
	}
   
	mpz_gcd(a,a,b);

	p3=new_object(OBJ_NGMP);
	p3->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p3->value.s,PRECISION,"%Zd",a);
	mpz_clear(a);
	mpz_clear(b);
	return p3;
}

/*
 * Syntax: (sqrt expr)
 */

objectp F_sqrt(objectp args)
{
	objectp p,p1,p2;
	mpf_t a;
	mpf_set_default_prec(128);

	mpf_init(a);
	p1=args;
	p = eval(car(args));

	if(p->type == OBJ_NGMP) {
		mpf_set_str(a,p->value.s,0);
	} else {
		mpf_clear(a);
		return nil;
	}
	
	p2=new_object(OBJ_NGMP);
	p2->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	if(mpf_cmp_d(a,0.0) == -1) {
	    mpf_clear(a);
	    warnx("`SQRT' warning: negative number");
	    return nil;
	}
	mpf_sqrt(a,a);

	gmp_snprintf(p2->value.s,PRECISION,"%.Ff",a);
	mpf_clear(a);
	return p2;
}

/*
 * Syntax: (expt expr) (^ expt)
 */

objectp F_pow(objectp args)
{
    mpf_t a;
    objectp p1,p2,p;
    unsigned long n;

    mpf_init(a);
    p1=eval(car(args));
    p2=eval(car(cdr(args)));
    p=eval(car(args));

    if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
	    mpf_set_str(a,p1->value.s,0);
	    n=strtol(p2->value.s,(char **) NULL,0);
	} else {
		mpf_clear(a);
	    return nil;
	}
	
	mpf_pow_ui(a,a,n);

	p=new_object(OBJ_NGMP);
	p->value.s = (char *) xcalloc(PRECISION,sizeof(char));	
	gmp_snprintf(p->value.s,PRECISION,"%.Ff",a);
	mpf_clear(a);
	return p;
}

/*
 * Syntax: (evenp expr)
 */

objectp F_even(objectp args)
{
	objectp p;
	mpz_t (a);
	mpz_init(a);
	p=eval(car(args));

	if( p->type == OBJ_NGMP) {
		mpz_set_str(a,p->value.s,0);
	    if(mpz_even_p(a) == 0) {
			mpz_clear(a);
			return t;
	    } else {
			mpz_clear(a);
			return nil;
	    }
	}
	mpz_clear(a);
	return nil;
}

/*
 * Syntax: (oddp expr)
 */

objectp F_odd(objectp args)
{
	objectp p;
	mpz_t (a);
	mpz_init(a);
	p=eval(car(args));

	if( p->type == OBJ_NGMP) {
		mpz_set_str(a,p->value.s,0);
	    if(mpz_odd_p(a) == 0) {
			mpz_clear(a);
			return t;
	    } else {
			mpz_clear(a);
			return nil;
	    }
	}
	mpz_clear(a);
    return nil;
}


/*
 * Syntax: (< expr_0 expr_1 ... expr_n)
 */

objectp F_lt(objectp args)
{
	objectp p1, p2;
	mpf_t a,b;

	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	mpf_init(a);
	mpf_init(b);
	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) < 0)
		return t;
	    return nil;
	}
	if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
		mpf_set_str(a,p1->value.s,0);
		mpf_set_str(b,p2->value.s,0);
	} else {
	    mpf_clear(a);
		mpf_clear(b);
		return nil;
	}
	
	
	if(mpf_cmp(a,b) == -1) {
	    mpf_clear(a);
	    mpf_clear(b);
	    return t;
	} else {
	    mpf_clear(a);
	    mpf_clear(b);
	    return nil;
	}
}


/*
 * Syntax: (> expr_0 expr_1 ... expr_n)
 */

objectp F_gt(objectp args)
{
	objectp p1, p2;
	mpf_t a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	mpf_init(a);
	mpf_init(b);

	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) > 0)
		return t;
	    return nil;
	}

	if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
		mpf_set_str(a,p1->value.s,0);
		mpf_set_str(b,p2->value.s,0);
	} else {
		mpf_clear(a);
		mpf_clear(b);
		return nil;
	}
	
	if(mpf_cmp(a,b) == 1) {
	    mpf_clear(a);
	    mpf_clear(b);
	    return t;
	} else {
	    mpf_clear(a);
	    mpf_clear(b);
	    return nil;
	}
}
/*
 * Syntax: (<= expr_0 expr_1 ... expr_n)
 */

objectp F_le(objectp args)
{
	objectp p1, p2;
	mpf_t a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	mpf_init(a);
	mpf_init(b);

	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) <= 0)
		return t;
	    return nil;
	}

	if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
		mpf_set_str(a,p1->value.s,0);
		mpf_set_str(b,p2->value.s,0);
	} else {
		mpf_clear(a);
		mpf_clear(b);
		return nil;
	}
	
	if(mpf_cmp(a,b) == -1 || mpf_cmp(a,b) == 0) {
	    mpf_clear(a);
	    mpf_clear(b);
	    return t;
	} else {
	    mpf_clear(a);
	    mpf_clear(b);
	    return nil;
	}
}
/*
 * Syntax: (>= expr_0 expr_1 ... expr_n)
 */

objectp F_ge(objectp args)
{
	objectp p1, p2;
	mpf_t a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	mpf_init(a);
	mpf_init(b);

	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) >= 0)
		return t;
	    return nil;
	}

	if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
		mpf_set_str(a,p1->value.s,0);
		mpf_set_str(b,p2->value.s,0);
	} else {
		mpf_clear(a);
		mpf_clear(b);
		return nil;
	}

	
	if(mpf_cmp(a,b) == -1 || mpf_cmp(a,b) == 0) {
	    mpf_clear(a);
	    mpf_clear(b);
	    return t;
	}
 	else {
	    mpf_clear(a);
	    mpf_clear(b);
	    return nil;
	}
}

objectp F_eq(objectp args)
{
	objectp p1, p2;
	mpf_t a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	mpf_init(a);
	mpf_init(b);

	if(p1->type == OBJ_NGMP && p2->type == OBJ_NGMP) {
		mpf_set_str(a,p1->value.s,0);
		mpf_set_str(b,p2->value.s,0);
	} else {
		mpf_clear(a);
		mpf_clear(b);
		return nil;
	}
	    
	
	if(mpf_cmp(a,b) == 0) {
	    mpf_clear(a);
	    mpf_clear(b);
	    return t;
	} else {
	    mpf_clear(a);
	    mpf_clear(b);
	    return nil;
	}
}
objectp F_abs(objectp args)
{
    objectp p,p1;
    p=eval(car(args));
    mpf_t a;
    mpf_init(a);
    mpf_set_str(a,p->value.s,0);
    mpf_abs(a,a);
    
    p1=new_object(OBJ_NGMP);
    p1->value.s=(char *)xcalloc(PRECISION,sizeof(char));
    gmp_snprintf(p1->value.s,PRECISION,"%.Ff",a);
    return p1;
}
#else 

/* IF NO GMP */

objectp F_abs(objectp args)
{
    objectp p,p1;
    p=eval(car(args));
    switch(p->type) {
	case OBJ_INTEGER:
	    p1 = new_object(OBJ_INTEGER);
	    p1->value.i = abs(p->value.i);
	    break;
	case OBJ_DOUBLE:
	    p1 = new_object(OBJ_DOUBLE);
	    p1->value.d = fabs(p->value.d);
	    break;
	default:
	    warnx("`ABS' warning: non numeric argument");
	    return nil;
    }
    return p1;
}
objectp F_powm(objectp args)
{
    objectp p,p1,p2,p3;
    int a,b,c;
    p1=eval(car(args));
    p2=eval(car(cdr(args)));
    p3=eval(car(cdr(cdr(args))));
    
    if(p1->type == OBJ_INTEGER && p2->type == OBJ_INTEGER && p3->type == OBJ_INTEGER) {
	    a = p1->value.i;
	    b = p2->value.i;
	    c = p3->value.i;
    } else 
		return nil;
	

	p = new_object(OBJ_INTEGER);
	p->value.i = modexp(a,b,c);
	return p;

}       

objectp F_primep(objectp args)
{
    objectp p;
    p=eval(car(args));

    if(p->type == OBJ_INTEGER) {
		if(isprime(p->value.i) == 1) 
			return t;
		else
			return nil;
    } else if(opt_lint) {
		warnx("`PRIMEP' warning: non integer argument");
		return nil;
    } else
		return nil;
}


objectp F_zerop(objectp args)
{
    objectp p1 = car(args);

    switch(p1->type) {
		
	case OBJ_INTEGER:
	    if(p1->value.i == 0)
			return t;
		return nil;
	    break;

	case OBJ_DOUBLE:
	    if(p1->value.d == 0)
			return t;
	    return nil;
	    break;
	default:
		return nil;
		
    }
    return nil;
}

objectp F_floor(objectp args)
{
	objectp p,p1;
	p = eval(car(args));
	p1 = new_object(OBJ_INTEGER);

	if (p->type == OBJ_DOUBLE) {
		p1->value.i=floor(p->value.d);
		return p1;
	} else if( p->type == OBJ_INTEGER) {
		p1->value.i=p->value.i;
		return p1;
	} else if(opt_lint) {
		warnx("`LOG' warning: non numeric value");
		return nil;
	} else
		return nil;
	
}

objectp F_lt(objectp args)
{
	objectp p1, p2;
	double a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	
	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) < 0)
		return t;
	    return nil;
	}

	switch(p1->type) {
	case OBJ_INTEGER:
		a = p1->value.i;
		break;
	case OBJ_DOUBLE:
		a = p1->value.d;
		break;
	
	default:
		if(opt_lint) {
		    warnx("`<' warning: comparision between non integer operand");
		    return nil;
		} else
		    return nil;
	}	
	switch(p2->type) {
	case OBJ_INTEGER:
		b = p2->value.i;
		break;
	case OBJ_DOUBLE:
		b = p2->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`<' warning: comparision between non integer operand");
		    return nil;
		} else
			return nil;
	}	
	if(a < b)
		return t;
	else
		return nil;
	
}

/*
 * Syntax: (<= expr1 expr2)
 */

objectp F_le(objectp args)
{
	objectp p1, p2;
	double a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));

	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) <= 0)
		return t;
	    return nil;
	}

	switch(p1->type) {
	case OBJ_INTEGER:
		a = p1->value.i;
		break;
	case OBJ_DOUBLE:
		a = p1->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`<=' warning: comparision between non integer operand");
		    return nil;
		} else
		    return nil;
	}	

	switch(p2->type) {
	case OBJ_INTEGER:
		b = p2->value.i;
		break;
	case OBJ_DOUBLE:
		b = p2->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`<=' warning: comparision between non integer operand");
		    return nil;
		} else
			return nil;
	}	
	if(a <= b)
		return t;
	else
		return nil;
	
	
}
/*
 * Syntax: (> expr1 expr2)
 */

objectp F_gt(objectp args)
{
	objectp p1, p2;
	double a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	
	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) > 0)
		return t;
	    return nil;
	}

	switch(p1->type) {
	case OBJ_INTEGER:
		a = p1->value.i;
		break;
	case OBJ_DOUBLE:
		a = p1->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`>' warning: comparision between non integer operand");
		    return nil;
		} else
		    return nil;
	}	
	switch(p2->type) {
	case OBJ_INTEGER:
		b = p2->value.i;
		break;
	case OBJ_DOUBLE:
		b = p2->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`>' warning: comparision between non integer operand");
		    return nil;
		} else
			return nil;
	}	
	if(a > b)
		return t;
	else
		return nil;
	
	
}

/*
 * Syntax: (>= expr1 expr2)
 */

objectp F_ge(objectp args)
{
	objectp p1, p2;
	double a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));

	if(p1->type == OBJ_CSTRING && p2->type == OBJ_CSTRING) {
	    if(strcmp(p1->value.s,p2->value.s) >= 0)
		return t;
	    return nil;
	}

	switch(p1->type) {
	case OBJ_INTEGER:
		a = p1->value.i;
		break;
	case OBJ_DOUBLE:
		a = p1->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`>=' warning: comparision between non integer operand");
		    return nil;
		} else
		    return nil;
	}	
	switch(p2->type) {
	case OBJ_INTEGER:
		b = p2->value.i;
		break;
	case OBJ_DOUBLE:
		b = p2->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`>=' warning: comparision between non integer operand");
		    return nil;
		} else
			return nil;
	}	
	if(a >= b)
		return t;
	else
		return nil;
	
	
}


objectp F_gcd(objectp args)
{
	objectp p1,p2,p3;
	int a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));

	if (p1->type == OBJ_INTEGER && p2->type == OBJ_INTEGER) {
		a=p1->value.i;
		b=p2->value.i;
		p3=new_object(OBJ_INTEGER);
		p3->value.i=gcd(a,b);
		return p3;
	} else if(opt_lint) {
		warnx("`GCD' warning: non integers values");
		return nil;
	} else
		return nil;
}

objectp F_add(objectp args)
{
	objectp p = args, p1;
	double v = 0.0;

	do {
		p1 = eval(car(p));
		if (p1->type == OBJ_DOUBLE) {
			v += (double)p1->value.d;
		} else if(p1->type == OBJ_INTEGER) {
			v +=(double)p1->value.i;
		} else if (opt_lint) {
			warnx("`+' warning: non numeric operand");
			return nil;
		} else
			return nil;
		p = cdr(p);
	} while (p != nil);

	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = v;
	
	return p1;
}

objectp F_even(objectp args)
{
	objectp p;
	p = eval(car(args));

	if( p->type == OBJ_INTEGER)
		return (( (p->value.i & 1) == 1 ) ? nil : t);
	else if(opt_lint) {
		warnx("`EVENP' warning: non integer value");
	    return nil;
	} else
	    return nil;
}

objectp F_odd(objectp args)
{
	objectp p;
	p = eval(car(args));

	if( p->type == OBJ_INTEGER)
		return ( ( (p->value.i & 1) == 1 ) ? t : nil);
	else if(opt_lint) {
		warnx("`OODP' warning: non integer value");
		return nil;
	} else
		return nil;
}

objectp F_div(objectp args)
{
	objectp p = args, p1,p2;
	double v = 0.0;
	do {
		p1 = eval(car(p));

		if (p1->type == OBJ_INTEGER) {
			if (p == args && cdr(p) != nil)
				v = p1->value.i + 0.0;
			else {
				if (p1->value.i == 0) {
					if (opt_lint)
						warnx("`/' warning: division by zero");
					v = 0.0;
					break;
				} else
					v /= p1->value.i + 0.0;
			}
			
		} else if(p1->type == OBJ_DOUBLE) {
			if (p == args && cdr(p) != nil)
				v = p1->value.d;
			else {
				if (p1->value.d == 0) {
					if (opt_lint)
						warnx("`/' warning: division by zero");
					v = 0.0;
					break;
				} else
					v /= p1->value.d;
			}
			
		} else if (opt_lint) {
		  warnx("`/' warning: non integer operand");
		  return nil;
		} else
			return nil;
		
		p = cdr(p);
	} while (p != nil);
	
	p2 = new_object(OBJ_DOUBLE);
	p2->value.d = v;
	
	return p2;
}

/*
 * Syntax: (% expr1 expr2)
 */
objectp F_mod(objectp args)
{
	objectp p1, p2, p3;
	int a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	a = b = 1;

	if(p1->type == OBJ_INTEGER && p2->type == OBJ_INTEGER) {
		a = p1->value.i;
		b = p2->value.i;
	} else
		return nil;
	
	if(b == 0) {
	    warnx("`%' warning: module by zero");
	    return nil;
	}
	
	p3=new_object(OBJ_INTEGER);
	p3->value.i =  a % b;
	return p3;
}

objectp F_sqrt(objectp args)
{
	objectp p,p1;
	p = eval(car(args));
	p1 = new_object(OBJ_DOUBLE);

	if (p->type == OBJ_DOUBLE ) {
		if(p->value.d < 0) {
		    warnx("`SQRT' warning: negative number");
		    return nil;
		}
		p1->value.d=sqrt(p->value.d);
		return p1;
	} else if( p->type == OBJ_INTEGER) {
		if(p->value.i < 0) {
		    warnx("`SQRT' warning: negative number");
		    return nil;
		}
		p1->value.d=sqrt(p->value.i);
		return p1;
	} else if(opt_lint) {
		warnx("`SQRT' warning: non numeric value");
		return nil;
	} else
		return nil;
}

objectp F_pow(objectp args)
{
	objectp p,p1,p2;
	double a,b;	
	p = new_object(OBJ_DOUBLE);
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));

	switch(p1->type) {
	case OBJ_INTEGER:
		a = p1->value.i;
		break;
	case OBJ_DOUBLE:
		a = p1->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`POW' warning: non integer operand");
		    return nil;
		} else
		    return nil;
	}	
	
	switch(p2->type) {
	case OBJ_INTEGER:
		b = p2->value.i;
		break;
	case OBJ_DOUBLE:
		b = p2->value.d;
		break;
	default:
		if(opt_lint) {
		    warnx("`POW' warning: non integer operand");
		    return nil;
		} else
			return nil;
	}	
	p->value.d = pow(a,b);	
	return p;
}


objectp F_sub(objectp args)
{
	objectp p = args, p1,p2;
	register int v;
	double vv;
	v=vv=0;
	do  {
		p1 = eval(car(p));
		if (p1->type == OBJ_DOUBLE) {
			if (p == args && cdr(p) != nil)
				vv =  p1->value.d;
			else
				vv -=  p1->value.d;
			
		} else if (p1->type == OBJ_INTEGER) {
			if (p == args && cdr(p) != nil)
				vv = p1->value.i + 0.0;
			else
				vv -= p1->value.i + 0.0;
		} else
		    return nil;
		    
		p = cdr(p);
	} while(p != nil);
	
	if(vv!=0) {
		p2 = new_object(OBJ_DOUBLE);
		p2->value.d = vv + v;
	}
	else {
		p2 = new_object(OBJ_INTEGER);
		p2->value.i =  (int)v + vv ;
	} 
	return p2;
}

/*
 * Syntax: (* expr...)
 */
objectp F_mul(objectp args)
{
	objectp p = args, p1;
	double v=1;
	do {
		p1 = eval(car(p));
		if (p1->type == OBJ_INTEGER) {
			v *= p1->value.i;
		} else if (p1->type == OBJ_DOUBLE) {
			v *= p1->value.d;
		} else if (opt_lint) {
			warnx("`*' warning: non numeric operand");
			return nil;
		} else
			return nil;
		p = cdr(p);
	} while (p != nil);
	p1 = new_object(OBJ_DOUBLE);
	p1->value.d = v;
	return p1;
}


/*
 * Syntax: (eq expr1 expr2)
 */
objectp F_eq(objectp args)
{
	objectp p1, p2;
	double a,b;
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));

	switch(p1->type) {
	    case OBJ_INTEGER:
		a = (double) p1->value.i;
		break;
	    case OBJ_DOUBLE:
		a = p1->value.d;
		break;
	    default:
		return nil;
	}

	switch(p2->type) {
	    case OBJ_INTEGER:
		b = (double) p2->value.i;
		break;
	    case OBJ_DOUBLE:
		b = p2->value.d;
		break;
	    default:
		return nil;
	}

	if(a == b)
	    return t;
	
	return nil;
		
} 

#endif


objectp F_bin2dec(objectp args)
{
    objectp p, p1;
    p=eval(car(args));
	
    if(p->type == OBJ_CSTRING) {
		if(num_string(p->value.s) == 1) {
#ifdef HAVE_GMP
		    p1 = new_object(OBJ_NGMP);
		    p1->value.s = (char *) xcalloc(16,sizeof(char));
		    snprintf(p1->value.s,16,"%d", bin2dec(p->value.s));
#else
		    p1 = new_object(OBJ_INTEGER);
		    p1->value.i = bin2dec(p1->value.s);
#endif
		    return p1;

		} else
			return nil;
    } else if(opt_lint) {
		warnx("`BIN2DEC' warning: non string argument");
		return nil;
    } else
		return nil;
}

objectp F_dec2bin(objectp args)
{
    objectp p;
    long n;
    p=eval(car(args));
    
    if(p->type == OBJ_INTEGER) {
		dec2bin(p->value.i);
		return t;
    } else if(p->type == OBJ_CSTRING) {
		n = strtol(p->value.s, (char **) NULL, 0);
		dec2bin( (int) n);
		return t;
    } else
		return nil;
}


objectp F_atoi(objectp args)
{
    objectp p,p1;
    p=eval(car(args));

    if(p->type == OBJ_CSTRING) {
		if(num_string(p->value.s) == 1) {
#ifdef HAVE_GMP
		    p1 = new_object(OBJ_NGMP);
		    p1->value.s = (char *) xcalloc(16,sizeof(char));
		    snprintf(p1->value.s,16,"%d", atoi(p->value.s));
#else
		    p1 = new_object(OBJ_INTEGER);
		    p1->value.i = atoi(p1->value.s);
#endif
		    return p1;
			
		} else
			return nil;
    } else if(opt_lint) {
		warnx("`ATOI' warning: non bignum argument");
		return nil;
    } else
		return nil;
}

objectp F_pi(objectp args)
{
	objectp p;
#ifdef HAVE_GMP
	p = new_object(OBJ_NGMP);
	p->value.s = (char *) xcalloc(32,sizeof(char));
	snprintf(p->value.s,16,"%g",3.141592653589793);
#else
	p = new_object(OBJ_DOUBLE);
	p->value.d = 3.141592;
#endif
	return p;
}

objectp F_phi(objectp args) 
{
    objectp p,p1;
#ifdef HAVE_GMP
    long n;
#endif
    int j;
    p=eval(car(args));
#ifdef HAVE_GMP
    if (p->type == OBJ_NGMP) {
		n = strtol(p->value.s, (char **) NULL, 0);
		j=phi((int) n);
		p1 = new_object(OBJ_NGMP);
		p1->value.s = (char *) xcalloc(16,sizeof(char));
		snprintf(p1->value.s,16,"%d", j);
		return p1;
#else
    if(p->type == OBJ_INTEGER) {
		j = phi(p->value.i);
		p1=new_object(OBJ_INTEGER);
		p1->value.i = j;
		return p1;
#endif
    } else if(opt_lint) {
		warnx("`PHI' warning: non numeric operand");
		return nil;
    } 

	return nil;
}


objectp F_inverse(objectp args) 
{
    objectp p,p1,p2;
    int n,m,i;
    p=eval(car(args));
    p1=eval(car(cdr(args)));
    n=m=i=0;

#ifdef HAVE_GMP
    if(p->type == OBJ_NGMP)
	n =(int) strtol(p->value.s, (char **) NULL, 0);
#else
    if(p->type == OBJ_INTEGER)
	n=p->value.i;
#endif
     else if(opt_lint) {
		warnx("`INVERSE' warning: non integer operand");
		return nil;
    } else
		return nil;

#ifdef HAVE_GMP
    if(p1->type == OBJ_NGMP)
	m =(int) strtol(p1->value.s, (char **) NULL, 0);
#else
    if(p1->type == OBJ_INTEGER)
	m=p1->value.i;
#endif
    else if(opt_lint) {
		warnx("`INVERSE' warning: non integer operand");
		return nil;
    } else
		return nil;
    
    i = inverse(n,m);	

#ifdef HAVE_GMP
    p2 = new_object(OBJ_NGMP);
    p2->value.s = (char *) xcalloc(16,sizeof(char));
    snprintf(p2->value.s,16,"%d",i);
#else
    p2=new_object(OBJ_INTEGER);
    p2->value.i = i;
#endif
    return p2;
}    

objectp F_ord(objectp args)
{
    objectp p, p1, p2;
    int a,m,d,i,x;
    p = eval(car(args));
    p1= eval(car(cdr(args)));
    a=m=d=i=x=0;

#ifdef HAVE_GMP
    if(p->type == OBJ_NGMP)
	a =(int) strtol(p->value.s, (char **) NULL, 0);
#else
    if(p->type == OBJ_INTEGER)
	a = p->value.i;
#endif
     else
	return nil;

#ifdef HAVE_GMP
    if(p1->type == OBJ_NGMP)
		m =(int) strtol(p1->value.s, (char **) NULL, 0);
#else
    if(p1->type == OBJ_INTEGER) 
		m = p1->value.i;
#endif
     else
		return nil;
	
	
    d=gcd(a,m);


    if(d>1) {
#ifdef HAVE_GMP
		p2 = new_object(OBJ_NGMP);
		p2->value.s = (char *) xcalloc(16,sizeof(char));
		snprintf(p2->value.s,16,"%d",0);
		return p2;
#else
		p2=new_object(OBJ_INTEGER);
		p2->value.i = 0;
		return p2;
#endif
    }
    x = 1;

    for(i=1;i<=m;i++) {
		if ( (x = (x * a) % m) == 1)  {
#ifdef HAVE_GMP
			p2 = new_object(OBJ_NGMP);
			p2->value.s = (char *) xcalloc(16,sizeof(char));
			snprintf(p2->value.s,16,"%d",i);
			return p2;
#else
			p2=new_object(OBJ_INTEGER);
			p2->value.i = i;
			return p2;
#endif
		}
    }
#ifdef HAVE_GMP
	p2 = new_object(OBJ_NGMP);
	p2->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p2->value.s,16,"%d",0);
	return p2;
#else

	p2=new_object(OBJ_INTEGER);
	p2->value.i = 0;
	return p2;
#endif
    
}

objectp F_variance(objectp args) 
{
    objectp p,p1;
    double sum,n,x,ssum,var;
    sum = n = x = ssum = var = 0.0;
    p=eval(car(args));

    while(p != nil) {
		p1=eval(car(p));
#ifdef HAVE_GMP
		if(p1->type == OBJ_NGMP)
			x = strtod(p1->value.s, (char **) NULL);
#else
		if(p1->type == OBJ_INTEGER)
			x = (double) p1->value.i;
		else if(p1->type == OBJ_DOUBLE)
			x = p1->value.d;
#endif
		else if(opt_lint) {
			warnx("`VARIANCE' warning: non numeric operand ");
			return nil;
		} else
			return nil;
	    
		sum += x;
		ssum += x * x;
		n++;
		p=cdr(p);
    }
	
    var = ssum / n - (sum/n) * (sum / n);

#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",var);
	return p1;

#else
	p1=new_object(OBJ_DOUBLE);
	p1->value.d = var;
	return p1;

#endif

	return nil;
}

objectp F_qres(objectp args)
{
    int i,q,m;
    objectp p,p1;
    objectp lis,first= NULL,prev = NULL;
    lis=new_object(OBJ_CONS);
    p=eval(car(args));

    q = i = m = 0;
#ifdef HAVE_GMP
    if(p->type == OBJ_NGMP)
	    q =(int) strtol(p->value.s, (char **) NULL, 0);
#else
    if(p->type == OBJ_INTEGER)
		q = p->value.i;
#endif
    else if(opt_lint) {
		warnx("`QRES' warning: non integer operand");
		return nil;
    } else
		return nil;

    m = floor(q/2) + 1;
    i = 0;

	do {
		p1=new_object(OBJ_INTEGER);
		p1->value.i = i;
		i++;
		p1->value.i = p1->value.i * p1->value.i % q;
		lis = new_object(OBJ_CONS); 
		lis->value.c.car = eval(p1);
		if (first == NULL)
    		    first = lis;
		if (prev != NULL)
			prev->value.c.cdr = lis;
		prev = lis;
    } while (i < m);
	
    return first;
}
/* Taken from Handbook Of applied cryptography */


objectp F_jacobi(objectp args)
{
    objectp p,p1,p2;
    int n,m,j;
    p=eval(car(args));
    p1=eval(car(cdr(args)));
    n=m=0;
#ifdef HAVE_GMP
    if(p->type == OBJ_NGMP)
		n =(int) strtol(p->value.s, (char **) NULL, 0);
#else
    if(p->type == OBJ_INTEGER) 
		n=p->value.i;
#endif
    else if(opt_lint) {
		warnx("`JACOBI' warning: non integer operand");
		return nil;
    } else
		return nil;

#ifdef HAVE_GMP
    if(p1->type == OBJ_NGMP)
	    m =(int) strtol(p1->value.s, (char **) NULL, 0);
#else
    if(p1->type == OBJ_INTEGER) 
		m=p1->value.i;
#endif
    else if(opt_lint) {
		warnx("`JACOBI' warning: non integer operand");
		return nil;
    } else
		return nil;
    j = JACOBI(n,m);
#ifdef HAVE_GMP
	p2 = new_object(OBJ_NGMP);
	p2->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p2->value.s,16,"%d",j);
	return p2;
#else
	p2=new_object(OBJ_INTEGER);
	p2->value.i = j;
	return p2;
#endif
	
}

objectp F_res(objectp args)
{
    int i,r;
    objectp p,p4;
    objectp lis,first= NULL,prev = NULL;
    lis=new_object(OBJ_CONS);
    p=eval(car(args));
    r=0;
#ifdef HAVE_GMP
    if(p->type == OBJ_NGMP) 
		r =(int) strtol(p->value.s, (char **) NULL, 0);
#else
    if(p->type == OBJ_INTEGER) 
		r = p->value.i;
#endif
    else if(opt_lint) {
		warnx("`RESMOD' warning: non integer operand");
		return nil;
    } else
		return nil;
    i=1;

	do {
		p4=new_object(OBJ_INTEGER);
		p4->value.i = i;
		i++;
	    	if(gcd(r,p4->value.i) == 1) {
    			lis = new_object(OBJ_CONS); 
			lis->value.c.car = eval(p4);
			if (first == NULL)
				first = lis;
			if (prev != NULL)
				prev->value.c.cdr = lis;
			prev = lis;
		}
	} while (i < r);
	
    return first;
}

objectp F_divisors(objectp args)
{
    int i,n,h;
    objectp p,p4;
    objectp lis,first= NULL,prev = NULL;
    lis=new_object(OBJ_CONS);
    p=eval(car(args));
    n=h=0;
#ifdef HAVE_GMP
    if(p->type == OBJ_NGMP) 
	h =(int) strtol(p->value.s, (char **) NULL, 0);
#else
    if(p->type == OBJ_INTEGER) 
		h = p->value.i;
#endif
     else if(opt_lint) {
		warnx("`DIVOF' warning: non integer operand");
		return nil;
    } else
		return nil;
	
    n = (int)(h / 2);
    i=1;
	do {
		p4=new_object(OBJ_INTEGER);
		p4->value.i = i;
		i++;
		if(h % p4->value.i == 0) {
			lis = new_object(OBJ_CONS); 
			lis->value.c.car = eval(p4);
			if (first == NULL)
				first = lis;
			if (prev != NULL)
				prev->value.c.cdr = lis;
			prev = lis;
		}
	} while (i <= n);
	
    return first;
}


objectp F_log10(objectp args)
{
	objectp p,p1;
#ifdef HAVE_GMP
	long l;
#endif
	double d;
	p = eval(car(args));
#ifdef HAVE_GMP
	 if( p->type == OBJ_NGMP) {
		l = strtol(p->value.s, (char **) NULL, 0);
		d = (double) log10((double) l);
	 }
#else
	if (p->type == OBJ_DOUBLE) 
		d=log10(p->value.d);
	 else if( p->type == OBJ_INTEGER) 
		d=log10(p->value.i);
#endif
	 else if(opt_lint) {
		warnx("`LOG10' warning: non numeric value");
		return nil;
	} else
		return nil;
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	snprintf(p1->value.s,16,"%g",d);
	return p1;
#else
	p1=new_object(OBJ_DOUBLE);
	p1->value.d = d;
	return p1;
#endif

}


objectp F_max(objectp args)
{
	objectp p, p1;
#ifdef HAVE_GMP
	mpf_t max,n;
	mpf_init(max);
	mpf_init(n);
#else
	double max;
	double n;
	max = n = 0;
#endif
	p=eval(car(args));

	if(p->type == OBJ_CONS) {
	    p1=eval(car(p));
	} else {
	    p=args;
	    p1 = eval(car(args));
	}

#ifdef HAVE_GMP
	 if (p1->type == OBJ_NGMP) 
	    mpf_set_str(max,p1->value.s,0);
#else
	 if (p1->type == OBJ_INTEGER) 
	    max = p1->value.i;
	 else if (p1->type == OBJ_DOUBLE) 
	    max = p1->value.d;
#endif
	 else if (opt_lint) {
	    warnx("`MAX' warning: non numeric value");
	    return nil;
	} else
	    return nil;
	    
	p = cdr(p);

	while(p != nil) {
		p1 = eval(car(p));
#ifndef HAVE_GMP
		if (p1->type == OBJ_INTEGER) {
			if(p1->value.i > max)
				max = p1->value.i;
		} else if (p1->type == OBJ_DOUBLE) {
			if(p1->value.d > max)
				max = p1->value.d;
#else
		if (p1->type == OBJ_NGMP) {
			mpf_set_str(n,p1->value.s,0);
			if(mpf_cmp(n,max) == 1)
		    	    mpf_set(max,n);
#endif
		} else if (opt_lint) {
		    warnx("`MAX' warning: non numeric value");
		    return nil;
		} else
		    return nil;

		p = cdr(p);
	}
	 

#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(PRECISION,sizeof(char));
	gmp_snprintf(p1->value.s,PRECISION,"%.Ff",max);
	return p1;
#else
	p1=new_object(OBJ_DOUBLE);
	p1->value.d = max;
	return p1;
#endif
  
}

objectp F_min(objectp args)
{
	objectp p, p1;
#ifdef HAVE_GMP
	mpf_t min,n;
	mpf_init(min);
	mpf_init(n);
#else
	double min;
	double n;
	min = n = 0;
#endif
	p=eval(car(args));

	if(p->type == OBJ_CONS) {
	    p1=eval(car(p));
	} else {
	    p=args;
	    p1 = eval(car(args));
	}

#ifdef HAVE_GMP
	 if (p1->type == OBJ_NGMP) 
	    mpf_set_str(min,p1->value.s,0);
#else
	 if (p1->type == OBJ_INTEGER) 
	    min = p1->value.i;
	 else if (p1->type == OBJ_DOUBLE) 
	    min = p1->value.d;
#endif
	 else if (opt_lint) {
	    warnx("`MIN' warning: non numeric value");
	    return nil;
	} else
	    return nil;
	    
	p = cdr(p);

	while(p != nil) {
		p1 = eval(car(p));
#ifndef HAVE_GMP
		if (p1->type == OBJ_INTEGER) {
			if(p1->value.i < min)
				min = p1->value.i;
		} else if (p1->type == OBJ_DOUBLE) {
			if(p1->value.d < min)
				min = p1->value.d;
#else
		if (p1->type == OBJ_NGMP) {
			mpf_set_str(n,p1->value.s,0);
			if(mpf_cmp(n,min) == -1)
		    	    mpf_set(min,n);
#endif
		} else if (opt_lint) {
		    warnx("`MIN' warning: non numeric value");
		    return nil;
		} else
		    return nil;

		p = cdr(p);
	}
	 

#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(PRECISION,sizeof(char));
	gmp_snprintf(p1->value.s,PRECISION,"%.Ff",min);
	return p1;
#else
	p1=new_object(OBJ_DOUBLE);
	p1->value.d = min;
	return p1;
#endif
  
}


void princ_string(FILE *fout, char *s)
{
	char *p;
	
	fputc('"', fout);
	for (p = s; *p != '\0'; ++p)
	    switch (*p) {
		case '\a':
			fputc('\\', fout);
			fputc('a', fout);
			break;
		case '\b':
			fputc('\\', fout);
			fputc('b', fout);
			break;
		case '\f':
			fputc('\\', fout);
			fputc('f', fout);
			break;
		case '\n':
			fputc('\\', fout);
			fputc('n', fout);
			break;
		case '\r':
			fputc('\\', fout);
			fputc('r', fout);
			break;
		case '\t':
			fputc('\\', fout);
			fputc('t', fout);
			break;
		case '\v':
			fputc('\\', fout);
			fputc('v', fout);
			break;
		default:
			fputc(*p, fout);
	    }

	fputc('"', fout);
}

void princ_object(FILE *fout, objectp p)
{
	objectp p1;
	switch (p->type) {

	case OBJ_NIL:
		fprintf(fout, "nil");
		break;

	case OBJ_T:
		fputc('t', fout);
		break;

	case OBJ_IDENTIFIER:
		fprintf(fout, "%s", p->value.id);
		break;

	case OBJ_NGMP:
		fprintf(fout,"%s", p->value.s);
		break;
	case OBJ_CSTRING:
		princ_string(fout, p->value.s);
		break;
	case OBJ_INTEGER:
		fprintf(fout, "%d", p->value.i);
		break;
	case OBJ_DOUBLE:
		fprintf(fout, "%g",p->value.d);
		break;

	case OBJ_CONS:
		fputc('(', fout);
		p1 = p;
		do {
			princ_object(fout, p1->value.c.car);
			p1 = p1->value.c.cdr;
			if (p1 != nil) {
				fputc(' ', fout);
				if (p1->type != OBJ_CONS) {
					fprintf(fout, ". ");
					princ_object(fout, p1);
				}
			}
		} while (p1 != nil && p1->type == OBJ_CONS);
		fputc(')', fout);
	}
}

/*
 * Syntax: (princ expr...)
 */

objectp F_princ(objectp args)
{
	objectp p = args, p1;
	
	do {
		p1 = eval(car(p));
		if(p1->type == OBJ_CSTRING)
		    printf("%s",p1->value.s);
		else
		    princ_object(stdout, p1);
		p = cdr(p);
	} while (p != nil);
	
	return t;
}

objectp F_prinf(objectp args)
{
	objectp p,p1,p2;
	FILE *out;	

	p1=eval(car(args));
	if(p1 == nil || p1->type != OBJ_CSTRING)
	    return nil;

	p2=eval(car(cdr(args)));
	if(p2 == nil || p2->type != OBJ_CSTRING)
	    return nil;

	if( (out=fopen(p1->value.s,p2->value.s)) == NULL) {
		perror("warning");
		return nil;
	}

	p=cdr(cdr(args));
	do {
		p1 = eval(car(p));
		if(p1->type == OBJ_CSTRING)
		    fprintf(out,"%s", p1->value.s);
		else 
		    princ_object(out, p1);
		p = cdr(p);
	} while (p != nil);
	
	fclose(out);
	return t;

}

/*
 * Syntax: (atom expr)
 */
 objectp F_atom(objectp args)
{
	objectp p;
	
	p = eval(car(args));
	
	switch (p->type) {
	case OBJ_T:
	case OBJ_NIL:
	case OBJ_INTEGER:
	case OBJ_DOUBLE:
#ifdef HAVE_GMP
	case OBJ_NGMP:
#endif
	case OBJ_CSTRING:
	case OBJ_IDENTIFIER:
		return t;
	}
  
	return nil;
}
	
/*
 * Syntax: (isnum value)
 */
		
objectp F_isnum(objectp args)
{
	objectp p;
	
	p = eval(car(args));
	
	if((p->type) == OBJ_INTEGER) {
		return t;
	} else if((p->type) == OBJ_CSTRING) {
		if(num_string(p->value.s) == 1)
			return t;
#ifdef HAVE_GMP
	} else if((p->type) == OBJ_NGMP) {
		    return t;
#endif
	} else if(p->type == OBJ_DOUBLE) {
		return t;
	}
	return nil;
}

/*
 * Syntax: (islong value)
 */

objectp F_islong(objectp args)
{
	objectp p;
	
	p = eval(car(args));
  
	if((p->type) == OBJ_DOUBLE)
		return t;
#ifdef HAVE_GMP
	 else if ( (p->type == OBJ_NGMP))  
		return t;
#endif	
	return nil;
}

/*
 * Syntax: (cons expr1 expr2)
 */
objectp F_cons(objectp args)
{
	objectp p;
	
	p = new_object(OBJ_CONS);
	p->value.c.car = eval(car(args));
	p->value.c.cdr = eval(car(cdr(args)));
	
	return p;
}

/*
 * Syntax: (list expr1...)
 */

objectp F_list(objectp args)
{
	objectp p = args, first = NULL, prev = NULL, p1;
	
	if (p == nil)
		return nil;
	
	do {
		p1 = new_object(OBJ_CONS);
		p1->value.c.car = eval(car(p));
		if (first == NULL)
			first = p1;
		if (prev != NULL)
			prev->value.c.cdr = p1;
		prev = p1;
		p = cdr(p);
	} while (p != nil);
	
	return first;
}


/*
 * Syntax: (sort list ('< | '>) )
 * `name' is not evalled
 * `arglist' is not evalled
 */


objectp F_cmpn(objectp args) 
{
    objectp p,p1,p2,p3,p4;
    objectp lis,first= NULL,prev = NULL;
	
    int v = 0;
    double *array;
    double *base;
    double l;
    int (*compare) (const void *, const void *);

    p3 = p = eval(car(args));
    p1 = eval(car(cdr(args)));
    p2 = new_object(OBJ_DOUBLE);

    if(p->type != OBJ_CONS) {
	warnx("`SORT' warning: argument must be a list");
	return nil;
    }	

    if(p1 == nil) {
		warnx("`SORT' warning: must supply a sorting rule");
		return nil;
    }

    while(p!=nil) {
		p = cdr(p);
		++v;
    }
    array=(double *) xcalloc(v,sizeof(double));
    base=array;
    
    while(p3 != nil) {
		p2 = car(p3);
		p3 = cdr(p3);
		if(p2->type == OBJ_CSTRING) {
			l = strtod(p2->value.s, (char **) NULL);
			*array = l;
			
		} else if(p2->type == OBJ_DOUBLE) {
			*array=p2->value.d;
		} else if(p2->type == OBJ_INTEGER) {
			*array=(double) p2->value.i;
		} else {
			warnx("`SORT' warning: values must be numbers");
			return nil;
		}
		
		
		array++;
    }
    if (strcmp((char *)p1->value.s,"<") == 0) 
	    compare=up;
    else if (strcmp((char *)p1->value.s,">") == 0) 
	    compare = down;
    else {
	    warnx("`SORT' warning: unknow sort rule");
	    return nil;
    }
    
    if(heapsort(base,v,sizeof(double),compare) == 0)
    {
	    
		array=base;
		do {
			p4=new_object(OBJ_DOUBLE);
			p4->value.d = *array;
			array++;
			lis = new_object(OBJ_CONS);
			lis->value.c.car = eval(p4);
			if (first == NULL)
				first = lis;
			if (prev != NULL)
				prev->value.c.cdr = lis;
			prev = lis;
			v--;
		} while (v > 0);
		
		return first;
		
    }
    warnx("`SORT' warning: Problem sorting list");
    return nil;
}

objectp F_cmpstr(objectp args) 
{
    objectp p,p1,p2,p3,p4;
    objectp lis,first= NULL,prev = NULL;
	
    int v = 0;
    char **array;
    char **base;
    p3 = p = eval(car(args));
    p1 = eval(car(cdr(args)));
    p2 = new_object(OBJ_NGMP);

    if(p->type != OBJ_CONS || cdr(p) == nil) {
	warnx("`SORT' warning: argument must be a non null list");
	return nil;
    }	

    if(p1 == nil) {
		warnx("`SORT' warning: must supply a sorting rule");
		return nil;
    }

    while(p!=nil) {
		p = cdr(p);
		++v;
    }
    array=(char **) xcalloc(v,sizeof(char));
    base=(char **) xcalloc(v,sizeof(char));
    base = array;
    p2->value.s = (char *) xcalloc(512,sizeof(char));

    while(p3 != nil) {
		p2 = car(p3);
		p3 = cdr(p3);
		if(p2->type == OBJ_CSTRING) {
			*array = p2->value.s;
		} else {
			warnx("`SORT' warning: values must be strings");
			return nil;
		}
		array++;
    }
    if (strcmp((char *)p1->value.s,"<") == 0) 
	    sort_strings(base,v,0);		    
    else if (strcmp((char *)p1->value.s,">") == 0) 
	    sort_strings(base,v,1);		    
    else {
	    warnx("`SORT' warning: unknow sort rule");
	    return nil;
    }
    

		do {
			p4=new_object(OBJ_CSTRING);
			p4->value.s = (char *) xcalloc(512,sizeof(char));
			sprintf(p4->value.s,"%s",*base);
			base++;
			lis = new_object(OBJ_CONS);
			lis->value.c.car = eval(p4);
			if (first == NULL)
				first = lis;
			if (prev != NULL)
				prev->value.c.cdr = lis;
			prev = lis;
			v--;
		} while (v > 0);
		
		return first;
		
}

objectp F_compar(objectp args)
{
    objectp p1;
    p1=car(car(cdr(car(args))));
    if(p1->type == OBJ_CSTRING)
	return F_cmpstr(args);

#ifdef HAVE_GMP
    if(p1->type == OBJ_NGMP)
	    return F_cmpn(args);
#endif
    else if(p1->type == OBJ_INTEGER || p1->type == OBJ_DOUBLE)
	return F_cmpn(args);
    else  {
	warnx("Don`t know how to sort this type of elements");
	return nil;
    }
    return nil;
}

/*
 * Syntax: (quote expr)
 */

objectp F_quote(objectp args)
{
	return car(args);
}


objectp bquote_cons(objectp args)
{
	objectp p = args, first = NULL, prev = NULL, p1,p2,p3;

	if (p == nil)
		return nil;

	do {
		p1 = new_object(OBJ_CONS);
		p1->value.c.car = car(p);
		p2=p1->value.c.car;
	
		if(p2->type == OBJ_CONS) {
		    p3 = car(p2);
		    if(p3->type == OBJ_IDENTIFIER) {
			if(strcmp(p3->value.id,"comma") == 0 || strcmp(p3->value.id,"eval") == 0) 
			    p1->value.c.car = eval(car(p));
			else
			    p1->value.c.car = bquote_cons(p2);
		    } 
		}
		if (first == NULL)
			first = p1;
		if (prev != NULL)
			prev->value.c.cdr = p1;
		prev = p1;
		p = cdr(p);
	} while (p != nil);

	return first;
}
/*
 * Syntax: (backqoute expr ) 
 *     or: `(expr)
 */

objectp F_bquote(objectp args)
{
	objectp p = args,p1,p2,first = NULL, prev = NULL,t;

	int is_simple = 0;

	if(p == nil)
	    return nil;

	t = car(p);
	
	/* if the argument to backqute is not a list i.e.: `(+ 1 ,x) or `(+ 1 x) 
	   then we need to create a list with this,  i.e.: `((+ 1 ,x))
	   so we could call bquote_cons, and we set is_simple to return the car to get
	    `(+ 1 ,x) ==> `(+ 1 value_of_x)
	*/
	
	if(t->type != OBJ_CONS) {
	    t = new_object(OBJ_CONS);
	    t->value.c.car = p;
	    p = t;
	    is_simple = 1;
	}

	do {
		p2 = new_object(OBJ_CONS);
		p1 = car(p);
		if(p1->type == OBJ_CONS) { 
		    p2->value.c.car = bquote_cons(p1);
		} 
		if(first == NULL)
			first = p2;
		if(prev != NULL)
			prev->value.c.cdr = p2;
		prev = p2;
		p = cdr(p);
	} while (p != nil);
	
	if(is_simple == 0) 
	    return first;
	else
	    return car(first);
	    
}
/*
 * Syntax: (comma expr)
 */

objectp F_comma(objectp args)
{
	return (F_eval(args) == nil) ? eval(car(args)) : F_eval(args);
}



/*
 * Syntax: (and expr...)
 */
objectp F_and(objectp args)
{
	objectp p = args, p1;
	
	do {
		p1 = eval(car(p));
		if (p1 == nil)
			return nil;
		p = cdr(p);
	} while (p != nil);
	
	return p1;
}

/*
 * Syntax: (or expr...)
 */
objectp F_or(objectp args)
{
	objectp p = args, p1;
	
	do {
		p1 = eval(car(p));
		if (p1 != nil)
			return p1;
		p = cdr(p);
	} while (p != nil);
	
	return nil;
}

/*
 * Syntax: (not expr)
 * Syntax: (null expr)
 */
objectp F_not(objectp args)
{
	objectp p = eval(car(args));
	
	if (p != nil)
		return nil;
	
	return t;
}

/*
 * Syntax: (cond (expr1 [expr2])...)
 */
objectp F_cond(objectp args)
{
	objectp p = args, p1, p2, p3;
	
	do {
		p1 = car(p);
		if ((p2 = eval(car(p1))) != nil) {
			if ((p3 = cdr(p1)) != nil)
				return F_progn(p3);
			return p2;
		}
		p = cdr(p);
	} while (p != nil);
	
	return nil;
}

		
/*
 * Syntax: (if expr then-expr else-expr...)
 */
objectp F_if(objectp args)
{
	objectp p1, p2, p3;
	
	p1 = car(args);
	p2 = car(cdr(args));
	p3 = cdr(cdr(args));
	
	if (eval(p1) != nil)
		return eval(p2);
	
	return F_progn(p3);
}

		
/*
 * Syntax: (when expr then-expr...)
 */
objectp F_when(objectp args)
{
	objectp p1, p2;
	
	p1 = car(args);
	p2 = cdr(args);
	if (eval(p1) != nil)
		return F_progn(p2);
	
	return nil;
}

/*
 * Syntax: (unless expr else-expr...)
 */
objectp F_unless(objectp args)
{
	objectp p1, p2;
	
	p1 = car(args);
	p2 = cdr(args);
	if (eval(p1) == nil)
		return F_progn(p2);
	
	return nil;
}

/*
 * Syntax: (while expr exprs...)
 */
objectp F_while(objectp args)
{
	objectp p1, p2;
	
	p1 = car(args);
	p2 = cdr(args);
	
	while (eval(p1) != nil)
		F_progn(p2);
	
	return nil;
}

/*
 * Syntax: (progn expr...)
 */
objectp F_progn(objectp args)
{
	objectp p = args, p1;
	do {
	    p1 = eval(car(p));
	    p = cdr(p);
	} while (p != nil);
  
	return p1;
}

/*
 * Syntax: (prog1 expr...)
 */
objectp F_prog1(objectp args)
{
	objectp p = args, first = NULL, p1;
	
	do {
		p1 = eval(car(p));
		if (first == NULL)
			first = p1;
		p = cdr(p);
	} while (p != nil);
	
	if (first == NULL)
		first = nil;
	
	return first;
}

/*
 * Syntax: (prog2 expr...)
 */
objectp F_prog2(objectp args)
{
	objectp p = args, second = NULL, p1;
	int i = 0;
	
	do {
		++i;
		p1 = eval(car(p));
		if (i == 2)
			second = p1;
		p = cdr(p);
	} while (p != nil);
	
	if (second == NULL)
		second = nil;
	
	return second;
}

/* 
 * Syntax (eq expr1 expr2)
 */
objectp F_eqexpr(objectp args)
{
	objectp p1, p2;
	
	p1 = eval(car(args));
	p2 = eval(car(cdr(args)));
	
	if (p1 == p2)
	    return t;
	
	switch (p1->type) {
	    case OBJ_IDENTIFIER:
		    if(p2->type != OBJ_IDENTIFIER)
			return nil;
		    if(strcmp(p1->value.id,p2->value.id) == 0)
			return t;
		    return nil;
		    break;		    

	    case OBJ_INTEGER:
	    case OBJ_DOUBLE:
			if(p2->type != OBJ_DOUBLE && p2->type != OBJ_INTEGER)
			    return nil;
			return F_eq(args);
#ifdef HAVE_GMP
	    case OBJ_NGMP:
		    if(p2->type != OBJ_NGMP)
			return nil;
		    else
			return F_eq(args);
		    break;
#endif
	    case OBJ_CSTRING:
		    if(p2->type != OBJ_CSTRING)
			return nil;
		    else
			if(strcmp(p1->value.s,p2->value.s) == 0)
			    return t;
			else
			    return nil;
		    break;
	    default:
		    return nil;
	}

	return nil;    
}

/*
 * Syntax: (set name value)
 */
objectp F_set(objectp args)
{
	objectp p1 = eval(car(args)), p2 = eval(car(cdr(args)));

	if (p1 == nil || p1 == t) {
		if (opt_lint)
			warnx("warning: setting value to a non valid object");
	} else
		set_object(p1, p2);

	return p2;
}
/*
 * Syntax: (setq name value...)
 * `name' is not evalled
 */

objectp F_setq(objectp args)
{
	objectp p = args, p1, p2;
	
	do {
		p1 = car(p);
		p2 = eval(car(cdr(p)));
		if(p1->type != OBJ_INTEGER && p1->type != OBJ_DOUBLE && p1->type != OBJ_T &&
			p1->type != OBJ_NIL && p1->type != OBJ_CSTRING)
			set_object(p1,p2);
#ifdef HAVE_GMP
		if(p1->type != OBJ_NGMP)
			set_object(p1,p2);
#endif
		else 
			return nil;	
		p = cdr(cdr(p));
	} while (p != nil);
	
	return p2;
}
	
/*
 * Syntax: (let ((name_0 value_0) (name_1 value_1) ... (name_n value_n) expr_0 expr_1 ... expr_n)
 * `name' is not evalled
 * `value' is evalled
 *  always return expr_n :) or nil
 */

objectp F_let(objectp args)
{
	objectp p = args, p1,p2,p3,p4,p5;
	objectp old_objs[MAX_PARAMS];
	objectp new_objs[MAX_PARAMS];
	int i=0;

	p3 = cdr(args);
	
	if(p==nil && p3 == nil)
	    return nil;
	
	p1 = car(car(car(p)));
	p2 = eval (car(cdr(car(car(p)))));
	p4 = eval(p1);

	/* Nice to accept no bindings */
	if(p1 == nil) {
	    return F_progn(p3);
	}

	old_objs[i]=eval(p4);
	set_object(p1,p2);
	new_objs[i]=p1;
	p = cdr(car(p));
	
	if(p == nil && p3 != nil) {
	    p5=F_progn(p3);
	    set_object(new_objs[i],old_objs[i]);
	    return p5;
	}
	
	p1 = car (car (p));
	p2 = eval (car (cdr (car (p))));
	p4 = eval (p1);
	
	if(p1 == nil) {
	    set_object(new_objs[i],old_objs[i]);
	    return nil;
	}
	i++;
	
	old_objs[i]=eval(p4);
	set_object(p1,p2);
	new_objs[i]=p1;
	
	while(1) {
	    p = cdr(p);
	    if(p == nil)
		break;

	    p1 = car (car (p));
	    p2 = eval(car (cdr (car (p))));
	    p4 = eval(p1);

	    if(p1 == nil)
		break;
	    
	    i++;
	    
	    old_objs[i]=eval(p4);
	    set_object(p1,p2);
	    new_objs[i]=p1;
		
	    p = cdr(p);
	    
	    if(p==nil)
		break;
	
	    p1 = car (car (p));
	    p2 = eval(car (cdr (car (p))));
	    p4 = eval (p1);
	
	    if(p1 == nil)
		break;
	
	    i++;

	    old_objs[i]=eval(p4);
	    set_object(p1,p2);
	    new_objs[i]=p1;

	};

	if(p3 != nil) {
	    p5=F_progn(p3);

	    while(i >= 0) {
	        set_object(new_objs[i],old_objs[i]);
		i--;
	    }

	    return p5;

	} else {
	    while(i >= 0) {
	        set_object(new_objs[i],old_objs[i]);
		i--;
	    }
		
	    return nil;
	}
}


objectp F_defmacro(objectp args)
{
	objectp p1 = car(args), p2 = car(cdr(args)), p3 = cdr(cdr(args)),p4;
	
	objectp lexpr;
	
	p4=new_object(OBJ_CONS);
	p4->value.c.car = new_object(OBJ_CONS);
	p4->value.c.car->value.c.car = new_object(OBJ_IDENTIFIER);
	p4->value.c.car->value.c.car->value.id = xstrdup("backquote");
	p4->value.c.car->value.c.cdr = car(p3);
	
		
	lexpr = new_object(OBJ_CONS);
	lexpr->value.c.car = new_object(OBJ_IDENTIFIER);
	lexpr->value.c.car->value.id = xstrdup("lambda");
	lexpr->value.c.cdr = new_object(OBJ_CONS);
	lexpr->value.c.cdr->value.c.car = p2;
	lexpr->value.c.cdr->value.c.cdr = p4;
	set_object(p1, lexpr); 
	
	return lexpr;
}

__inline__ objectp caca(objectp a, objectp b)
{
    objectp p1,x;

    p1=b;
    if(p1->type != OBJ_CONS) {
	if(a==b)
	    return t;
	else
	    return nil;
    }
    do {
	    x=car(p1);
	    if(x->type == OBJ_CONS) {
		    if(caca(a,x) == t)
			    return t;
	    }
	    if(a == x) 
		    return t;
	    
	    p1=cdr(p1);
	    
    } while(p1!=nil);
    return nil;
}

objectp F_defun(objectp args)
{
	objectp p1 = car(args), p2 = car(cdr(args)), p3 = cdr(cdr(args));
	objectp p4 = car(cdr(cdr(args)));
	
	if(p4->type == OBJ_CSTRING) {
	    p3 = cdr(cdr(cdr(args)));
	    if(p3 == nil) {
		p3 = cdr(cdr(args));
		p4 = nil;
	    }
	} else {
	    p4=nil;
	}
    	objectp lexpr;	    
	lexpr = new_object(OBJ_CONS);
	lexpr->value.c.car = new_object(OBJ_IDENTIFIER);
	lexpr->value.c.car->value.id = xstrdup("lambda");
	lexpr->value.c.cdr = new_object(OBJ_CONS);
	lexpr->value.c.cdr->value.c.car = p2;
	lexpr->value.c.cdr->value.c.cdr = p3;
	
	set_object(p1, lexpr); 
	if(p4!=nil) {
	    p1->doc = (char *) xcalloc(strlen(p4->value.s)+1,sizeof(char));
	    strcpy(p1->doc,p4->value.s);
	}
	return lexpr;
}

objectp F_makedoc(objectp args)
{
    objectp func,doc,func_name;
    func = eval(car(args));
    func_name = car(args);
    doc = eval(car(cdr(args)));
    if(func->value.id == NULL)
	return nil;
    func_name->doc = (char *) xcalloc(strlen(doc->value.s)+1,sizeof(char));
    strcpy(func_name->doc,doc->value.s);
    return t;
}

objectp F_getdoc(objectp args)
{
    objectp func;
    func =car(args);
    if(func->type != OBJ_IDENTIFIER || func->value.id == NULL)
	return nil;
    princ_object(stdout, func);
    printf(": %s\n",(func->doc == NULL) ? "no doc" : func->doc);
    return t;
}

objectp eval_func(objectp p, objectp args, bool a[])
{
	objectp p1, p2, p3, p4, p5;
	objectp eval_objs[MAX_PARAMS], save_objs[MAX_PARAMS];
	register int i;
	p1 = car(p);
	if (p1->type == OBJ_IDENTIFIER && !strcmp(p1->value.id, "lambda")) {
		p2 = car(cdr(p));
		p3 = args;
		if (count_list(p2) != count_list(p3)) {
			warnx("`EVAL' warning: expected %d param",count_list(p2));
			return nil;
		}
			/*
			 * Save the new variable values.
			 */
		i = 0;
		do {
			if(a[i] == false)
				p5 = nil;
			else
				p5 = eval(car(p3));
			
			eval_objs[i++] = p5;
			p3 = cdr(p3);
			if(i==MAX_PARAMS) {
				warnx("`EVAL' warning: Consider increase params value.");
				return nil;
			}
		} while (p3 != nil);
		
		/*
		 * Save the old variable values and set the new ones.
		 */
		i = 0;
		
		
		while(p2 != nil) {
			p4 = car(p2);
			save_objs[i] = get_object(p4);
			set_object(p4, eval_objs[i]);
			p2 = cdr(p2);
			++i;
			
		}
		p5 = F_progn(cdr(cdr(p)));
		
			/*
			 * Restore the old variable values.
			 */
		p2 = car(cdr(p));
		i = 0;

		while (p2 != nil) {
			p4 = car(p2);
			set_object(p4,save_objs[i++]);
			p2 = cdr(p2);
		}
		
	return p5;
	}
	
	return nil;
}


objectp F_gc(objectp args)
{
	garbage_collect();
	
	return t;
}

objectp F_date(objectp args)
{
	objectp p,p1;
	struct timeval tv;
	struct tm *ptm;

	p1=new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(56, sizeof(char));
	gettimeofday(&tv, NULL);
	ptm = localtime((const time_t *) &tv.tv_sec);
	p=eval(car(args));

	if( p != NULL && ((p->type) == OBJ_CSTRING)) {
		strftime (p1->value.s, 56 ,p->value.s, ptm);
	} else {
		strftime (p1->value.s, 56 ,"%a %b %d %H:%M:%S %Z %Y", ptm);
	}
	return p1;
}

objectp F_exec(objectp args)
{
	objectp p,p1;
	char **v;
	int n=0,child_status;
	pid_t child;
	p=args;
	v=(char **) xcalloc(32,sizeof(char));
	do {
		p1 = eval(car(p));
		if (p1->type == OBJ_CSTRING) {
			v[n]=p1->value.s;
			n++;
		}
		else
			return nil;
		p = cdr(p);
	} while (p != nil);
	child=fork();
	if(child == 0) {
			/* children process */
	    execve(v[0],v,NULL);
	    abort(); /* if execve fails */
	}
	else
			/* parent process */
	    child_status = child; 
	
	wait(&child_status);
	
	if(WIFEXITED(child_status)) 
	    printf("%d\n",WEXITSTATUS(child_status));
	else {
	    return nil;
	}
	return t;
}
#ifdef HAVE_READLINE

objectp F_reads(objectp args)
{
    objectp p;

    if(fgets(buffer,1024,input_file) == NULL)
	    return nil;
    buffer[strlen(buffer)] = '\0';
    p=parse_object(0);
    return p;
}
#else

objectp F_reads(objectp args)
{
	objectp p;
	char buf[1024];
	if(fgets(buf,1024,input_file) == NULL)
		return nil;
	buf[strlen(buf)] = '\0';
	p=parse_object(0);
	return p;
}
#endif	

#ifdef HAVE_YACAS

int analize(char *str)
{
	int c;
	c=*str;
	if(isalpha(c) || c == '(')
		return 1;
	else
		return 0;
}

objectp F_yacas(objectp args)
{
	objectp p,p1;
	char buf[1100];
	int len;
	p=eval(car(args));
	
	if(p->type != OBJ_CSTRING) {
		warnx("use: (yacas \"expr\")");
		return nil;
	}
	
	yacas_init();
	snprintf(buf,1099,"FullForm(Eval(FromString(\"%s\") LispRead()))",p->value.s);
	yacas_eval(buf);
	if(analize(yacas_output()) == 0) {
		warnx("Can't understand yacas output: %s",stripwhite(yacas_output()));
		return nil;
	}
	if(!yacas_error()) {
#ifdef HAVE_READLINE
		strncpy(buf,yacas_output(),1023);
		buffer=stripwhite(buf);
		len=strlen(buffer);
		buffer[len] = '\0';	    
		p1=parse_object(0);
		yacas_exit();
		return p1;
#else
		strcnpy(buf,yacas_output(),1023);
		buf=stripwhite(buf);
		len=strlen(buf);
		buf[len] = '\0';
		p1=parse_object(0);
		yacas_exit();
		return p1;
#endif
	} else
		warnx("yacas: error evaluation expression");
	return nil;
	
}

void runexpr(char* expr)
{
	
	yacas_eval(expr);
	if (yacas_error())
	{
		printf("Error> %s",yacas_error());
	}  
	else if (yacas_result())
	{
		printf("Out> %s\n",yacas_result());
	}
}

#else


objectp F_yacas(objectp args)
{
    warnx("warning: YACAS is not usable");
    return nil;
}


#endif


objectp F_consp(objectp args)
{
    objectp p = eval(car(args));

	if( (p->type) == OBJ_CONS) 
		return t;

	return nil;
}



objectp F_open(objectp args)
{
    objectp fn,mode,fd;
    register int i;
    fn = car(args);
    mode= car(cdr(args));

    if(fn->type != OBJ_CSTRING || mode->type != OBJ_CSTRING) {
	    warnx("(fopen \"filename\" \"mode\")");
	    return nil;
    }
    for(i=0;i<FILES_N;i++)
	    if(used[i] == 0)
		    break;
    if(i==FILES_N) {
	    warnx("No more free streams");
	    return nil;
    }
    
    if((fds[i] = fopen(fn->value.s,mode->value.s)) == NULL) {
	    warnx("error opening file: %s",fn->value.s);
	    return nil;
    }
    used[i] = 1;
#ifdef HAVE_GMP
    fd=new_object(OBJ_NGMP);
    fd->value.s = xmalloc(32*sizeof(char));
    snprintf(fd->value.s,32,"%d",i);
#else
    fd=new_object(OBJ_INTEGER);
    fd->value.i = i;
#endif
    return fd;
    
}

objectp F_close(objectp args)
{
	objectp p = eval(car(args));
	int fd;
	
#ifdef HAVE_GMP
	if(p->type != OBJ_NGMP) {
		warnx("(fclose fd)");
		return nil;
	}
	fd=atoi(p->value.s);
#else
	if(p->type != OBJ_INTEGER) {
		warnx("(fclose fd)");    
		return nil;
	}
	fd = p->value.i;
#endif
	if(fd>=FILES_N || fds[fd] == 0) {
		warnx("file descriptor is invalid");
		return nil;
	}
	if(fclose(fds[fd]) != 0) {
		warnx("error closing file");
		return nil;
	}
	used[fd] = 0;
	return t;
}


objectp F_rewind(objectp args)
{
	objectp p;
	int i;
	p=eval(car(args));
#ifdef HAVE_GMP
	if(p->type != OBJ_NGMP) {
		warnx("(frewind fd)");
		return nil;
	}
	i=atoi(p->value.s);
#else
	if(p->type != OBJ_INTEGER) {
		warnx("(frewind fd)");
		return nil;
	}
	i=p->value.i;
#endif
	if(i>=FILES_N || fds[i] == 0) {
	warnx("file descriptor is invalid");
	return nil;
	}
	rewind(fds[i]);
	return t;
}


objectp F_tell(objectp args)
{
	objectp ret,p;
	int i;
	double ft;
	p=eval(car(args));
#ifdef HAVE_GMP
	if(p->type != OBJ_NGMP) {
		warnx("(ftell fd)");
		return nil;
	}
	i=atoi(p->value.s);
#else
	if(p->type != OBJ_INTEGER) {
		warnx("(ftell fd)");
		return nil;
	}
	i=p->value.i;
#endif
	if(i>=FILES_N || fds[i] == 0) {
		warnx("invalid file descriptor");
		return nil;
	}
	ft=ftell(fds[i]);
#ifdef HAVE_GMP
	ret = new_object(OBJ_NGMP);
	ret->value.s = (char *) xcalloc(32,sizeof(char));
	sprintf(ret->value.s,"%lu",(long)ft);
#else
	ret = new_object(OBJ_DOUBLE);
	ret->value.d = (double) ft;
#endif
    return ret;
}

objectp F_seek(objectp args)
{
	objectp fin, offset, whence,ret;
	int off,w,fd,r;
	fin=eval(car(args));
	offset=eval(car(cdr(args)));
	whence=eval(car(cdr(cdr(args))));
	
#ifdef HAVE_GMP
	if(fin->type != OBJ_NGMP || offset->type != OBJ_NGMP ||
		whence->type != OBJ_CSTRING) {
		warnx("(fseek fd offset whence)\twhence = \"SEEK_SET\" || \"SEEK_CUR\" || \"SEEK_END\"");
		return nil;
	}
	fd = atoi(fin->value.s);
	off = atoi(offset->value.s);
#else
	if(fin->type != OBJ_INTEGER || offset->type != OBJ_INTEGER ||
		whence->type != OBJ_CSTRING) {
		warnx("(fseek fd offset whence)\twhence = \"SEEK_SET\" || \"SEEK_CUR\" || \"SEEK_END\"");
		return nil;
	}
	fd = fin->value.i;
	off = offset->value.i;
#endif    
	if(strcmp(whence->value.s,"SEEK_SET") == 0)
		w=0;
	else if(strcmp(whence->value.s,"SEEK_CUR") == 0)
		w=1;
	else if(strcmp(whence->value.s,"SEEK_END") == 0)
		w=2;
	else {
		warnx("(fseek fd offset whence)\twhence = \"SEEK_SET\" || \"SEEK_CUR\" || \"SEEK_END\"");
		return nil;
	}
	if(fd>=FILES_N || fds[fd] == 0) {
		warnx("invalid file descriptor");
		return nil;
	}
	r=fseek(fds[fd],off,w);
#ifdef HAVE_GMP
	ret = new_object(OBJ_NGMP);
	ret->value.s = (char *) xcalloc(32,sizeof(char));
	sprintf(ret->value.s,"%d",r);
#else
	ret = new_object(OBJ_INTEGER);
	ret->value.i = r;
#endif
	return ret;
}

objectp F_read(objectp args)
{
    objectp fin,size,size_elem;
    objectp line;
    int siz, nmemb,fd;
    fin = eval(car(args));
    size = eval(car(cdr(args)));
    size_elem = (eval(car(cdr(cdr(args)))));
#ifdef HAVE_GMP
    if(fin->type != OBJ_NGMP || size->type != OBJ_NGMP || 
	    size_elem->type != OBJ_NGMP) {
	    warnx("(fread fd number_objects size_of_object)");
	    return nil;
    }
    fd = atoi(fin->value.s);
    siz = atoi(size->value.s);
    nmemb = atoi(size_elem->value.s);
#else
    if(fin->type != OBJ_INTEGER || size->type != OBJ_INTEGER || 
	    size_elem->type != OBJ_INTEGER) {
	    warnx("(fread fd number_objects size_of_object)");
	    return nil;
    }
    fd = fin->value.i;
    siz= size->value.i;
    nmemb = size_elem->value.i;
#endif
    if(fd>=FILES_N || fds[fd] == 0) {
	warnx("invalid file descriptor");
	return nil;
    }
    line=new_object(OBJ_NGMP);
    line->value.s = (char *) xcalloc(nmemb,sizeof(char));
    fread(line->value.s,siz,nmemb,fds[fd]);
    printf("%s\n",line->value.s);
    return line;
}

objectp F_write(objectp args)
{
	objectp x,out;
	int i;
	x=eval(car(args));
	out=eval(car(cdr(args)));
#ifdef HAVE_GMP
	if(x->type != OBJ_NGMP) {
		warnx("(fwrite fd expr)\t expr is evalled");
		return nil;
	}
	i=atoi(x->value.s);
#else
	if(x->type != OBJ_INTEGER) {
		warnx("(fwrite fd expr)\t expr is evalled");
		return nil;
	}
	i=x->value.i;
#endif
	if(i>=FILES_N || fds[i] == 0) {
		warnx("invalid file descriptor");
		return nil;
	}
	princ_object(fds[i],out);
	return t;
}

objectp F_dump_memory(objectp args)
{
	objectp p = car(args);
	
	if (p != nil && cdr(args) == nil && p->type == OBJ_CSTRING) {
		if (strlen(p->value.s) > 0) {
			dump_objects(p->value.s);
			return t;
		} else
			warnx("`DUMP_MEMORY' warning: expected filename");
	} else
		warnx("`DUMP_MEMORY': wrong number of parameters (expected string)");
	
	return nil;
}
objectp concaten(objectp p1, objectp p2)
{
	objectp p;    

	p = new_object(OBJ_CONS);
	p->value.c.car = p1;
	p->value.c.cdr = p2;
	
	return p;
}


/* logical or */

objectp F_lor(objectp args)
{
	objectp p = args, p1;
	int lor=0;
	p1=eval(car(p));
#ifndef HAVE_GMP
	if (p1->type == OBJ_INTEGER)
		lor = p1->value.i;
#endif
#ifdef HAVE_GMP
	if(p1->type == OBJ_NGMP)
		lor = (int) strtol(p1->value.s, (char **) NULL, 0);
#endif
	else if (opt_lint) {
		warnx("`OR' warning: there is a non numeric operand");
		return nil;
	} else
		return nil;
	
	p=cdr(p);
	
	do {
		p1 = eval(car(p));
#ifndef HAVE_GMP
		if (p1->type == OBJ_INTEGER)
			lor|=p1->value.i;
#endif
#ifdef HAVE_GMP
		if(p1->type == OBJ_NGMP)
			lor |= (int) strtol(p1->value.s, (char **) NULL, 0);
#endif
		else if (opt_lint) {
			warnx("`OR' warning: there is a non numeric operand");
			return nil;
		} else
			return nil;
		p = cdr(p);
	} while (p != nil);
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	sprintf(p1->value.s, "%d",lor);
#else
	p1 = new_object(OBJ_INTEGER);
	p1->value.i = lor;  
#endif
	return p1;
}

/* logical and */

objectp F_land(objectp args)
{
    objectp p = args, p1;
    int andv=0;
    p1=eval(car(p));

#ifndef HAVE_GMP
	if (p1->type == OBJ_INTEGER)
		andv = p1->value.i;
#endif
#ifdef HAVE_GMP
	if(p1->type == OBJ_NGMP)
		andv = (int) strtol(p1->value.s, (char **) NULL, 0);
#endif
	else if (opt_lint) {
		warnx("`AND' warning: argument must be an integer");
		return nil;
	} else
		return nil;

	p=cdr(p);
	
	do {
		p1 = eval(car(p));
#ifndef HAVE_GMP
		if (p1->type == OBJ_INTEGER)
		    andv &= p1->value.i;
#endif
#ifdef HAVE_GMP
		if(p1->type == OBJ_NGMP)
			andv &= (int) strtol(p1->value.s, (char **) NULL, 0);
#endif
		else if (opt_lint) {
			warnx("`AND' warning: argument must be an integer");
			return nil;
		} else
			return nil;
		p = cdr(p);
	} while (p != nil);
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	sprintf(p1->value.s, "%d",andv);
#else	
	p1 = new_object(OBJ_INTEGER);
	p1->value.i = andv;  
#endif
	return p1;
}

/* logical xor */

objectp F_lxor(objectp args)
{
	objectp p = args, p1;
	int xorv=0;
	p1=eval(car(p));

#ifndef HAVE_GMP
	if (p1->type == OBJ_INTEGER)
		xorv = p1->value.i;
#endif
#ifdef HAVE_GMP
	if(p1->type == OBJ_NGMP)
	    xorv = (int) strtol(p1->value.s, (char **) NULL, 0);
#endif
	else if (opt_lint) {
		warnx("`XOR' warning: argument must be an integer");
		return nil;
	} else
		return nil;
	
	p=cdr(p);
	
	do {
		p1 = eval(car(p));
#ifndef HAVE_GMP
		if (p1->type == OBJ_INTEGER)
			xorv ^= p1->value.i;
#endif
#ifdef HAVE_GMP
		if(p1->type == OBJ_NGMP)
			xorv = (int) strtol(p1->value.s, (char **) NULL, 0);
#endif
		else if (opt_lint) {
			warnx("`XOR' warning: there is a non numeric operand");
			return nil;
		} else
			return nil;
		p = cdr(p);
	} while (p != nil);
#ifdef HAVE_GMP
	p1 = new_object(OBJ_NGMP);
	p1->value.s = (char *) xcalloc(16,sizeof(char));
	sprintf(p1->value.s, "%d",xorv);
#else
	p1 = new_object(OBJ_INTEGER);
	p1->value.i = xorv;  
#endif
	return p1;
}

objectp F_listf(objectp args)
{
	show_f();
	return t;
}

objectp F_sets(objectp args)
{
	objectp p=eval(car(args));
#ifdef HAVE_GMP
	int stacks;
#endif
	
	if( p->type == OBJ_INTEGER) {
		if(p->value.i >= INT_MAX || p->value.i <= 0)  {
			warnx("warning: 0 > n < 2^31");
			return nil;
		}
		MAX_USED = p->value.i;
		return t;
	}
#ifdef HAVE_GMP
	if( p->type == OBJ_NGMP) {
		stacks = (int) strtol(p->value.s, (char **) NULL, 0);
		if(stacks >= INT_MAX || stacks <= 0)  {
			warnx("warning: 0 > n < 2^31");
			return nil;
		}
		MAX_USED = stacks;
		return t;
	}
#endif		
	else
		return nil;
}

objectp F_setmax(objectp args)
{
    objectp p=eval(car(args));
#ifdef HAVE_GMP
    int params;
#endif

#ifdef HAVE_GMP
    if( p->type == OBJ_INTEGER) {
	    if(p->value.i >= INT_MAX || p->value.i <= 0)  {
		    warnx("warning: 0 > n < 2^31");
		    return nil;
	    }
		MAX_PARAMS = p->value.i;
		return t;
    }
#endif
#ifdef HAVE_GMP    
    if( p->type == OBJ_NGMP) {
	    params = (int) strtol(p->value.s, (char **) NULL, 0);
	    if(params >= INT_MAX || params <= 0)  {
		    warnx("warning: 0 > n < 2^31");
		    return nil;
	    }
	    MAX_PARAMS = params;
	    return t;
    }
#endif
    else
	    return nil;
}

objectp F_shows(objectp args)
{
    printf("Stack size: %d\n",MAX_USED);
    return t;
}

objectp F_showmax(objectp args)
{
    printf("MAX PARAMS: %d\n",MAX_PARAMS);
    return t;
}

objectp F_lambda(objectp args) 
{
    return (cdr(args));
}

objectp F_values(objectp args)
{
	objectp p,p1,p2;
	
	p=args;
	p1=eval(car(args));
	do {
		p2=eval(car(p));
		princ_object(stdout,p2);
		printf(" ;\n");
		p=cdr(p);
	} while(p!=nil);
	return p1;
}

objectp F_areeq(objectp l1, objectp l2)
{
#ifdef HAVE_GMP
    mpf_t a,b;
    mpf_init(a);
    mpf_init(b);
#endif
    objectp i,j;

    do {
	    i=car(l1);
	    j=car(l2);
	    switch (i->type) {
		    case OBJ_CONS:
			    F_areeq(i,j);
			    break;
		    case OBJ_IDENTIFIER:
			    if(j->type != OBJ_IDENTIFIER)
				    return nil;
			    if(strcmp(i->value.id,i->value.id) != 0)
				    return nil;
			    break;		    
#ifndef HAVE_GMP
		    case OBJ_INTEGER:
			    if(j->type != OBJ_INTEGER)
				    return nil;
			    if(j->value.i != i->value.i)
				    return nil;
			    break;
		    case OBJ_DOUBLE:
			    if(j->type != OBJ_DOUBLE)
				    return nil;
			    if(j->value.d != i->value.d)
				    return nil;
#endif
		    case OBJ_CSTRING:
			    if(j->type != OBJ_CSTRING)
				    return nil;
			    if(strcmp(i->value.s,j->value.s) == 0) {
				    return t;
			    } else
				    return nil;
			    break;
#ifdef HAVE_GMP
		    case OBJ_NGMP:
			    if(j->type != OBJ_NGMP)
				    return nil;
			    mpf_set_str(a,i->value.s,0);
			    mpf_set_str(b,j->value.s,0);
			    if(mpf_cmp(a,b) != 0)
				    return nil;
			    break;			
#endif
		    default:
			    return nil;
	    }
	    
	    l1=cdr(l1);
	    l2=cdr(l2);
    } while(l1 != nil || l2 != nil);
    return t;
}	

objectp F_eqlis(objectp args)
{
    objectp p = args;
    objectp p1,p2;

    p1=eval(car(p));
    p2=eval(car(cdr(p)));
    
    if(p1->type != OBJ_CONS || p2->type != OBJ_CONS) {
	    warnx("(eql list_0 list_1)");
	    return nil;
    }

    if(F_areeq(p1,p2) == nil)
	    return nil;
    return t;
}

objectp F_insert (objectp args)
{
	objectp p,p1,p2,p3,first = NULL, prev = NULL;
	int pos;
	
	p=eval(car(args));
	p2=eval(car(cdr(args)));
	p3=eval(car(cdr(cdr(args))));
	
	if (p3 == nil)
		return nil;
#ifndef HAVE_GMP
	if(p2->type == OBJ_INTEGER)
		pos = p2->value.i;
#endif
	
#ifdef HAVE_GMP
	if(p2->type == OBJ_NGMP)
		pos = (int) strtol(p2->value.s, (char **) NULL, 10);
#endif
	
	else if(opt_lint) {
		warnx("(insert elem list)");
		return nil;
	} else 
		return nil;
	if(pos >= count_list(p3)) {
		warnx("`INSERT' warning: out of range");
		return nil;
	}
	do {
		p1 = new_object(OBJ_CONS);
		if(pos==0) {
			p1->value.c.car =p;
			if (first == NULL)
				first = p1;
			if (prev != NULL)
				prev->value.c.cdr = p1;
			prev = p1;
			
		} else {
			
			p1->value.c.car = eval(car(p3));
			if (first == NULL)
				first = p1;
			if (prev != NULL)
				prev->value.c.cdr = p1;
			prev = p1;
			p3 = cdr(p3);
		}
		pos--;
	} while (p3 != nil);
	
	return first;
}

objectp F_mix(objectp args)
{
	objectp p, p1, p2, first = NULL, prev = NULL;
	p1=eval(car(args));
	p2=eval(car(cdr(args)));

	if(p1->type != OBJ_CONS || p2->type != OBJ_CONS)
		return nil;
	
	do {
		p = new_object(OBJ_CONS);
		p->value.c.car = new_object(OBJ_CONS);
		p->value.c.car->value.c.car = eval(car(p1));
		p->value.c.car->value.c.cdr = new_object(OBJ_CONS);
		p->value.c.car->value.c.cdr->value.c.car = eval(car(p2));
		if(first == NULL)
			first = p;
		if(prev != NULL)
			prev->value.c.cdr = p;
		prev = p;

		p1=cdr(p1);
		p2=cdr(p2);
	} while(p1 != nil && p2 != nil);

	return first;
}

objectp F_setprec(objectp args) 
{
    objectp p=eval(car(args));
#ifdef HAVE_GMP
    int prec=0;
#endif

#ifndef HAVE_GMP
    if( p->type == OBJ_INTEGER) {
	    if(p->value.i >= INT_MAX || p->value.i <= 0)  {
		    warnx(" 0 > n < 2^31");
		    return nil;
	    }
	    PRECISION = p->value.i;
	    return t;
    }
#endif
	    
#ifdef HAVE_GMP	    
    if( p->type == OBJ_NGMP) {
	    prec = (int) strtol(p->value.s, (char **) NULL, 0);
	    if(prec >= INT_MAX || prec <= 0)  {
		    warnx(" 0 > n < 2^31");
		    return nil;
		    }
	    PRECISION=prec;
	    return t;
    }
#endif
    else if(opt_lint) {
	    warnx("argument must be an integer");
	    return nil;
    }
    return nil;
}


objectp F_typeof(objectp args)
{
	objectp p=eval(car(args));
	
	switch(p->type) {
	    case OBJ_NIL:
		    printf("nil\n");
		break;
		case OBJ_T:
			printf("t\n");
			break;
#ifndef HAVE_GMP
		case OBJ_INTEGER:
			printf("Integer\n");
			break;
		case OBJ_DOUBLE:
			printf("Double\n");
			break;
#endif
#ifdef HAVE_GMP
		case OBJ_NGMP:
			printf("Numeric (GMP)\n");
			break;
#endif
		case OBJ_CSTRING:
			printf("String\n");
			break;
		case OBJ_CONS:
			printf("List\n");
			break;
		case OBJ_IDENTIFIER:
			if(strcmp(p->value.id,"lambda") == 1)
				printf("Function\n");
			else
				printf("Identifier\n");
			break;
	}
	return t;
}

objectp F_funcall(objectp args)
{
	objectp p,p1,p2,body,x;
	bool array[MAX_PARAMS];
	int i=0;
	
	p = car(args);
	p1= cdr(args);

	p2 = new_object(OBJ_CONS);
	p2->value.c.car = new_object(OBJ_IDENTIFIER);
	p2->value.c.car->value.id = xstrdup("lambda");
	p2->value.c.cdr =  new_object(OBJ_CONS);
	p2->value.c.cdr->value.c.car = car(p);
	p2->value.c.cdr->value.c.cdr = cdr(p);

	body = car(p);

	do {
		x=car(body);
		if(caca(x,car(cdr(p))) == t) {
			array[i] = true;
		} else {
			array[i] = false;
		}
		i++;
		body=cdr(body);
	} while(body != nil);
	return eval_func(p2,p1,array);
}

objectp F_evale(objectp args)
{
    objectp body, p, r;
    body = car(cdr(car(args)));
    p = car(cdr(car(cdr(args))));
    r=new_object(OBJ_CONS);
    r->value.c.car = new_object(OBJ_IDENTIFIER);
    r->value.c.car->value.id = xstrdup("let");
    r->value.c.cdr = new_object(OBJ_CONS);
    r->value.c.cdr->value.c.car = p;
    r->value.c.cdr->value.c.cdr = new_object(OBJ_CONS);
    r->value.c.cdr->value.c.cdr->value.c.car = body;
    
    return eval_cons(r);
}

objectp F_evlis(objectp args)
{
    objectp body, p, r,listb;

    body = car(cdr(car(args)));
    p = car(cdr(car(cdr(args))));
    listb=new_object(OBJ_CONS);

    listb->value.c.car = new_object(OBJ_IDENTIFIER);
    listb->value.c.car->value.id = xstrdup("list");
    listb->value.c.cdr  = body;

    r=new_object(OBJ_CONS);
    r->value.c.car = new_object(OBJ_IDENTIFIER);
    r->value.c.car->value.id = xstrdup("let");
    r->value.c.cdr = new_object(OBJ_CONS);
    r->value.c.cdr->value.c.car = p;
    r->value.c.cdr->value.c.cdr = new_object(OBJ_CONS);
    r->value.c.cdr->value.c.cdr->value.c.car = listb;
    
    return eval_cons(r);
}

objectp F_labels(objectp args)
{
	objectp p = args, p1,p2,p3,p4,p5,p6,p7;
	objectp old_objs[MAX_PARAMS];
	objectp new_objs[MAX_PARAMS];
	int i=0;

	p3 = cdr(args);
	
	if(p==nil && p3 == nil)
	    return nil;

	/* labels argument format is like:
	   (labels ( (f_1 (args) (body)) (f_2 (args) (body)) ... ) expr_1 ... )
	   here:
	   p1 = f_i
	   p2 = args_i
	   p7 = body
	   p6 = (lambda (p2) (p7))

	   then we set to p1 (function name) p6
	*/
	   
	p1 = car(car(car(p)));
	p2 = car(cdr(car(car(p))));
	p7 = cdr(cdr(car(car(p))));

	p6 = new_object(OBJ_CONS);
	p6->value.c.car = new_object(OBJ_IDENTIFIER);
	p6->value.c.car->value.id = xstrdup("lambda");
	p6->value.c.cdr = new_object(OBJ_CONS);
	p6->value.c.cdr->value.c.car = p2;
	p6->value.c.cdr->value.c.cdr = p7;

	p4 = eval(p1);

	/* Nice to accept no bindings */
	if(p1 == nil) {
		return F_progn(p3);
	}
	
	old_objs[i]=eval(p4);
	set_object(p1,p6);
	new_objs[i]=p1;
	p = cdr(car(p));
	
	if(p == nil && p3 != nil) {
		p5=F_progn(p3);
		set_object(new_objs[i],old_objs[i]);
		return p5;
	}
	
	p1 = car (car (p));
	p2 = car (cdr (car (p)));
	p7 = cdr (cdr (car (p)));
	p6 = new_object(OBJ_CONS);

	p6->value.c.car = new_object(OBJ_IDENTIFIER);
	p6->value.c.car->value.id = xstrdup("lambda");
	p6->value.c.cdr = new_object(OBJ_CONS);
	p6->value.c.cdr->value.c.car = p2;
	p6->value.c.cdr->value.c.cdr = p7;
	
	p4 = eval (p1);
	
	if(p1 == nil) {
		set_object(new_objs[i],old_objs[i]);
		return nil;
	}
	i++;
	
	old_objs[i]=eval(p4);
	set_object(p1,p6);
	new_objs[i]=p1;
	
	while(1) {
	    p = cdr(p);
	    if(p == nil)
		    break;

	    p1 = car (car (p));
	    p2 = car (cdr (car (p)));
	    p7 = cdr (cdr (car (p)));
	    p6 = new_object(OBJ_CONS);
	    p6->value.c.car = new_object(OBJ_IDENTIFIER);
	    p6->value.c.car->value.id = xstrdup("lambda");
	    p6->value.c.cdr = new_object(OBJ_CONS);
	    p6->value.c.cdr->value.c.car = p2;
	    p6->value.c.cdr->value.c.cdr = p7;

	    p4 = eval(p1);

	    if(p1 == nil)
		    break;
	    
	    i++;
	    
	    old_objs[i]=eval(p4);
	    set_object(p1,p6);
	    new_objs[i]=p1;
		
	    p = cdr(p);
	    
	    if(p==nil)
		    break;
	
	    p1 = car (car (p));
	    p2 = car (cdr (car (p)));
	    p7 = cdr (cdr (car (p)));
	    p6 = new_object(OBJ_CONS);
	    p6->value.c.car = new_object(OBJ_IDENTIFIER);
	    p6->value.c.car->value.id = xstrdup("lambda");
	    p6->value.c.cdr = new_object(OBJ_CONS);
	    p6->value.c.cdr->value.c.car = p2;
	    p6->value.c.cdr->value.c.cdr = p7;

	    p4 = eval (p1);
	
	    if(p1 == nil)
		break;
	
	    i++;

	    old_objs[i]=eval(p4);
	    set_object(p1,p6);
	    new_objs[i]=p1;

	};

	if(p3 != nil) {
	    p5=F_progn(p3);

	    while(i >= 0) {
	        set_object(new_objs[i],old_objs[i]);
		i--;
	    }

	    return p5;

	} else {
	    while(i >= 0) {
	        set_object(new_objs[i],old_objs[i]);
		i--;
	    }
		
	    return nil;
	}
}

#ifdef HAVE_GMP

objectp F_dotimes(objectp args)
{
	objectp x,to,body,a;
	objectp save_x;
    
	mpz_t i,j;
	
	mpz_init(i); 
	mpz_init(j);
	x=car(car(args));
	to=eval(car(cdr(car(args))));
	body=cdr(args);
	save_x = get_object(x);
	
	if(to->type != OBJ_NGMP) {
		return nil;
	}
	mpz_set_str(i,"0",0);
	mpz_set_str(j,to->value.s,0);
	
	while(mpz_cmp(i,j)) {
		a=new_object(OBJ_NGMP);
		a->value.s = xcalloc(128,sizeof(char));
		gmp_snprintf(a->value.s,128,"%Zd",i);
		set_object(x,a);
		F_progn(body);
		mpz_add_ui(i,i,1);
	}
	
	mpz_clear(i);
	mpz_clear(j);
	set_object(x,save_x);
	return nil;
}

#else

objectp F_dotimes(objectp args)
{
	objectp x,to,body,save_x,a;
	double i,j;
	x=car(car(args));
	to=eval(car(cdr(car(args))));
	body=cdr(args);
	save_x = get_object(x);
	
	if(to->type == OBJ_DOUBLE) {
		j=to->value.d;
		i=0.0;
		
		while(i<j) {
			a=new_object(OBJ_DOUBLE);
			a->value.d = i;
			set_object(x,a);
			F_progn(body);
			i++;
		}
		
	} else if(to->type == OBJ_INTEGER) {
		j = to->value.i;
		i = 0;
		while(i<j) {
			a=new_object(OBJ_INTEGER);
			a->value.i = i;
			set_object(x,a);
			F_progn(body);
			i++;
		}
		
	} else 
		return nil;
	set_object(x,save_x);
	return nil;
}
#endif


int compar(const void *p1, const void *p2)
{
	return strcasecmp(((funcs *)p1)->name,((funcs *)p2)->name);
}

objectp eval_cons(objectp p)
{
	objectp p1 = car(p), p2 = cdr(p), p3,x,body;
	bool array[MAX_PARAMS];
	int i=0;
	if (p1 != nil && p1->type == OBJ_IDENTIFIER) {
		funcs key, *item;
		
		if (!strcmp(p1->value.id, "lambda"))
			return p;
		key.name = p1->value.id;
		if ((item =
			 bsearch(&key, functions, sizeof functions / sizeof functions[0],
					 sizeof functions[0], compar)) != NULL)

			return item->func(p2);
		
		if ((p3 = get_object(p1)) != nil) {
			body = car(cdr(p3));
			do {
			    x=car(body);
			    if(caca(x,car(cdr(cdr(p3)))) == t)
				array[i] = true;
			    else
				array[i] = false;
			    i++;
			    
			    body=cdr(body);
			} while(body != nil);
			return eval_func(p3, p2,array);
		} else {
			warnx("`EVAL': function `%s' is undefined", p1->value.id);
			return nil;
		}
	}
	
	return nil;
}


objectp eval(objectp p)
{
	switch (p->type) {
		case OBJ_IDENTIFIER:
			return get_object(p);
#ifndef HAVE_GMP
		case OBJ_INTEGER:
		case OBJ_DOUBLE:
#endif
#ifdef HAVE_GMP
		case OBJ_NGMP:
#endif
		case OBJ_CSTRING:
			return p;
		case OBJ_CONS:
			return eval_cons(p);
	}
	return p;
}

int show_f(void)
{
	int i,t,hm,j;

	t=sizeof functions / sizeof functions[0];
	
	for(i = 0 ; i < t-1 ; i++) {
		hm=0;
		hm=printf("(%s)",functions[i].name);
		hm=20-hm;
		for(j = 0; j < hm ; j++)
			printf(" ");
		if( (i % 4) == 0 && i != 0)
			printf("\n");
	}
	print_objects();
	printf("\n");
	return 0;
}


objectp F_eval(objectp args)
{
	return eval(eval(car(args)));
}
