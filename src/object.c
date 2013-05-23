/*	$Id: object.c,v 1.14 2001/08/10 16:27:04 sandro Exp $	*/

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

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <signal.h>
#include "slisp.h"
#include "extern.h"


/*--------------------------------------------------------------------------
 * Object allocator functions.
 *--------------------------------------------------------------------------*/

objectp nil, t;

static objectp free_objs_list = NULL;
static objectp used_objs_list = NULL;

int MAX_USED=32768; /* maximun number of objects in memory */

int free_objs = 0;
int used_objs = 0;

static char *obj_type(objectp p)
{
	char s[256];
	switch (p->type) {
		case OBJ_NIL:
			return "nil\n";
		case OBJ_T: 
			return "t\n";
			
		case OBJ_INTEGER: 
			return "\t(integer)\n";
			
		case OBJ_IDENTIFIER: 
			strncpy(s,p->value.id,256);
			s[strlen(s)-1] == '\0';
			return s; 
		case OBJ_DOUBLE: 
			return "\t(double)\n";
			
		case OBJ_CSTRING: 
			return "\t(string)\n";
#ifdef HAVE_GMP
		case OBJ_NGMP:
			return "\t(numeric GMP)\n";
#endif
		case OBJ_CONS: 
			return "\t(cons)\n";
			
		default: 
			return "FREED\n";
	}
	
}

void print_list(void)
{
	objectp p;
	printf(":: used objects\n\n");
	
	for (p = used_objs_list; p != NULL; p = p->next)
		printf("%s", obj_type(p));
	
	printf("\n");
	
}
extern void handsig(int);
objectp new_object(int type)
{
	objectp p;
	int MAX_ALLOWED = (MAX_USED/2 + MAX_USED/4);
	if(used_objs >= MAX_ALLOWED) {
	    warnx("*** STACK OVERFLOW ***");
	    handsig(1);
	    return nil;
	}
	if (free_objs_list == NULL) {
		p = (objectp)xmalloc(sizeof(struct object));
#ifdef DEBUG
		warnx(":: allocating cons %p", p);
#endif
	} else {
		p = free_objs_list;
		free_objs_list = free_objs_list->next;
		--free_objs;
#ifdef DEBUG
		warnx(":: recycling cons %p", p);
#endif
	}
	
	p->next = used_objs_list;
	used_objs_list = p;
	
	p->type = type;

	if (type == OBJ_CONS) {
		p->value.c.car = nil;
		p->value.c.cdr = nil;
	}

	p->gc = 0;
		
	++used_objs;
	
	return p;
}

__inline__  objectp search_object_identifier(char *s)
{
	objectp p;
	
	for (p = used_objs_list; p != NULL; p = p->next)
		if (p->type == OBJ_IDENTIFIER && !strcmp(p->value.id, s))
			return p;
	
	return NULL;
}

__inline__  objectp search_object_string(char *s)
{
  objectp p;
  
  for (p = used_objs_list; p != NULL; p = p->next)
	  if (p->type == OBJ_NGMP && !strcmp(p->value.s, s))
		  return p;
  
	 return NULL;
}

__inline__  objectp search_object_cstring(char *s)
{
  objectp p;
  
  for (p = used_objs_list; p != NULL; p = p->next)
	  if (p->type == OBJ_CSTRING && !strcmp(p->value.s, s))
		  return p;
  
	 return NULL;
}

__inline__  objectp search_object_integer(int in)
{
	objectp p;
	
	for (p = used_objs_list; p != NULL; p = p->next)
		if (p->type == OBJ_INTEGER && p->value.i == in)
			return p;
	
	return NULL;
}

__inline__  objectp search_object_double(double dou)
{
	objectp p;
	
	for (p = used_objs_list; p != NULL; p = p->next)
		if (p->type == OBJ_DOUBLE && p->value.d == dou)
			return p;
	
	return NULL;
}

void init_objects(void)
{
	nil = new_object(OBJ_NIL);
	t = new_object(OBJ_T);
}

/*--------------------------------------------------------------------------
 * Object set functions.
 *--------------------------------------------------------------------------*/

typedef struct object_pair *object_pairp;

struct object_pair {
	objectp		 name;
	objectp		 value;
	object_pairp next;
};

static object_pairp setobjs_list = NULL;

void set_object(objectp name, objectp value)
{
	object_pairp p;
	
	if(name->type != OBJ_IDENTIFIER) {
		warnx("warning: object is not an identifier");
		return;
	}
	
	for (p = setobjs_list; p != NULL; p = p->next)
		if (p->name->value.id != NULL &&
			!strcmp(name->value.id, p->name->value.id) ) {
			p->value = value;
			return;
		}
	p = (object_pairp)xmalloc(sizeof(struct object_pair));
	p->next = setobjs_list;
	setobjs_list = p;
	p->name = name;
	p->value = value;
}

objectp get_object(objectp name)
{
	object_pairp p;
	
	for (p = setobjs_list; p != NULL; p = p->next)
		if (p->name->value.id != NULL && !strcmp(name->value.id, p->name->value.id))
			return p->value;
	return nil;
}

void dump_objects(char *fname)
{
	object_pairp p;
	FILE *fout;
	
	if ((fout = fopen(fname, "w")) == NULL)
		err(1, "%s", fname);
	
	for (p = setobjs_list; p != NULL; p = p->next) {
		fprintf(fout, "(setq %s '", p->name->value.id);
		princ_object(fout, p->value);
		fprintf(fout, ")\n");
		if(p->name->doc != NULL) 
		    fprintf(fout,"(make-doc %s \"%s\")\n", p->name->value.id,p->name->doc);

	}
	fclose(fout); 
}

void print_objects(void)
{
	object_pairp p;
	int i,j,hm;
	i=0;	
	for (p = setobjs_list; p != NULL; p = p->next) {
		hm=0;
		hm=printf("(%s)", p->name->value.id);
		hm=20-hm;
		for(j=0;j<hm;j++)
		    printf(" ");
		if(i%4==0)
		    printf("\n");
		i++;
	}
 
}

#ifdef DEBUG

static char *obj_type_str(objectp p)
{
   
	switch (p->type) {

	case OBJ_NIL:
		return "nil";

	case OBJ_T:
		return "t";

	case OBJ_INTEGER:
		return "integer";

	case OBJ_IDENTIFIER:
		return "identifier";

	case OBJ_DOUBLE:
		return "double";

	case OBJ_CSTRING:
		return "string";
#ifdef HAVE_GMP
	case OBJ_NGMP:
		return "numeric (GMP)";
#endif
	case OBJ_CONS:
		return "cons";
	/* we don`t want to reach here */
	default: 
	    assert(0);
	} 
}

void print_obj_lists(void)
{
	objectp p;

	warnx(":: used objects");

	for (p = used_objs_list; p != NULL; p = p->next)
		warnx("::\t%p (%s)", p, obj_type_str(p));

	warnx(":: free objects");

	for (p = free_objs_list; p != NULL; p = p->next)
		warnx("::\t%p (%s)", p, obj_type_str(p));

}

#endif


/*--------------------------------------------------------------------------
 * Poor-man garbage collection functions.
 *--------------------------------------------------------------------------*/

/* The integer used for tagging the Lisp objects. */

static int gc_id = 0;

__inline__  static void tag_tree(objectp p)
{
	if (p->gc == gc_id)
		return;
	
	p->gc = gc_id;
	
	if (p->type == OBJ_CONS) {
		tag_tree(p->value.c.car);
		tag_tree(p->value.c.cdr);
	}
}

__inline__  static void tag_whole_tree(void)
{
	object_pairp p;
	
	for (p = setobjs_list; p != NULL; p = p->next) {
		tag_tree(p->name);
		tag_tree(p->value);
	}
}

static void do_garbage_collect(void)
{
	objectp p, new_used_objs_list = t, next;
	
	tag_whole_tree();
	
		/*
		 * Search in the object vector.
		 */
	for (p = used_objs_list; p != NULL && p != t; p = next) {
		next = p->next;
		if (p->gc != gc_id) {
			/* Remove unreferenced object. */
#ifdef DEBUG
			warnx(":: collecting cons %p", p);
#endif
			switch (p->type) {
				

				case OBJ_CSTRING: case OBJ_NGMP:
					free(p->value.s);
					break;
				case OBJ_IDENTIFIER:
					free(p->value.id);
					break;
				
			}
			
    			p->next = free_objs_list;
			free_objs_list = p;
			
			++free_objs;
			--used_objs;
		} else {
			/* The object is referenced somewhere. */
			p->next = new_used_objs_list;
			new_used_objs_list = p;
		}
	}
	
	used_objs_list = new_used_objs_list;
}

void garbage_collect(void)
{
	if (++gc_id == INT_MAX)
		gc_id = 1;
	do_garbage_collect();
}

