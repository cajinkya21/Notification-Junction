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


typedef struct np_node {
	char *name;
	struct np_node *next;
}np_node;

typedef struct app_node {
	char *data;
	struct app_node *prev;
	struct app_node *next;
	np_node *np_list_head;
	int np_count;
}app_node;

typedef struct app_dcll {
	app_node *head;
	int count;
}app_dcll;

void init_app(app_dcll *);
int addapp_node(app_dcll *, char*);
void print_app(app_dcll *);
app_node* search_app(app_dcll *, char*);
int del_app(app_dcll *, char*);
int add_np_to_app(app_dcll *, char*, char*);
int del_np_from_app(app_dcll *, char*, char*);

