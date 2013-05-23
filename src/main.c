/*	$Id: main.c,v 1.14 2001/08/10 15:18:08 sandro Exp $	*/

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
 * Changes by HLG
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>	
#include <errno.h>
#include <time.h>
#include <err.h>
#include <getopt.h>
#include <signal.h>
#include <setjmp.h>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "slisp.h"
#include "extern.h"
#include "version.h"
/* global variables */


int done;
int opt_lint = 1;
int opt_verbose = 0;

extern int used_objs;

jmp_buf env;
static void process_input(void);
extern char *randomstring(void);
extern void usage(void);
extern void print_logo(void);
/* required by -ansi :S */

extern int mkstemp(char *);
extern int kill(pid_t, int);
extern void _rl_enable_paren_matching(int);
extern char *progname;

extern void handsig(int sig)
{
    longjmp(env,sig);
}

#ifdef HAVE_READLINE
char *buffer; /* Contains the buffer to be parsed */

static  char *line_read = (char *) NULL; /* need this! */



/* yeah, very very stupid function */

__inline__ int getchl(void)
{
    int c;
    c=(int) *buffer;
    
    if(c == '\0')
	    return EOF;
    
    buffer++;
    return c;
}

/* Well we have to check  if the buffer has the same number of opened
   parenthesis as closed ones. Unless it returns 1, the buffer is not
   completed, so not evaluaded
*/

int donep(char *s)
{
	int op,cp; /* op: opened parenthesis, cp: closed parenthesis */
	int i;
	int len;
	op=cp=0;
	len=strlen(s);
	
	for(i=0;i<=len;i++) {
		if(s[i] == '(')
			op++;
		else if(s[i] == ')')
			cp++;
	}
	if(op > cp) 
		return 0;
	else 
		return 1;
}


__inline__ char *rl_gets ()
{
    if (line_read) {
	    free(line_read);
		line_read = (char *)NULL;
    }
    line_read = readline (">> ");
    return (line_read);
}

void initialize_readline (void)
{
	rl_readline_name = "";
	rl_bind_key('\t',rl_insert);
	rl_bind_key('\n',rl_insert);
	_rl_enable_paren_matching(1);
	
	rl_set_paren_blink_timeout(750000);
}

static void process_input(void)
{
	objectp p, p1;
	char *next;
	int onatty = input_file == stdin && isatty(STDIN_FILENO);
	buffer=(char *) calloc(1024,sizeof(char));

	initialize_readline();
	init_lex();

	if(setjmp(env) == 0) {
	  LOOP:
		for (;;) {
			if (onatty) {
					/* if thereis something to read ...*/
				while(!buffer || !*buffer) {
					buffer=rl_gets();
					
					while(donep(buffer) == 0) { /* if its not done */
					    next=readline("   ");/* read more */
					    strcat(buffer," ");/* put a space between lines */
					    strcat(buffer,next); /* append to the buffer */
					}
				}
				/* you think is a redundancy, but if rl_replace_line() */
				/* fails, it will cause a segv */
				if (buffer && *buffer) { 
					add_history (buffer);
				}
				
			}
			if ( (p = parse_object(0)) == NULL) {
				if( input_file == stdin) {
					goto LOOP;		
				} else 
					break;
			}
			
			if (opt_verbose) {
				printf("<= ");
				princ_object(stdout, p);
				printf("\n");
			}
			
			p1 = eval(p);
			if (onatty || opt_verbose) {
				printf("=> ");
				princ_object(stdout, p1);
				printf("\n");
			}
			if(!onatty) {
				
#ifdef DEBUG
				warnx(":: objects before collection");
				print_obj_lists();
#endif
			}
			garbage_collect();

			if(!onatty) {
#ifdef DEBUG
				warnx(":: objects after collection");
				print_obj_lists();
#endif
			}
			
		} /* end for */
	} else {
		buffer="t\n";
		goto LOOP;
	}
	done_lex();
}
/* don`t have readline */

#else

static void process_input(void)
{
	objectp p, p1;
	int onatty = input_file == stdin && isatty(STDIN_FILENO);
	
	init_lex();

	for (;;) {
		if (onatty) {
			printf(" >> ");
			fflush(stdout);
		}
		
		if ((p = parse_object(0)) == NULL)
			break;
		
		if (opt_verbose) {
			printf("<= ");
			princ_object(stdout, p);
			printf("\n");
		}
		
		p1 = eval(p);
		
		if (onatty || opt_verbose) {
			printf("=> ");
			princ_object(stdout, p1);
			printf("\n");
		}
		
#ifdef DEBUG
		warnx(":: objects before collection");
		print_obj_lists();
#endif
		garbage_collect();
#ifdef DEBUG
		warnx(":: objects after collection");
		print_obj_lists();
#endif
	}
	
	done_lex();
}
/* endif of HAVE_READLINE */
#endif 

int process_file(char *filename)
{
	if (filename != NULL && strcmp(filename, "-") != 0  && strcmp(filename, "--") != 0) {
		if ((input_file = fopen(filename, "r")) == NULL)
			return -1;
	} else
		input_file = stdin;
    
	process_input();
	if (input_file != stdin) 
		fclose(input_file);
	return 0;
}

int pro_file(char *filename)
{
	if (filename != NULL) {
		if ((input_file = fopen(filename, "r")) == NULL) {
			warn("%s",filename);
			return -1;
		} else {
			printf(";; Loading file %s\n",filename);
		}
	}
	return 0;
}


int main(int argc, char **argv)
{
	int next_option;
	int fd;
	char buildf[]="/tmp/slisp.XXXXXX";
	char *file;
	FILE *init_file;
	
	int no_init = 0;
	int p_only = 1;
	int display_ascii = 1;
	int no_build = 0;
	
	const char* const short_options = "hqVNiBbvpx:";
	const struct option long_options[] = {
		{ "help",     0, NULL, 'h' },
		{ "quiet",    0, NULL, 'q' },
		{ "version",  0, NULL, 'V' },
		{ "no-warnings", 0, NULL, 'N' },
		{ "process" , 0, NULL, 'i' },
		{ "no-build-in", 0, NULL, 'b' },
		{ "no-init-file",0, NULL, 'p' },
		{ "verbose",  0, NULL, 'v' },
		{ "exec", 1, NULL, 'x' },
		{ NULL,       0, NULL, 0   } 
	};
	signal(SIGUSR1,handsig);
	signal(SIGINT,handler);

	do {
		next_option = getopt_long (argc, argv, short_options,
					   long_options, NULL);
		switch (next_option)
		{
		case 'h':
			usage ();
		case 'q':
			display_ascii = 0;
			break;
		case 'V':
			fprintf(stderr,"%s\n",SLISP_VERSION);
			exit(0);
			break;
		case 'v':
			opt_verbose = 1;
			break;
		case 'N':   
			opt_lint = 0;
			break;
		case 'i':
			p_only = 0;
			display_ascii = 1;
			break;
		case 'b':
			no_build = 1;
			break;
		case 'p':
			no_init = 1;
			break;			
		case 'x':
			p_only = 1;
			display_ascii = 0;
			opt_lint = 1;
			no_build = 1;
			opt_verbose = 1;
			if((fd=mkstemp(buildf)) > 0) {
			    if( write(fd,optarg,strlen(optarg)) == -1) {
				fprintf(stderr,"error evaluating %s\n",optarg);
				unlink(buildf); 
				return -2;
			    }
				process_file(buildf);
				unlink(buildf);
			} else {
				fprintf(stderr,"error evaluating %s\n",optarg);
				return -2;
			}
			return 1;
			break;	
		case -1:
			break;
			
		default: 
			exit(-2);
		}
	}
	while (next_option != -1);
	
	argc -= optind;
	argv += optind;
	
	if(display_ascii == 1)
		print_logo();
	
	init_objects();
	if(no_init == 0) {
		file = (char *) xcalloc(521,sizeof(char));
		/* this is for avoid possibly exploits. It's common to exploit 
		   programs that read environment variables setting values with 
		   shellcodes hehe :P
		*/
		if(strlen(getenv("HOME")) >= 128) {
			warnx("your home path is just too long. :P");
			file = (char *)NULL;
		} else 
			file = getenv("HOME");
		if(file != NULL) {
			strcat(file,"/.slisprc");
			init_file=fopen(file,"r");
			if(init_file!=NULL) {
				fclose(init_file);
				process_file(file);
			}
		}
	}
	if(no_build == 0) {
		if((fd=mkstemp(buildf)) > 0) {
			if( write_m(fd) != 0) {
				unlink(buildf);
				warnx("%s",write_m);
			}
			process_file(buildf);
			unlink(buildf);
		} else 
			warnx("cannot load build-in functions\n");
		
	}
	if (argc < 1) 
		process_file(NULL);
	else {
		
		while (*argv) {
			if(process_file(*argv) != 0) {
				warn("%s",*argv);
			} else {
				printf(";; Loaded file %s\n",*argv);
			}
			argv++;
		}
	}
	if(p_only == 0) 
		process_file(NULL);
	return 0;
}

