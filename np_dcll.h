/*Notification-Junction is an Interface between multiple Applications and multiple Notification Providers.
Copyright (C) 2015  Navroop Kaur<watwanichitra@gmail.com> , Shivani Marathe<maratheshivani94@gmail.com> , Kaveri Sangale<sangale.kaveri9@gmail.com>
	All Rights Reserved
	
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
   
 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA 
*/

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
	char *data;
	struct main_np_node *prev;
	struct main_np_node *next;
	char *usage;
	int app_count;
	char **key_val_arr;
} main_np_node;

/* The node for the dcll */

typedef struct np_dcll {
	main_np_node *head;
	int count;
} np_dcll;

void init_np(np_dcll *l);
int add_np(np_dcll *l, char *np_name, char *usage, char ***key_val_arr);
void print_np(np_dcll *l);
main_np_node *search_np(np_dcll *l, char *val);
int del_np(np_dcll * l, char *val);
void incr_np_app_cnt(np_dcll * l, char *nval);
void decr_np_app_cnt(np_dcll * l, char *nval);
int get_np_app_cnt(np_dcll * l, char *nval);
int check_key_validity(np_dcll *npList, char *key_to_check);

#endif				/*np_dcll.h */
