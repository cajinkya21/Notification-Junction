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

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#define ALREXST -1
#define NOTFND -2

/* The node for np(s) */

typedef struct main_np_node {
	char *data;
	struct main_np_node *prev;
	struct main_np_node *next;
	char *usage;
	int app_count;
	char **key_val_arr;
	/* 
	* data is to store name of NP
	* prev is a pointer to previous node of same type
	* next is a pointer to previous node of same type
	* usage is key_val pair string
	* app_count is number od APPs registerd with NP
	* key_val_arr is array of key_value pair ending with NULL
	*/
} main_np_node;

/* The node for the dcll */

typedef struct np_dcll {
	main_np_node *head;
	int count;
	/*
	* head is the pointer to the first node of NP List
	* count is the number of NPs registered
	*/
} np_dcll;

void initNp(np_dcll *l);
int addNp(np_dcll *l, char *np_name, char *usage, char ***key_val_arr);
void printNp(np_dcll *l);
main_np_node *searchNp(np_dcll *l, char *val);
int delNp(np_dcll * l, char *val);
void incrNpAppCnt(np_dcll * l, char *nval);
void decrNpAppCnt(np_dcll * l, char *nval);
int getNpAppCnt(np_dcll * l, char *nval);
int checkKeyValidity(np_dcll *npList, char *key_to_check);

#endif				/*np_dcll.h */
