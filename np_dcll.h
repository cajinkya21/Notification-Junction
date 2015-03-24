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
* This file contains code for the dcll of np(s) 
*/


#ifndef _NPDCLL_H
#define _NPDCLL_H
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include "rdwr_lock.h"
#define ALREXST -1
#define NOTFND -2
#include <assert.h>
#include "uthash.h"

/* The node for np(s) */
typedef struct main_np_node {
	char *data;
	struct main_np_node *prev;
	struct main_np_node *next;
	char *usage;
	int app_count;
	char **key_val_arr;
    	UT_hash_handle hh;         /* makes this structure hashable */
	/* 
	* data is to store name of NP
	* prev is a pointer to previous node of same type
	* next is a pointer to previous node of same type
	* usage is key_val pair string
	* app_count is number od APPs registerd with NP
	* key_val_arr is array of key_value pair ending with NULL
	*/
} main_np_node;

/*************************************************/
/* 		List and its functions		 */
typedef struct np_dcll {
	main_np_node *head;
	int count;
	pthread_rdwr_t np_list_lock;
	/*
	* head is the pointer to the first node of NP List
	* count is the number of NPs registered
	*/
} np_dcll;

void init_np(np_dcll *l);
int add_np(np_dcll *l, char *np_name, char *usage, char ***key_val_arr);
void print_np(np_dcll *l);
main_np_node *search_np(np_dcll *l, char *val);
int del_np(np_dcll * l, char *val);
void incr_np_app_cnt(np_dcll * l, char *nval);
void decr_np_app_cnt(np_dcll * l, char *nval);
int get_np_app_cnt(np_dcll * l, char *nval);
int del_np_node(np_dcll *l,main_np_node * np_to_del);
//void empty_np_list(np_dcll * l);

/*		End of list functions		*/
/*************************************************/




/*************************************************/
/* 		hash and Hash functions 	*/

typedef struct hash_struct_np {
    struct main_np_node *np_hash;
    pthread_rdwr_t np_hash_lock;
}hash_struct_np;


void add_np_to_hash(hash_struct_np *hstruct, char *val, char *usage, char ***key_val_arr);
void del_np_from_hash(hash_struct_np *hstruct, char *val);
void print_hash_np(hash_struct_np *hstruct);
void incr_np_app_cnt_hash(hash_struct_np *hstruct, char *np_name);
void decr_np_app_cnt_hash(hash_struct_np *hstruct, char *np_name);
int get_np_app_cnt_hash(hash_struct_np *hstruct, char* np_name);


/* 		End of Hash functions		*/
/*************************************************/


#endif				/*np_dcll.h */
