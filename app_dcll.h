/* The file contains code for doubly linked list for applications */
#ifndef _APPDCLL_H
#define _APPDCLL_H	1	

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#define ALREXST -1
#define NOTFND -2

/* NP NODE STRUCTURE */
typedef struct np_node {
	char *name;
	struct np_node *next;
}np_node;

/* APPLICATION NODE STRUCTURE */
typedef struct app_node {
	char *data;
	struct app_node *prev;
	struct app_node *next;
	np_node *np_list_head;
	int np_count;
}app_node;

/* APPLICATION LIST STRUCTURE */
typedef struct app_dcll {
	app_node *head;
	int count;
}app_dcll;

void init_app(app_dcll *);
int addapp_node(app_dcll *, char*);
void print_app(app_dcll *);
app_node* search_app(app_dcll *, char*);
int searchReg(app_dcll *, char*, char*);
int del_app(app_dcll *, char*);
int add_np_to_app(app_dcll *, char*, char*);
int del_np_from_app(app_dcll *, char*, char*);

#endif /*app_dcll.h*/
