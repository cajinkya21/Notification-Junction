/*  
*    Notification-Junction is an interface between multiple applications and multiple Notification Providers.
*    Copyright (C) 2015      Navroop Kaur<watwanichitra@gmail.com>, 
*                            Shivani Marathe<maratheshivani94@gmail.com>, 
*                            Kaveri Sangale<sangale.kaveri9@gmail.com>
*	 All Rights Reserved.
*	
*    This program is free software; you can redistribute it and/or modify it under the terms of the 
*    GNU General Public License as published by the free Software Foundation; either version 3 of the
*    License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
*    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
*    See the GNU General Public License for more details.
*       
*    You should have received a copy of the GNU General Public License along with this program; if not, write to the 
*    free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef _APPDCLL_H
#define _APPDCLL_H	1

/* The file contains code for doubly linked list for applications 
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include<errno.h>
#include "rdwr_lock.h"
#include "uthash.h"

#define ALREXST -1
#define NOTFND -2

/* Structure for saving the arguments from getnotify() */
typedef struct extr_key_val {
	char **key_val_arr;
	struct extr_key_val *next;	
	/*	
	* key_val_arr 	: the pointer to the key-value pair, ending with NULL
	* next 		: the pointer to the next set of key-value pair stored in extr_key_val structure
	*/

} extr_key_val;

/* Structure for npnode */
typedef struct np_node {
	char *name;
	struct np_node *next;
	struct extr_key_val *key_val_ptr;
	/*
	* name 		: to store name of NP
	* next 		: the pointer to the node of same type
	* key_val_ptr 	: a pointer to the list of structures containing pointers to the key-values
	*/
} np_node;

/* Structure for application node */
typedef struct app_node {
	char *data;
	struct app_node *prev;
	struct app_node *next;
	np_node *np_list_head;
	int np_count;
	UT_hash_handle hh;         		/* makes this structure hashable */
	/*
	* data 		: to store name of APP
	* prev 		: a pointer to the previous node of same type
	* next 		: a pointer to the next node of same type
	* np_list_head 	: the pointer to the trailing list of NPs, APP registered with
	* np_count 	: the number of NPs, APP registered with
	*/
} app_node;


/*************************************************/
/* 		List and its functions		 */
/*************************************************/


/* Structure for application list's head */
typedef struct app_dcll {
	app_node *head;
	int count;
	pthread_rdwr_t app_list_lock;
	/*
	* head		: the pointer to the first node in App List
	* count 	: the number of applications registered with NJ
	*/
} app_dcll;

void init_app(app_dcll * l);					/* Initialise the application list */
void print_app(app_dcll * l);					/* Print the list */
void print_np_key_val(app_node * temp);				/* Print the Key-Value strings of each getnotify instance 
								   associated with each registration of the given application */
app_node *search_app(app_dcll * l, char *val);			/* Search the application and 
								 * return a pointer to it's node; 
								 * return NULL if not found */
int search_reg(app_dcll * l, char *appname, char *npname);	/* Search a registration in the Application List, 
								   for the given appname and npname and 
								 * return -1 if found, otherwise return 0 */
np_node *get_reg_list(app_dcll *, char *appname, char *npname);	/* Search a registration in the Application List, 
								   for the given appname and npname and 
								 * return a pointer to the npnode in the trailing list if found, 
								   otherwise return NULL */
int add_app_ref(app_dcll* l, char* app_name, char* np_name);	/* Searches if the app exists, 
								 * If it doesn't, adds the app to the list
								 * Checks the npname argument,
								 * If not NULL, adds the registration,
								 * If app exists,  traverse its trailing np list and if np exists, 
								   return error, otherwise add it */
int del_app_ref(app_dcll* l, struct app_node *, char* np_name);	/* Searches if the app exists, 
 								 * If it doesn't, error
 								 * Checks the npname argument,
 								 * If not NULL, removes the regustration(decr_np_app_cnt(&npList, np_name))
 								   if NULL, removes the app(dec_all_np_counts(&appList, &npList, app_name))
 								 */
/*************************************************/
/*		End of list functions		*/
/*************************************************/



/*************************************************/
/* 		hash and Hash functions 	*/
/*************************************************/

/* Structure for hashing */
typedef struct hash_struct_app {
    	struct app_node *app_hash;
    	pthread_rdwr_t app_hash_lock;
	/*
	* app_hash	: to make it hashable
	* app_hash_lock	: Reader-Writer lock to protect access to hash
	*/

}hash_struct_app;

int add_app_ref_hash(hash_struct_app *hstruct, char* app_name, char* np_name);	
								/* Searches if the app exists, 
								* If it doesn't, adds the app to the hash
								* Checks the npname argument,
								* If not NULL, adds the registration,
								* If app exists,  traverse its trailing np list and if np exists, 
								   return error, otherwise add it 
								*/
int del_app_ref_hash(hash_struct_app *hstruct, char* app_name, char* np_name); 
								/* Searches if the app exists, 
 								 * If it doesn't, error
 								 * Checks the npname argument,
 								 * If not NULL, removes the registration
 								   if NULL, removes the app
 								 */
void print_hash_app(hash_struct_app *hstruct);			/* Prints hash */
np_node *get_reg_hash(hash_struct_app *hstruct, char* appname, char* np_name); 
								/* Search a registration in the Application List, 
								   for the given appname and npname */

/*************************************************/
/* 		End of Hash functions		*/
/*************************************************/

#endif /*app_dcll.h */
