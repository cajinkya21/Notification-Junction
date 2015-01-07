#ifndef STDIO_H
#define STDIO_H
#include<stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include<stdlib.h>
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include<unistd.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

#ifndef ALREXST
#define ALREXST -1
#endif

#ifndef NOTFND
#define NOTFND -2
#endif


typedef struct main_np_node {
	char* data;
	struct main_np_node *prev;
	struct main_np_node *next;
	int app_count;
}main_np_node;

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
int get_np_app_cnt(np_dcll *, char *);



