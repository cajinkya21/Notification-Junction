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

/*
* This file contains code for the dcll and hash of np(s) 
*/


#ifndef _NPDCLL_H
#define _NPDCLL_H
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include "rdwr_lock.h"
#include <assert.h>
#include "uthash.h"

/* The node for np(s) */
typedef struct main_np_node {
	char *data;                 //  name of NP
	struct main_np_node *prev;  //  pointer to previous node of same type
	struct main_np_node *next;  //  pointer to next node of same type
	char *usage;                //  key_val pair string
	int app_count;              //  number of APPs registerd with NP
	char **key_val_arr;         //  array of key_value pair ending with NULL
    UT_hash_handle hh;          //   makes this structure hashable 
} main_np_node;


/* List and its functions */
typedef struct np_dcll {
	main_np_node *head;         //  pointer to the first node of NP List
	int count;                  //  number of NPs registered
	pthread_rdwr_t np_list_lock;
} np_dcll;

/* Initialise */
void init_np(np_dcll *l);

/* Add an NP with the given name to the linked list for NPs, return 0 on success */
int add_np(np_dcll *l, char *np_name, char *usage, char ***key_val_arr);

/* print the NP list */
void print_np(np_dcll *l);

/* Search for an np with the given name and return a pointer to that node if found */
main_np_node *search_np(np_dcll *l, char *val);

/*Delete the main_np_node whose pointer is passed as argument*/
int del_np_node(np_dcll *l,main_np_node * np_to_del);

/* Delete the np with the given name, if found */
int del_np(np_dcll * l, char *val);

/* For the np with the given name, if found, find the count of the applications that have registered with it */
int get_np_app_cnt(np_dcll * l, char *nval);

/* For the np with the given name, if found, increment the count of the applications that have registered with it */
void incr_np_app_cnt(np_dcll * l, char *nval);

/* For the np with the given name, if found, decrement the count of the applications that have registered with it */
void decr_np_app_cnt(np_dcll * l, char *nval);

/* Hash and its functions */

typedef struct hash_struct_np {
    struct main_np_node *np_hash;
    pthread_rdwr_t np_hash_lock;
}hash_struct_np;

/* Add the NP to the hash structure */
int add_np_to_hash(hash_struct_np *hstruct, char *val, char *usage, char ***key_val_arr);

/* delete the NP from hash */
int del_np_from_hash(hash_struct_np *hstruct, char *val);

/* print hash */
void print_hash_np(hash_struct_np *hstruct);

/* increment app_count of NP in the hash */
void incr_np_app_cnt_hash(hash_struct_np *hstruct, char *np_name);

/* decrement app_count of NP in the hash */
void decr_np_app_cnt_hash(hash_struct_np *hstruct, char *np_name);

/* get app_count of NP */
int get_np_app_cnt_hash(hash_struct_np *hstruct, char* np_name);

#endif				/*np_dcll.h */
