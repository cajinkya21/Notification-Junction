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
 *	This file contains the code for the dcll implementation of 
 *	Initialising,
 *	Adding an np,
 *	Printing the np list,
 *	Searching an np, 
 *	Deleting an np,
 *	Getting the count of the applications registered with this particular np, and
 *	Incrementing the count of the applications registered with this particular np
 *	Decrementing the count of the applications registered with this particular np
 *
 *	At the end of this file is the commented code to test the linked list
 *
 */

#include "np_dcll.h"

/* Initialise */

void init_np(np_dcll * l)
{
	l->head = NULL;
	l->count = 0;
}

/* Add an np with the given name*/

int add_np(np_dcll * l, char *val, char *usage, char ***key_val_arr)
{

	main_np_node *new, *tmp;

	tmp = search_np(l, val);

	if (tmp != NULL) {
/*		printf("> %s %d add_np() :NP_DCLL : Existing NP\n", __FILE__, __LINE__);
		new->data = (char *)malloc(sizeof(val) + 1);
		new->usage = usage;
		new->key_val_arr = *key_val_arr;
*/
		errno = EEXIST;
		return ALREXST;
	}

	new = (main_np_node *) malloc(sizeof(main_np_node));

	if (new == NULL) {
		errno = ECANCELED;
		perror("NP_DCLL : ERROR IN MALLOC");
		exit(1);
	}

	new->data = (char *)malloc((strlen(val) + 1) * sizeof(char));
	new->usage = usage;
	new->key_val_arr = *key_val_arr;

	if (new->data == NULL) {
		errno = ECANCELED;
		perror("NP_DCLL : ERROR IN MALLOC");
		exit(1);
	}

	new->app_count = 0;

	if (l->count == 0) {
		l->head = new;
		new->prev = new;
		new->next = new;
		strcpy(new->data, val);
		l->count++;
		return 0;
	}

	else if (l->count == 1) {
		l->head->prev = new;
		l->head->next = new;
		new->prev = l->head;
		new->next = l->head;
		strcpy(new->data, val);
		l->count++;
		return 0;
	}

	else {
		l->head->prev->next = new;
		new->prev = l->head->prev;
		new->next = l->head;
		l->head->prev = new;
		strcpy(new->data, val);
		l->count++;
		return 0;
	}

}

/* Print the list */

void print_np(np_dcll * l)
{

	int i = l->count;
	main_np_node *ptr;
	char **kptr;

	if (l->count == 0) {
		printf("> %s %d print_np() : NP_DCLL  : Nothing to print_np\n", __FILE__, __LINE__);
		return;
	}

	printf("\n> %s %d print_np() : Total number of NPs : %d\n", __FILE__, __LINE__, l->count);

	ptr = l->head;
	kptr = ptr->key_val_arr;

	printf("\n> %s %d print_np() : NP\t\t\tApp_Count\t\t\tKeyValue", __FILE__, __LINE__);
	printf("\n==================================================\n");

	while (i) {
		printf("> %s %d print_np() : %s   ", __FILE__, __LINE__, ptr->data);
		printf("\t\t\t%d\n", ptr->app_count);

		while (*kptr) {
			printf("\t\t\t\t%s\n", *kptr);
			kptr++;
		}

		printf("\n");
		ptr = ptr->next;
		i--;
	}
	printf("====================================xx======================================\n");
}

/* Search for an np with the given name and return a pointer to that node if found */

main_np_node *search_np(np_dcll * l, char *val)
{

	int i = l->count;
	int found = 0;
	main_np_node *ptr;

	if (l->count == 0) {
		errno = ENODEV;
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
		errno = ENODEV;
		return NULL;
	}

	else {
		errno = EEXIST;
		return ptr;
	}
}

/* Delete the np with the given name, if found */

int del_np(np_dcll * l, char *val)
{
	char **np_key_val_arr;
	int i = 0;
	main_np_node *p, *temp, *q;
	temp = search_np(l, val);
	if (temp == NULL) {
		errno = ENODEV;
		return NOTFND;
	}

	else if (temp == l->head && l->count == 1) {

		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
	}

	else if (temp == l->head && l->count > 1) {

		l->head = temp->next;
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
	} 
	else {
		p = temp->prev;
		q = temp->next;
		p->next = q;
		q->prev = p;
	}

	np_key_val_arr = temp->key_val_arr;
	i = 0;
	while(np_key_val_arr[i] != NULL) {
		free(np_key_val_arr[i]);
		np_key_val_arr[i] = NULL;
		i++;
	}

	free(temp->usage);
	temp->usage = NULL;
	free(temp->data);
	temp->data = NULL;
	free(temp);
	temp = NULL;
	l->count--;
	return 0;

}

/* For the np with the given name, if found, find the count of the applications that have registered with it */

int get_np_app_cnt(np_dcll * l, char *nval)
{
	main_np_node *temp;

	temp = search_np(l, nval);
	if (temp == NULL) {
		errno = ENODEV;	
		return NOTFND;
	}
	return temp->app_count;
}

/* For the np with the given name, if found, increment the count of the applications that have registered with it */

void incr_np_app_cnt(np_dcll * l, char *nval)
{

	main_np_node *temp;

	temp = search_np(l, nval);

	if (temp == NULL) {
		errno = ENODEV;	
		return;
	}

	temp->app_count++;
}

void decr_np_app_cnt(np_dcll * l, char *nval)
{

	main_np_node *temp;
	temp = search_np(l, nval);

	if (temp == NULL) {
		errno = ENODEV;	
		return;
	}

	temp->app_count--;
	printf("> %s %d decr_np_app_cnt() :\t%d\n", __FILE__, __LINE__, temp->app_count);
}
/*
void empty_np_list(np_dcll * l) {
	printf("> %s %d empty_np_list : starting Np list count is %d ",__FILE__, __LINE__, l->count);

	struct main_np_node *temp;

	temp = l->head;

	assert(l);
	while(temp != NULL) {
		del_np( l, temp->data);
		temp = temp->next;
	}
	assert(l);
	printf("> %s %d empty_np_list : Np list count is %d ",__FILE__, __LINE__, l->count);
	return ;
}

*/

/*
 * This is the code for testing the list
 */

/*void extractKeyVal(char *usage, char ***keyVal)
  {
  int cnt = 0, i = 0;
  char copyusage[512];
  char *ptr;

  printf("NJ : EXTRACTVAL : usage is %s\n", usage);

  strcpy(copyusage, usage);
  cnt = 4;					 
  printf("EXTRACT KEYVAL : NJ : The count is %d\n", cnt);

 *keyVal = (char **)malloc((cnt + 1) * sizeof(char *));
 ptr = strtok(copyusage, "##");

 printf("EXTRACT : NJ : PTR[%d] is %s\n", i, ptr);
 (*keyVal)[i] = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));

 printf("Before strcpy\n");
 strcpy((*keyVal)[i], ptr);
 printf("After strcpy\n");

 for (i = 1; i < cnt; i++) {
 ptr = strtok(NULL, "##");
 if (!ptr) {
 break;
 }
 printf("EXTRACT : NJ : PTR[%d] is %s\n", i, ptr);
 if (!((*keyVal)[i] = (char *)malloc(sizeof(char) * 128)))
 perror("MALLOC IS THE CULPRIT");
 printf("Before strcpy\n");
 strcpy((*keyVal)[i], ptr);
 printf("After strcpy\n");
 }
 (*keyVal)[i] = NULL;
 return;
 }

 int main() {
 char **keyVal;
 np_dcll l;
 char val[25];
 initNp(&l);
 int i;
 int ch;
 char *usage = (char *)malloc(sizeof(char) * 128);
 while(1) {
 printf("NP_DCLL  : Enter Choice :\n0 - Print\n1 - Add NP\n2 - Search NP\n3 - Delete NP\n4 - Increment app_val\n5 - Get app_val\n\n");
 scanf("%d", &ch);
 switch(ch) {

 case 0 :	print_np(&l);
 break;
 case 1 :	printf("NP_DCLL  : Enter Val :\n");
 scanf(" %s", val);
 printf("Enter key val string:\n");
 scanf("%s", usage);
 extractKeyVal(usage, &keyVal);
 add_np(&l, val, usage, &keyVal);
 printNp(&l);
 break;
 case 2 : 	//printf("NP_DCLL  : Enter Val :\n");
 scanf(" %s", val);
 search_np(&l, val);
 break;
 case 3 :	//printf("NP_DCLL  : Enter Val :\n");
 scanf(" %s", val);
 del_np(&l, val);
 printNp(&l);
 break;
 case 4 :	//printf("NP_DCLL  : Enter Val :\n");
scanf(" %s", val);
incr_np_app_cnt(&l, val);
break;
case 5 :	//printf("NP_DCLL  : Enter Val :\n");
scanf(" %s", val);
i = getNpAppCnt(&l, val);
printf("NP_DCLL  : app_cnt:%d\n", i);
break;
case 6 : 	
free(usage);
exit(0);
}
}
return 0;

}
*/
