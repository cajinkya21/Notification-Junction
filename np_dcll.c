/*Notification-Junction is an Interface between multiple Applications and multiple Notification Providers.
Copyright (C) 2015  Navroop Kaur<watwanichitra@gmail.com> , Shivani Marathe<maratheshivani94@gmail.com> , Kaveri Sangale<sangale.kaveri9@gmail.com>
	All Rights Reserved
	
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
   
 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA 
*/

/*
*	This file contains the code for the dcll implementation of 
*	Initialising,
*	Adding an np,
*	Printing the np list,
*	Searching an np, 
*	Deleting an np,
*	Getting the count of tha applications registered with this particular np, and
*	INcrementing the count of the applications registered with this particular np
*	DEcrementing the count of the applications registered with this particular np
*
*	At the end of this file is the commented code to test the linked list
*
*/

#include "np_dcll.h"

/* Initialise */

void init_np(np_dcll * l)
{

	//printf("NP_DCLL  : In NP Init\n");    
	l->head = NULL;
	l->count = 0;
	//printf("NP_DCLL : Initialization done\n");
}

/* Add an np with the given name*/

int add_np(np_dcll * l, char *val, char *usage, char ***key_val_arr)
{

	main_np_node *new;
	new = search_np(l, val);

	if (new != NULL) {
		//printf("NP_DCLL : Existing NP\n");
		return ALREXST;
	}

	new = (main_np_node *) malloc(sizeof(main_np_node));

	if (new == NULL) {
		perror("NP_DCLL : ERROR IN MALLOC");
		exit(1);
	}
	// WRITE AN INIT FUNCTION FOR A NODE

	new->data = malloc(sizeof(val) + 1);
	new->usage = usage;
	new->key_val_arr = *key_val_arr;

	if (new->data == NULL) {
		perror("NP_DCLL : ERROR IN MALLOC");
		exit(1);
	}

	new->app_count = 0;
	//printf("NP_DCLL  : COUNT in add_main_np_node: %d\n", l->count);

	if (l->count == 0) {
		l->head = new;
		new->prev = new;
		new->next = new;
		strcpy(new->data, val);
		l->count++;
		//printf("NP_DCLL  : COUNT after add: %d\n", l->count);
		return 0;
	}

	else if (l->count == 1) {
		//printf("NP_DCLL  : l head %s\n", l->head->data);
		l->head->prev = new;
		l->head->next = new;
		new->prev = l->head;
		new->next = l->head;
		strcpy(new->data, val);
		//printf("NP_DCLL  : new data %s\n", l->head->next->data);
		l->count++;
		//printf("NP_DCLL  : COUNT after add: %d\n", l->count);
		return 0;
	}

	else {
		l->head->prev->next = new;
		new->prev = l->head->prev;
		new->next = l->head;
		l->head->prev = new;
		strcpy(new->data, val);
		//printf("NP_DCLL  : new data %s\n", l->head->next->data);
		l->count++;
		//printf("NP_DCLL  : COUNT after add: %d\n", l->count);
		return 0;
	}

}

/* Print the list */

void print_np(np_dcll * l)
{

	int i = l->count;

	if (l->count == 0) {
		printf("NP_DCLL  : Nothing to print_np\n");
		return;
	}

	printf("\nTotal number of NPs : %d\n", l->count);
	main_np_node *ptr;
	char **kptr;
	ptr = l->head;
	kptr = ptr->key_val_arr;

	printf("\nNP\t\t\tApp_Count\t\t\tKeyValue");
	printf("\n==================================================\n");

	while (i) {
		printf("%s   ", ptr->data);
		printf("\t\t\t%d\n", ptr->app_count);
		while (*kptr) {
			printf("\t\t\t\t%s\n", *kptr);
			kptr++;
		}
		printf("\n");
		ptr = ptr->next;
		i--;
	}

	printf("\nKey-Value ");

	//printf("NP_DCLL  : \n");
}

/* Search for an np with the given name and return a pointer to that node if found */

main_np_node *search_np(np_dcll * l, char *val)
{

	int i = l->count;
	int found = 0;
	main_np_node *ptr;

	if (l->count == 0) {
		return NULL;
	}

	ptr = l->head;

	while (i) {

		if (strcmp(ptr->data, val) == 0) {
			found = 1;
			break;
		}

		i--;
		ptr = ptr->next;
	}

	if (found == 0) {
		//printf("NP_DCLL  : Not found %s\n", val);
		return NULL;
	}

	else
		return ptr;
}

/* Delete the np with the given name, if found */

int del_np(np_dcll * l, char *val)
{

	main_np_node *p, *temp;
	temp = search_np(l, val);
	//printf("NP_DCLL  : Deleting THIS VALUE - %s\n\n", temp->data);

	if (temp == NULL) {
		//printf("NP_DCLL  : Not found %s\n", val);
		return NOTFND;
	}

	if (l->count == 1) {
		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
		free(temp);
		l->count--;
		return 0;
	}

	else if (temp == l->head) {
		//printf("NP_DCLL  : Asked to delete HEAD\n");
		l->head = temp->next;
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
		free(temp);
		l->count--;
		return 0;
	}

	else {
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
		free(temp);
		l->count--;
		return 0;
	}
}

/* For the np with the given name, if found, find the count of the applications that have registered with it */

int get_np_app_cnt(np_dcll * l, char *nval)
{
	main_np_node *temp;
	temp = search_np(l, nval);
	if (temp == NULL)
		return NOTFND;
	return temp->app_count;
}

/* For the np with the given name, if found, increment the count of the applications that have registered with it */

void incr_np_app_cnt(np_dcll * l, char *nval)
{

	main_np_node *temp;
	temp = search_np(l, nval);

	if (temp == NULL)
		return;

	temp->app_count++;
	//printf("NP_DCLL  : App_cnt of %s = %d\n",nval, temp->app_count);
}

void decr_np_app_cnt(np_dcll * l, char *nval)
{

	main_np_node *temp;
	temp = search_np(l, nval);

	if (temp == NULL)
		return;

	temp->app_count--;
	printf("\t%d\n", temp->app_count);
	//printf("NP_DCLL  : App_cnt of %s = %d\n",nval, temp->app_count);
}

/*
* This is the code for testing the list
*/

/*

int main() {

	np_dcll l;
	char val[25];
	init_np(&l);
	int i;
	int ch;
	while(1) {
		//printf("NP_DCLL  : Enter Choice :\n0 - Print\n1 - Add NP\n2 - Search NP\n3 - Delete NP\n4 - Increment app_val\n5 - Get app_val\n\n");
		scanf("%d", &ch);
		switch(ch) {
	
			case 0 :	print_np(&l);
					break;
			case 1 :	//printf("NP_DCLL  : Enter Val :\n");
					scanf(" %s", val);
					add_np(&l, val);
					print_np(&l);
					break;
			case 2 : 	//printf("NP_DCLL  : Enter Val :\n");
					scanf(" %s", val);
					search_np(&l, val);
					break;
			case 3 :	//printf("NP_DCLL  : Enter Val :\n");
					scanf(" %s", val);
					del_np(&l, val);
					print_np(&l);
					break;
			case 4 :	//printf("NP_DCLL  : Enter Val :\n");
					scanf(" %s", val);
					incr_np_app_cnt(&l, val);
					break;
			case 5 :	//printf("NP_DCLL  : Enter Val :\n");
					scanf(" %s", val);
					i = get_np_app_cnt(&l, val);
					//printf("NP_DCLL  : app_cnt:%d\n", i);
					break;
		}
	}
	return 0;

}

*/
