/*  
    Notification-Junction is an interface between multiple applications and multiple Notification Providers.
    Copyright (C) 2015      Navroop Kaur<watwanichitra@gmail.com>, 
                            Shivani Marathe<maratheshivani94@gmail.com>, 
                            Kaveri Sangale<sangale.kaveri9@gmail.com>
	All Rights Reserved.
	
    This program is free software; you can redistribute it and/or modify it under the terms of the 
    GNU General Public License as published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the GNU General Public License for more details.
       
    You should have received a copy of the GNU General Public License along with this program; if not, write to the 
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

/* The file contains code for doubly linked list for applications */
#ifndef _APPDCLL_H
#define _APPDCLL_H	1

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#define ALREXST -1
#define NOTFND -2

/* GETNOTIFY ARGUMENT STRUCTURE */

typedef struct extr_key_val {
	char **key_val_arr;
	struct extr_key_val *next;
} extr_key_val;

/* NP NODE STRUCTURE */

typedef struct np_node {
	char *name;
	struct np_node *next;
	char *arguments;
	struct extr_key_val *key_val_ptr;	/* The pointer to the list of structures containing pointers to the key-values */
} np_node;

/* APPLICATION NODE STRUCTURE */

typedef struct app_node {
	char *data;
	struct app_node *prev;
	struct app_node *next;
	np_node *np_list_head;
	int np_count;
} app_node;

/* APPLICATION LIST STRUCTURE */

typedef struct app_dcll {
	app_node *head;
	int count;
} app_dcll;

void init_app(app_dcll * l);
int addapp_node(app_dcll * l, char *val);
void print_app(app_dcll * l);
app_node *search_app(app_dcll * l, char *val);
int searchReg(app_dcll * l, char *appname, char *npname);
np_node *getReg(app_dcll *, char *appname, char *npname);
int del_app(app_dcll * l, char *val);
int add_np_to_app(app_dcll * l, char *aval, char *nval);
int del_np_from_app(app_dcll * l, char *aval, char *nval);
void printNpKeyVal(app_node * temp);

#endif				/*app_dcll.h */
