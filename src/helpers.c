#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "err.h"
#include "version.h"

char *progname = "slisp";


extern void dump_objects(char *fname);

/* Some helpers for func.s */

extern int up(const void *a, const void *b)
{
	double c,d;
	c=*(double *)a;
	d=*(double *)b;

	if(c>d) 
		return 1;
	else if(c<d)
		return -1;
	
	return 0;
}

extern int down(const void *a, const void *b)
{
	double c,d;
	c=*(double *)b;
	d=*(double *)a;

	if(c>d) 
		return 1;
	else if(c<d)
		return -1;

	return 0;
}


extern void dec2bin(int byte )
{
	int count=8;
	int MASK = 1 << (count-1);
	
	while(count--) {
		printf("%d", ( byte & MASK ) ? 1 : 0 );
		byte <<= 1;
	} 
	
	printf("\n");
}

extern int bin2dec(char *s)
{
	int r=0;
	for ( ; *s ; )
		r = ((r<<1) | (*s++ - '0'));
	
	return r;
}


#ifndef HAVE_GMP

extern int modexp(int m,int x,int n)
{
	
    int i,r=1;

    for(i = 0;i < x;i++) 
		r= (r*m) % n;

    return r;
}


extern int isprime(unsigned int n)
{
	unsigned int i, r, c;
	
	if (n <= 1)
		return 0;
	if(n <= 3)
		return 1;
	i = 2;
	while ( ((r= n % i) != 0) && ((c = n / i) > i) )
		++i;
	return (r != 0);
	
}
#endif

/* This function determines whatever an string is a number or not.
   i.e: "23" is a number. "23.2.2" is not a number
   returns 1 if string is a number, 0 if it nots
*/

extern int num_string(char *s1)
{
	unsigned short int dots =0;
	register char *s;
	
	s=s1;
	
	if(*s == '-' || *s == '+')
		s++;
	
	while(*s) {
	    if(*s == '.' && dots <= 1)
		    s++;
	    
	    if(! isdigit(*s))
		    return 0;
	    s++;
	}
	return 1;
}

extern unsigned int phi(unsigned int n)
{
	unsigned int p;
	float f;
	
	f=n;
	if ((float)n/2 == n/2) {
		f *= (1-1.0/2);
		while ((float)n/2 == n/2) 
			n/=2;
	}
	
	p=3;
	while (p*p <= n) {
		if ((float)n/p == n/p) {
			f *= (1-1.0/p);
			while ((float)n/p == n/p) 
				n/=p;
		}
		p += 2;
	}
	if (n!=1) 
		f *= (1-1.0/n);
	
	return (unsigned int)f;
}

extern int gcd(int a, int b)
{
	int r,t;
	if(a<b) {
		t=a;
		b=t;
		a=b;
	}
	while(b) {
		r = a % b;
		a = b;
		b = r;
	}
	
	return a;
}

/* see Handbook of applied cryptography */

extern int JACOBI(int a, int n)
{
	int s;
	int a1, b = a, e = 0, m, n1;
	s=0;
	
	if (a == 0)
		return 0;
	if (a == 1)
		return 1;
	while ((b & 1) == 0)
		b >>= 1, e++;

	a1 = b;
	m = n % 8;
	
	if (!(e & 1))
		s = 1;
	else if (m == 1 || m == 7)
		s = + 1;
	else if (m == 3 || m == 5)
		s = - 1;
	if ( (n % 4 == 3) && (a1 % 4 == 3) )
		s = - s;
	if (a1 != 1)
		n1 = n % a1;
	else
		n1 = 1;
	
	return s * JACOBI(n1, a1);
}

extern int inverse(int mod, int num)
{
	int g[20], u[20], v[20];
	int c,i,r;
	
	g[0] = num;
	g[1] = mod;
	u[0] = 1;
	u[1] = 0;
	v[0] = 0;
	v[1] = 1;

	i = 1;
	
	while(g[i] != 0) {
		c      = g[i-1] / g[i];
		g[i+1] = g[i-1] % g[i];
		u[i+1] = u[i-1] - c*u[i];
		v[i+1] = v[i-1] - c*v[i];
		i++;
	}
	
	while(v[i-1]<0) 
		v[i-1] = v[i-1] + num;
	
    r = v[i-1] % num;
    
    if( (mod * r) % num  == 1)
	    return r;
    return -1;
}


extern char * stripwhite (char *string)
{
  register char *s, *t;

  for (s = string; isspace (*s); s++)
    ;
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && isspace (*t))
    t--;
  *++t = '\0';

  return s;
}

/* some for main.c */
/* generates a random string of 6 chars */
extern char *randomstring(void)
{
	char *ptr = (char *) malloc(6);
	int i = 0;
	
	srand(time(NULL));
	memset( ptr, '\0',6);   
	
	for( i = 0; i < 6; i++ )
		ptr[i] = (char)(25 * (rand()/(RAND_MAX+0.0))) + 'a';
	
	if(ptr==NULL)
		return (char *) NULL;

	return ptr;
}

extern void handsig(int);
extern void handler(int signal_number)
{
	int answer=1;
	char *rand;
	char *dump_file = (char *) malloc(16);

	signal(SIGINT, handler);
	printf("\nBreak>\n");
	do {
		printf("1) Exit inmediatly.(dumping memory)\n");
		printf("2) Continue.\n");
		printf("3) Stop current operation.\n");			
		printf("4) Just exit.\n");
		printf("Break> ");
		scanf("%d",&answer);
		if(answer==1) {
			warnx(":: Dumping memory");
			if( (rand=randomstring()) != NULL) {
				sprintf(dump_file,"dump.%s.lisp",rand);
				dump_objects(dump_file);
				free(rand);
				free(dump_file);
			} else {
				warnx(":: Cannot create dump file");
			}
			exit(-1);
			
		} else if(answer == 2) {
			printf("\n>> ");
		} else if(answer == 3) {
			warnx("*** USER ABORT OPERATION ***");
			handsig(2);
		} else if(answer == 4) {
			exit(-1);
		}
	} while (answer != 1 && answer != 2 && answer != 3 && answer != 4);
	
}

extern void usage(void)
{
	fprintf(stderr, "usage: %s options [file...]\n\n",progname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, " -V    --version\t\tShow version and quit.\n");
	fprintf(stderr, " -N    --no-warnings\t\tDon't displays Warnings.\n");
	fprintf(stderr, " -b    --no-build-in\t\tDon`t load build-in functions.\n");
	fprintf(stderr, " -h    --help\t\t\tDisplay this message.\n");
	fprintf(stderr, " -i    --process\t\tProcess file and continue.\n");
	fprintf(stderr, " -q    --quiet\t\t\tDon`t show logo.\n");
	fprintf(stderr, " -v    --verbose\t\tBe more verbose.\n");
	fprintf(stderr, "\n\n");	
	exit(1);
}

extern void print_logo(void)
{
	printf("\n(let\n\t((NAME 'SLISP)\n\t (VERSION %s)))\n\n", SLISP_VERSION);
}

extern void sort_strings(char *sp[], int n, int how)
{
	int min;
	int i,j;
	char *temp;
	
	if(how == 0) {
		for (i=0;i<(n-1);i++) {
			min = i;				
			for (j=i;j<n;j++)
				if (strcmp(sp[j],sp[min]) < 0)
					min = j;
			temp = sp[i];
			sp[i] = sp[min];
			sp[min] = temp;
		}				
	} else {
		for (i=0;i<(n-1);i++) {
			min = i;				
			for (j=i;j<n;j++)
				if (strcmp(sp[j],sp[min]) > 0)
					min = j;

			temp = sp[i];
			sp[i] = sp[min];
			sp[min] = temp;
		}				
	}
	
}

