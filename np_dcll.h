/* This file contains code for the dcll of np(s) */
#ifndef _NPDCLL_H
#define _NPDCLL_H	

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#define ALREXST -1
#define NOTFND -2

/* The node for np(s) */

typedef struct main_np_node {
	char* data;
	struct main_np_node *prev;
	struct main_np_node *next;
	char *usage;
	int app_count;
}main_np_node;

/* The node for the dcll */

typedef struct np_dcll {
	main_np_node *head;
	int count;
}np_dcll;

void init_np(np_dcll *);
int add_np(np_dcll *, char*);
void print_np(np_dcll *);
main_np_node* search_np(np_dcll *, char*);
int del_np(np_dcll *, char*);
void incr_np_app_cnt(np_dcll *, char *);
void decr_np_app_cnt(np_dcll *, char *);
int get_np_app_cnt(np_dcll *, char *);



#endif  /*np_dcll.h*/
