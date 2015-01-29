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

/* INITIALISE APPLICATION LIST */

void init_app(app_dcll * l)
{
	l->head = NULL;
	l->count = 0;
}

/* APPEND NEW APPLICATION-NODE TO THE LIST */

int addapp_node(app_dcll * l, char *val)
{
	app_node *new, *tmp;
	tmp = search_app(l, val);	/* Search a app_node in app_list if it already exists */
	if (tmp != NULL)
		return ALREXST;	/* If node already exists, it returns ALREXST error */
	new = (app_node *) malloc(sizeof(app_node));

	if (new == NULL) {
		perror("APP_DCLL : ERROR IN MALLOC");
		exit(1);
	}

	new->data =(char *)malloc(sizeof(val) + 1);

	if (new->data == NULL) {
		perror("APP_DCLL : ERROR IN MALLOC");
		exit(1);
	}
	//printf("APP_DCLL :  COUNT IN ADD-APP-NODE : %d\n", l->count);

	new->np_count = 0;

	if (l->count == 0) {
		l->head = new;
		new->prev = new;
		new->next = new;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		l->count++;
		//printf("APP_DCLL : COUNT AFTER APPEND : %d\n", l->count);
		return 0;
	}

	else if (l->count == 1) {
		//printf("APP_DCLL : DATA IN LIST HEAD : %s\n", l->head->data);
		l->head->prev = new;
		l->head->next = new;
		new->prev = l->head;
		new->next = l->head;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		//printf("APP_DCLL : DATA IN NEW NODE : %s\n", l->head->next->data);
		l->count++;
		//printf("APP_DCLL : COUNT AFTER APPEND : %d\n", l->count);
		return 0;
	}

	else {
		l->head->prev->next = new;
		new->prev = l->head->prev;
		new->next = l->head;
		l->head->prev = new;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		//printf("APP_DCLL : DATA IN NEW NODE : %s\n", l->head->next->data);
		l->count++;
		//printf("APP_DCLL : COUNT AFTER APPEND : %d\n", l->count);
		return 0;
	}
}

/* PRINT LIST */

void print_app(app_dcll * l)
{

	int i = l->count;
	if (l->count == 0) {
		printf("APP_DCLL : NO APPS TO PRINT\n");
		return;
	}
	printf("\nTotal number of applications : %d\n", l->count);
	app_node *ptr;
	np_node *np;
	ptr = l->head;
	printf("\nApp\t\t\tNp_Count");
	printf("\n===================================\n");
	while (i) {		/* Printing list in forward direction */

		printf("%s\t\t\t", ptr->data);
		printf("%d\n", ptr->np_count);
		printNpKeyVal(ptr);
		printf("\n\n\n");
		ptr = ptr->next;
		i--;
	}

// Printing registrations

	i = l->count;
	ptr = l->head;
	if (i != 0) {
		printf("\nApp\t\t\tRegistered with");
		printf("\n===================================\n");

	}

	while (i) {
		np = ptr->np_list_head;
		printf("\n%s\t\t\t", ptr->data);
		//printf("%s\n", np->name);     
		while (np != NULL) {
			printf("%s\n", np->name);
			np = np->next;
			printf("\t\t\t");
		}

		ptr = ptr->next;
		i--;
	}

	/* Printing list in backward direction */

	/*
	   printf("APP_DCLL : \n\n");

	   ptr = (l->head)->prev;
	   i = l->count;
	   while(i) {
	 */
	//      printf("APP_DCLL : NODE :");
	//      printf("APP_DCLL : %s ", ptr->data);
	//      np = ptr->np_list_head;
	//      printf("APP_DCLL : NP's data is %s\n", np->name);
	//      while(np != NULL) {
	//              printf("APP_DCLL : NP - %s", np->name); 
	//              np = np->next;          
	//      }
	//      ptr = ptr->prev;
	//      printf("APP_DCLL :  <==> \n");
	//      i--;

	//}
	//printf("APP_DCLL : \n");

}

void printNpKeyVal(app_node * temp)
{
	printf("INSIDE PRINTNPKEYVAL temp->data = %s \n", temp->data);
	np_node *head = temp->np_list_head;

	if (!head) {
		printf("HEADLESS NICK\n\n");
		return;
	}

	printf("HEAD DATA = %s\n", head->name);
	extr_key_val *vptr;
	while (head) {
		vptr = head->key_val_ptr;
		if (vptr == NULL) {
			printf("VPTR IS NULL\n");
			head = head->next;
			continue;
		}
		printf("%s IS head_name \n", head->name);
		while (vptr) {
			// For every np_node
			char **kptr;
			kptr = vptr->key_val_arr;
			if (kptr == NULL) {
				printf("HERE LIES THE PROBLEM\n\n");
				return;
			}
			printf("VPTR->KEYVALARR is %s\n", *(vptr->key_val_arr));

			printf
			    ("\n==================================================\n");

			printf("*KPTR is %s\n", *kptr);

			while (*kptr) {
				printf("IN THE WHILE OF KPTR\n\n");
				printf("\t\t\t\t%s\n", *kptr);
				kptr++;
			}
			printf("OUT OF THE WHILE OF KPTR\n\n");
			vptr = vptr->next;
			printf("VPTR PROGRESSED\n\n");

		}
		head = head->next;
	}
	return;
}

/* SEARCH APP, RETURN POINTER TO NODE */

app_node *search_app(app_dcll * l, char *val)
{

	int i = l->count;
	int found = 0;
	app_node *ptr;

	if (l->count == 0)
		return;

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
		//printf("APP_DCLL : NOT FOUND : %s\n", val);
		return NULL;
	}

	else
		return ptr;
}

int searchReg(app_dcll * l, char *appname, char *npname)
{

	app_node *ptr;
	np_node *nptr;
	int k;
	ptr = search_app(l, appname);

	printf("Searching Registration\n");

	if (ptr == NULL)
		return 0;

	if (ptr != NULL) {

		printf("App %s has been found. Checking np %s in app's list\n",
		       appname, npname);
		nptr = ptr->np_list_head;
		k = ptr->np_count;

		/* CODE FOR CHECKING NP EXISTANCE */
		while (nptr != NULL) {
			if (!strcmp(npname, nptr->name)) {
				printf("Duplicate Registration Detected");
				return -1;
			}
			nptr = nptr->next;
		}

	}

	return 0;
}

np_node *getReg(app_dcll * l, char *appname, char *npname)
{

	app_node *ptr;
	np_node *nptr;
	int k;
	ptr = search_app(l, appname);

	//printf("Searching Registration\n");

	if (ptr == NULL)
		return 0;

	if (ptr != NULL) {

		printf("App %s has been found. Checking np %s in app's list\n",appname, npname);
		nptr = ptr->np_list_head;
		k = ptr->np_count;

		/* CODE FOR CHECKING NP EXISTANCE */
		while (nptr != NULL) {
			if (!strcmp(npname, nptr->name)) {
				//printf("Duplicate Registration Detected");
				return nptr;
			}
			nptr = nptr->next;
		}

	}

	return NULL;

}

/* DELETE APP */

int del_app(app_dcll * l, char *val)
{

	app_node *p, *temp, *q;
	temp = search_app(l, val);	/* Search a list to check if it exists for deletion */
	//printf("APP_DCLL : DELETING VALUE : %s\n", temp->data);

	if (temp == NULL) {
		//printf("APP_DCLL : NOT FOUND : %s\n", val);
		return NOTFND;
	}

	else if (temp == l->head && l->count == 1) {
		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
		free(temp);
		l->count--;
		return 0;
	}

	else if (temp == l->head && l->count > 1) {

		//printf("APP_DCLL : DELETING HEAD\n");
		l->head = temp->next;
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
		free(temp);
		l->count--;
		return 0;
	} else {
		p = temp->prev;
		//printf("APP_DCLL : p:%s\n", p->data);
		q = temp->next;
		//printf("APP_DCLL : q:%s\n", q->data);
		p->next = q;
		q->prev = p;
		free(temp);
		l->count--;
		return 0;
	}
}

/* ADD NP TO APPLICATION */
/*
int add_np_to_app(app_dcll *l, char *aval, char *nval)	{

	//printf("APP_DCLL : ADDING TO APPLICATION %s, NP %s\n", aval, nval);
	app_node *temp = search_app(l, aval);	
	np_node *n, *m, *b;
    int i;
	if(temp == NULL)	{	
		//printf("APP_DCLL : NOT FOUND %s\n", aval);
		return NOTFND;
	}
		
    printf("You asked to add np %s to app %s\n", nval, aval);
	np_node *np_ptr;
	np_ptr = temp->np_list_head;
	while(np_ptr != NULL) {
		if(!strcpy(np_ptr->name, nval)) {
			return ALREXST;
		}
		np_ptr = np_ptr->next;
	}
		
	n = malloc(sizeof(np_node));
	
	if(n == NULL){
		perror("APP_DCLL :  ERROR IN MALLOC");
		exit(1);
	}

	n->name = malloc(sizeof(nval) + 1);

	if(n->name == NULL){
		perror("APP_DCLL :  ERROR IN MALLOC");
		exit(1);
	}
	n->next = NULL;
	strcpy(n->name, nval);
	if(temp->np_list_head == NULL)	{
		temp->np_list_head = n;
		temp->np_count++;
		printf("temp->np_count = %d\n", temp->np_count);
		return 0;
	}
	else	{
		m = temp->np_list_head;
		b = m;
		if(m->name == nval)	{
				//printf("APP_DCLL : EXISTING NODE\n");
				return ALREXST;
		}
		b = m;
		m = m->next;
		while(m != NULL)	{
			if(m->name == nval)	{
				//printf("APP_DCLL : EXISTING NODE\n");
				return ALREXST;
			}
			b = m;
			m = m->next;
		}
		b->next = n;
		temp->np_count++;
		printf("temp->np_count = %d\n", temp->np_count);

	}

    printf("Printing from add_np_to_app :\n");

printf("App - %s:\n", temp->data);
np_ptr = temp->np_list_head;
i = temp->np_count;
while(i) {
    printf("Np is %s\n", np_ptr->name);
    np_ptr = np_ptr->next;
    i--;

}

	return 0;
}


*/

int add_np_to_app(app_dcll * l, char *aval, char *nval)
{

	//printf("APP_DCLL : ADDING TO APPLICATION %s, NP %s\n", aval, nval);
	app_node *temp = search_app(l, aval);
	np_node *n, *m, *b;
	if (temp == NULL) {
		//printf("APP_DCLL : NOT FOUND %s\n", aval);
		return NOTFND;
	}

	else {
		//printf("APP_DCLL : FOUND AND ADDING TO %s\n", temp->data);    
	}

	n = malloc(sizeof(np_node));

	if (n == NULL) {
		perror("APP_DCLL :  ERROR IN MALLOC");
		exit(1);
	}

	n->name = malloc(sizeof(nval) + 1);
	n->arguments = NULL;
	n->key_val_ptr = NULL;

	if (n->name == NULL) {
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
			//printf("APP_DCLL : EXISTING NODE\n");
			return ALREXST;
		}
		b = m;
		m = m->next;
		while (m != NULL) {
			if (m->name == nval) {
				//printf("APP_DCLL : EXISTING NODE\n");
				return ALREXST;
			}
			b = m;
			m = m->next;
		}
		b->next = n;
		temp->np_count++;
	}
	return 0;
}

/* DELETE NP FROM APPLICATION */

int del_np_from_app(app_dcll * l, char *aval, char *nval)
{
	app_node *temp = search_app(l, aval);
	np_node *n, *m, *b;
	if (l->count == 0)
		return NOTFND;
	if (temp == NULL) {
		//printf("APP_DCLL : NOT FOUND %s\n", aval);
		return NOTFND;
	}
	if (temp->np_list_head == NULL) {
		//printf("APP_DCLL : NOT FOUND %s\n", nval);
		return 0;
	}
	if (temp->np_list_head->next == NULL) {
		m = temp->np_list_head;
		temp->np_list_head = m->next;
		temp->np_count--;
		printf("temp->np_count = %d\n", temp->np_count);

		free(m);
		return 0;
	} else if (!strcmp(temp->np_list_head->name, nval)) {
		m = temp->np_list_head;
		n = m->next;
		temp->np_list_head = n;
		temp->np_count--;
		printf("temp->np_count = %d\n", temp->np_count);

		free(m);
		return 0;
	} else {
		m = temp->np_list_head;
		b = m;
		if (temp->np_list_head == m && m->name == nval) {
			temp->np_list_head = m->next;
			temp->np_count--;
			printf("temp->np_count = %d\n", temp->np_count);

		free(m);
			return 0;
		}
		m = m->next;
		while (m != NULL) {
			if (strcmp(m->name, nval) == 0) {

				b->next = m->next;
				temp->np_count--;
				printf("temp->np_count = %d\n", temp->np_count);

				free(m);
				return 0;
			}
			b = m;
			m = m->next;
		}
	}
}

/* Below is the code to test the list */

/*int main() {

	app_dcll l;
	char val[25], aval[25], nval[25];
	init_app(&l);
	
	int ch;
	while(1) {
		//printf("APP_DCLL : Enter Choice :\n0 - print_app\n1 - Add App\n2 - search_app App\n3 - Delete App\n4 - Add NP to App\n5 - Delete NP from App\n");
		scanf("%d", &ch);
		switch(ch) {
	
			case 0 :	print_app(&l);
					break;
			case 1 :	//printf("APP_DCLL : Enter Val :\n");
					scanf(" %s", val);
					addapp_node(&l, val);
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
