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
	/*	
	* key_val_arr is the pointer to the key-value pair, ending with NULL
	* next is the pointer to the next set of key-value pair stored in extr_key_val structure
	*/
} extr_key_val;

/* NP NODE STRUCTURE */

typedef struct np_node {
	char *name;
	struct np_node *next;
	struct extr_key_val *key_val_ptr;
	/*
	* name is to store name of NP
	* next is the pointer to the node of same type
	* key_val_ptr is a pointer to the list of structures containing pointers to the key-values
	*/
} np_node;

/* APPLICATION NODE STRUCTURE */

typedef struct app_node {
	char *data;
	struct app_node *prev;
	struct app_node *next;
	np_node *np_list_head;
	int np_count;
	/*
	* data is to store name of APP
	* prev is a pointer to the previous node of same type
	* next is a pointer to the next node of same type
	* np_list_head is the pointer to the trailing list of NPs, APP registered with
	* np_count is the number of NPs, APP registered with
	*/
} app_node;

/* APPLICATION LIST STRUCTURE */

typedef struct app_dcll {
	app_node *head;
	int count;
	/*
	* head is the pointer to the first node in App List
	* count is the number of applications registered with NJ
	*/
} app_dcll;

void initApp(app_dcll * l);
int addAppNode(app_dcll * l, char *val);
void printApp(app_dcll * l);
app_node *searchApp(app_dcll * l, char *val);
int searchReg(app_dcll * l, char *appname, char *npname);
np_node *getReg(app_dcll *, char *appname, char *npname);
int delApp(app_dcll * l, char *val);
int addNpToApp(app_dcll * l, char *aval, char *nval);
int delNpFromApp(app_dcll * l, char *aval, char *nval);
void printNpKeyVal(app_node * temp);

#endif				/*app_dcll.h */
