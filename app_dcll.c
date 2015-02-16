/*  
    Notification-Junction is an interface between multiple applications and multiple Notification Providers.
    Copyright (C) 2015      Navroop Kaur<watwanichitra@gmail.com>, 
                            Shivani Marathe<maratheshivani94@gmail.com>, 
                            Kaveri Sangale<sangale.kaveri9@gmail.com>
	All Rights Reserved.
	
    This program is free software; you can redistribute it and/or modify it under the terms of the 
    GNU General Public License as published by the free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the GNU General Public License for more details.
       
    You should have received a copy of the GNU General Public License along with this program; if not, write to the 
    free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

/* 
*	The file contains the code for the dcll implementation with trailing singly linked lists for
*	Initializing app_list, 
*	Adding a node to app_list, 
*	Printing app_list, 
*	Searching app_node in app_list, 
*	Deleting a node from app_list, 
*	Adding a np to app_node, 
*	Deleting a np from app_node
*
*
*	At the end of this file is the commented code to test the linked list
*/

#include "app_dcll.h"

/* Initialise the application list */

void init_app(app_dcll * l)
{
	l->head = NULL;
	l->count = 0;
}

/* Append a new application "val" to the list */

int add_app_node(app_dcll * l, char *val)
{
	
	app_node *new, *tmp;

/* Search if the application exists; if it doesn't, return error ALREXST */	
	tmp = search_app(l, val);	
	if (tmp != NULL)	{
		errno = EEXIST;
		return -1;
	}
/* Create and set values for the new node */
	new = (app_node *) malloc(sizeof(app_node));

	if (new == NULL) {
		errno = ECANCELED;
		perror("> app_dcll.c add_app_node() : ERROR IN MALLOC");
		exit(1);
	}

	new->data =(char *)malloc((strlen(val) + 1) * sizeof(char));

	if (new->data == NULL) {
		errno = ECANCELED;
		perror("> app_dcll.c add_app_node() : ERROR IN MALLOC");
		exit(1);
	}

	new->np_count = 0;

	/* When there is no application in the list */
	if (l->count == 0) {
		l->head = new;
		new->prev = new;
		new->next = new;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		l->count++;
		return 0;
	}

	/* When there is only one existing application in the list */
	else if (l->count == 1) {
		l->head->prev = new;
		l->head->next = new;
		new->prev = l->head;
		new->next = l->head;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		l->count++;
		return 0;
	}

	/* More than two applications in the list */
	else {
		l->head->prev->next = new;
		new->prev = l->head->prev;
		new->next = l->head;
		l->head->prev = new;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		l->count++;
		return 0;
	}
}

/* Print the list */
void print_app(app_dcll * l)
{

	int i = l->count;
	app_node *ptr;
	np_node *np;
	
	if (l->count == 0) {
		printf("> app_dcll.c print_app() : NO APPS TO PRINT\n");
		return;
	}
	
	ptr = l->head;
	
	printf("\n> %s %d print_app() : Total number of applications : %d\n",__FILE__, __LINE__, l->count);
	printf("\n> %s %d print_app() :App\t\t\tNp_Count", __FILE__, __LINE__);
	printf("\n===================================\n");
	
/* While applications in the list are being traversed, print the contents of each application node */
	while (i) {		
		printf("%s\t\t\t", ptr->data);
		printf("%d\n", ptr->np_count);
		print_np_key_val(ptr);
		printf("\n\n\n");
		ptr = ptr->next;
		i--;
	}

	/* Print the registrations of each application */


	i = l->count;
	ptr = l->head;
	if (i != 0) {
		printf("\n> %s %d print_app() :App\t\t\tRegistered with", __FILE__, __LINE__);
		printf("\n===================================\n");
	}

	/* While each application is being traversed */

	while (i) {
		np = ptr->np_list_head;
		printf("\n%s\t\t\t", ptr->data);
		
		/* While it's trailing np list is being traversed */

		while (np != NULL) {
			printf("%s\n", np->name);
			np = np->next;
			printf("\t\t\t");
		}

		ptr = ptr->next;
		i--;
	}

}

/* Print the Key-Value strings of each getnotify instance associated with each registration of the given application */

void print_np_key_val(app_node * temp)
{

	np_node *head = temp->np_list_head;
	extr_key_val *vptr;
	char **kptr;
	

	if (!head) {
		printf("\n> %s %d print_np_key_val() : No head in temp list\n\n", __FILE__, __LINE__);
	}

	printf("> %s %d print_np_key_val() : HEAD DATA = %s\n", __FILE__, __LINE__, head->name);
	
	while (head) {
		vptr = head->key_val_ptr;
		
		if (vptr == NULL) {
			printf("> %s %d print_np_key_val() : VPTR IS NULL\n", __FILE__, __LINE__);
			head = head->next;
			continue;
		}
		
		while (vptr) {
			kptr = vptr->key_val_arr;
			
			if (kptr == NULL) {
				printf("> %s %d print_np_key_val() : The key_val_arr is NULL\n\n", __FILE__, __LINE__);
				return;
			}
			
			printf("> %s %d print_np_key_val() : *KPTR is %s\n",__FILE__, __LINE__, *kptr);

			while (*kptr) {
				printf("\t\t\t\t%s\n", *kptr);
				kptr++;
			}
			
			vptr = vptr->next;
		}
		head = head->next;
	}
return;
}

/* Search the application and return a pointer to it's node; return NULL if not found */

app_node *search_app(app_dcll * l, char *val)
{

	int i = l->count;
	int found = 0;
	app_node *ptr;

	if (l->count == 0)
		return NULL;

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
		errno = EEXIST;
		return NULL;
	}

	else {
		errno = ENODEV;
		return ptr;
	}
}

/* Search a registration in the Application List, for the given appname and npname and return -1 if found, otherwise return 0 */

int search_reg(app_dcll * l, char *appname, char *npname)
{

	app_node *ptr;
	np_node *nptr;
	
	ptr = search_app(l, appname);

	printf("\n> %s %d search_reg() : Searching Registration\n", __FILE__, __LINE__);

	if (ptr == NULL)
		return 0;

	if (ptr != NULL) {

		printf("> %s %d search_reg() : App %s has been found. Checking np %s in app's list\n",__FILE__, __LINE__, appname, npname);
		nptr = ptr->np_list_head;

		while (nptr != NULL) {          /* Code for checking NP existance */
			if (!strcmp(npname, nptr->name)) {
				printf("> %s %d search_reg() : Duplicate Registration Detected", __FILE__, __LINE__);
				errno = EEXIST;
				return -1;
			}
			nptr = nptr->next;
		}

	}

	errno = ENODEV;
	return 0;
}

/* Search a registration in the Application List, for the given appname and npname and return a pointer to the npnode in the trailing list if found, otherwise return NULL */

np_node *get_reg(app_dcll * l, char *appname, char *npname)
{

	app_node *ptr;
	np_node *nptr;
	
	ptr = search_app(l, appname);

	if (ptr == NULL)
		return 0;

	if (ptr != NULL) {

		printf("> %s %d get_reg() : App %s has been found. Checking np %s in app's list\n",__FILE__, __LINE__, appname, npname);
		nptr = ptr->np_list_head;

		/* Check the existance of the np in the trailing list */
		while (nptr != NULL) {
			if (!strcmp(npname, nptr->name)) {
				errno = EEXIST;
				return nptr;
			}
			nptr = nptr->next;
		}

	}
	errno = ENODEV;
	return NULL;

}

/* Delete application with given name */

int del_app(app_dcll * l, char *val)
{
	char **np_key_val_arr;
	int i = 0;
	np_node *np_tail, *np_temp;
	app_node *p, *temp, *q;
	struct extr_key_val *extr_kv, *extr_kv_temp;

/* Check whether the requested application exists */

	temp = search_app(l, val);
	
	if (temp == NULL) {
		errno = ENODEV;
		return -2;
	}

/* If it is at the head of the list and the only application registered */
	else if (temp == l->head && l->count == 1) {

		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
	}

/* If it is at the head of the list and there are more applications */
	else if (temp == l->head && l->count > 1) {

		l->head = temp->next;
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
	} 

/* Any other case */
	else {
		p = temp->prev;
		q = temp->next;
		p->next = q;
		q->prev = p;
	}
	
	/* Free logic */

	np_tail = temp->np_list_head;
	
	while(np_tail != NULL) {
		extr_kv = np_tail->key_val_ptr;
		while(extr_kv != NULL) {
			np_key_val_arr = (extr_kv)->key_val_arr;
		 	i = 0;
			while(np_key_val_arr[i] != NULL) {
				free(np_key_val_arr[i]);
				np_key_val_arr[i] = NULL;
				i++;
			}
			extr_kv_temp = extr_kv;
			extr_kv = extr_kv->next;
			free(extr_kv_temp);
			extr_kv_temp = NULL;
		}
		np_temp = np_tail;
		np_tail = np_tail->next;
		free(np_temp->name);
		np_temp->name = NULL;
		free(np_temp);
		np_temp = NULL;
	}
	
	free(temp->data);
	temp->data = NULL;
	free(temp);
	temp = NULL;
	l->count--;
	return 0;	
}

/* Add to the trailing list of the application "aval" the np "nval" */

int add_np_to_app(app_dcll * l, char *aval, char *nval)
{

	app_node *temp;
	np_node *n, *m, *b;

/* Search if the application even exists */

    temp = search_app(l, aval);

	if (temp == NULL) {
		errno = ENODEV;
		return -2;
	}

	n = (np_node *)malloc(sizeof(np_node));

	if (n == NULL) {
		errno = ECANCELED;
		perror("APP_DCLL :  ERROR IN MALLOC");
		exit(1);
	}

	n->name = (char *)malloc((strlen(nval) + 1) * sizeof(char));
	n->key_val_ptr = NULL;

	if (n->name == NULL) {
		errno = ECANCELED;
		perror("APP_DCLL :  ERROR IN MALLOC");
		exit(1);
	}

	n->next = NULL;
	strcpy(n->name, nval);
	if (temp->np_list_head == NULL) {
		temp->np_list_head = n;
		temp->np_count++;
		return 0;
	} else {
		m = temp->np_list_head;
		b = m;
		if (m->name == nval) {
			errno = EEXIST;
			return ALREXST;
		}
		b = m;
		m = m->next;
		while (m != NULL) {
			if (m->name == nval) {
				errno = EEXIST;
				return -1;
			}
			b = m;
			m = m->next;
		}
		b->next = n;
		temp->np_count++;
	}
	return 0;
}

/* Delete the np "nval" from the trailing list of the application "aval" */

int del_np_from_app(app_dcll * l, char *aval, char *nval)
{
	char **np_key_val_arr;
	int i = 0;
	np_node *np_tail;
	struct extr_key_val *extr_kv, *extr_kv_temp;
	
	int flag = 0;
	app_node *temp = search_app(l, aval);
	np_node *n, *m, *b;

	if (l->count == 0) {
		errno = ENODEV;
		return NOTFND;
	}
		
	if (temp == NULL) {
		errno = ENODEV;
		return NOTFND;
	}
	
	if (temp->np_list_head == NULL) {
		return 0;
	}
	
	if (temp->np_list_head->next == NULL) {
		m = temp->np_list_head;
		temp->np_list_head = m->next;
		temp->np_count--;
		printf("\n> %s %d del_np_from_app() : temp->np_count = %d\n", __FILE__, __LINE__, temp->np_count);
		flag = 1;
	} 
	
	else if (!strcmp(temp->np_list_head->name, nval)) {
		m = temp->np_list_head;
		n = m->next;
		temp->np_list_head = n;
		temp->np_count--;
		printf("\n> %s %d del_np_from_app() : temp->np_count = %d\n",__FILE__, __LINE__, temp->np_count);
		flag = 1;
	} 
	
	else {
		m = temp->np_list_head;
		b = m;
		
		if (temp->np_list_head == m && m->name == nval) {
			temp->np_list_head = m->next;
			temp->np_count--;
			printf("\n> %s %d del_np_from_app() : temp->np_count = %d\n",__FILE__, __LINE__, temp->np_count);
			flag = 1;
		}
		m = m->next;
		
		while (m != NULL) {
		
			if (strcmp(m->name, nval) == 0) {
				b->next = m->next;
				temp->np_count--;
				printf("\n> %s %d del_np_from_app() : temp->np_count = %d\n",__FILE__, __LINE__, temp->np_count);
				flag = 1;
				break;
			}
			b = m;
			m = m->next;
		}
	}
	if(flag == 0) {
		errno = ENODEV;	
		return -1;
	}

/* Logic for freeing the memory associated with the registration */

	np_tail = m;
		
	extr_kv = np_tail->key_val_ptr;
	while(extr_kv != NULL) {
		np_key_val_arr = (extr_kv)->key_val_arr;
	 	i = 0;
		while(np_key_val_arr[i] != NULL) {
			free(np_key_val_arr[i]);
			np_key_val_arr[i] = NULL;
			i++;
		}
		extr_kv_temp = extr_kv;
		extr_kv = extr_kv->next;
		free(extr_kv_temp);
		extr_kv_temp = NULL;
	}
	free(np_tail->name);
	np_tail->name = NULL;
	free(np_tail);	
	np_tail = NULL;
	return 0;
}
/*
void empty_app_list(app_dcll * l) {
	
	int i = l->count;
	struct app_node *temp;
	
	temp = l->head;
	
	
	for(; i != 0;i--) {
		del_app( l, temp->data);
		temp = temp->next;
	}
	printf(">%s %d empty_app_list : App List deleted completely  %d \n ",__FILE__ , __LINE__, l->count);
	return ;
}
*/

/* Below is the code to test the list */

/*int main() {

	app_dcll l;
	char val[25], aval[25], nval[25];
	init_app(&l);
	
	int ch;
	while(1) {
		//printf("APP_DCLL : Enter Choice :\n0 - printApp\n1 - Add App\n2 - search_app App\n3 - Delete App\n4 - Add NP to App\n5 - Delete NP from App\n");
		scanf("%d", &ch);
		switch(ch) {
	
			case 0 :	print_app(&l);
					break;
			case 1 :	//printf("APP_DCLL : Enter Val :\n");
					scanf(" %s", val);
					add_app_node(&l, val);
					print_app(&l);
					break;
			case 2 : 	//printf("APP_DCLL : Enter Val :\n");
					scanf(" %s", val);
					search_app(&l, val);
					break;
			case 3 :	//printf("APP_DCLL : Enter Val :\n");
					scanf(" %s", val);
					del_app(&l, val);
					print_app(&l);
					break;
			case 4 :	//printf("APP_DCLL : Enter Val for App and NP:\n");
					scanf(" %s %s", aval, nval);
					//printf("APP_DCLL : Aval - %s| and nval - %s|\n", aval, nval);
					add_np_to_app(&l, aval, nval);
					print_app(&l);
					break;
			case 5 :	//printf("APP_DCLL : Enter Val for App and NP:\n");
					scanf(" %s %s", aval, nval);
					del_np_from_app(&l, aval, nval);
					print_app(&l);
					break;
		}
	}
	return 0;

}

*/
