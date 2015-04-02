/*  
    Notification-Junction is an interface between multiple applications and multiple Notification Providers.
    Copyright (C) 2015      
    
    Navroop Kaur<watwanichitra@gmail.com>, 
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
	pthread_rdwr_init_np(&(l->np_list_lock), NULL);	
	l->head = NULL;
	l->count = 0;
}

/* Add an NP with the given name to the linked list for NPs, return 0 on success */

int add_np(np_dcll * l, char *val, char *usage, char ***key_val_arr)
{
	char **np_key_val_arr;
	int i = 0;
	main_np_node *new, *temp, *p, *q;

	temp = search_np(l, val);

	pthread_rdwr_wlock_np(&(l->np_list_lock));	
    
    /* case when np already exists */
	if (temp != NULL) { 
		/* only one node in the np list */
		if ((temp == (l->head)) && (l->count == 1)) {
			l->head->prev = NULL;
			l->head->next = NULL;
			l->head = NULL;
		}
        /* first node and more nodes in np list */
		else if ((temp == (l->head)) && (l->count > 1)) {
			l->head = temp->next;
			p = temp->prev;
			(temp->next)->prev = p;
			p->next = temp->next;
		} 
		/* other case */
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
		l->count = l->count - 1;
	}

    
	new = (main_np_node *) malloc(sizeof(main_np_node));

	if (new == NULL) {
		errno = ECANCELED;
		perror("NP_DCLL : ERROR IN MALLOC");
		pthread_rdwr_wunlock_np(&(l->np_list_lock));
		return -1;
	}

    
	new->data = (char *)malloc((strlen(val) + 1) * sizeof(char));
	new->usage = usage;
	new->key_val_arr = *key_val_arr;

	if (new->data == NULL) {
		errno = ECANCELED;
		perror("NP_DCLL : ERROR IN MALLOC");
		pthread_rdwr_wunlock_np(&(l->np_list_lock));
		return -1;
	}

	new->app_count = 0;

    /* first node in the list */
	if (l->count == 0) {
		l->head = new;
		new->prev = new;
		new->next = new;
	}

    /* only one node in the list */
	else if (l->count == 1) {
		l->head->prev = new;
		l->head->next = new;
		new->prev = l->head;
		new->next = l->head;
	}
    /* other case */
	else {
		l->head->prev->next = new;
		new->prev = l->head->prev;
		new->next = l->head;
		l->head->prev = new;
	}

	l->count++;

	strcpy(new->data, val);

	pthread_rdwr_wunlock_np(&(l->np_list_lock));
	return 0;
}
/* print the NP list */
void print_np(np_dcll * l)  {

	int i ;
	main_np_node *ptr;
	char **kptr;

	pthread_rdwr_rlock_np(&(l->np_list_lock));

	i = l->count;
	if (l->count == 0) {
		printf("> %s %d print_np() : NP_DCLL  : Nothing to print_np\n", __FILE__, __LINE__);
		pthread_rdwr_runlock_np(&(l->np_list_lock));
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
		kptr = ptr->key_val_arr;
		while (*kptr) {
			printf("\t\t\t\t%s\n", *kptr);
			kptr++;
		}

		printf("\n");
		ptr = ptr->next;
		i--;
	}
	printf("====================================xx======================================\n");
	pthread_rdwr_runlock_np(&(l->np_list_lock));
}

/* Search for an np with the given name and return a pointer to that node if found */

main_np_node *search_np(np_dcll * l, char *val) {

	int i;
	int found = 0;
	main_np_node *ptr;

	pthread_rdwr_rlock_np(&(l->np_list_lock));

	i = l->count;
	if (l->count == 0) {
		errno = ENODEV;
		pthread_rdwr_runlock_np(&(l->np_list_lock));
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
		pthread_rdwr_runlock_np(&(l->np_list_lock));
		return NULL;
	}

	else {
		errno = EEXIST;
		pthread_rdwr_runlock_np(&(l->np_list_lock));
		return ptr;
	}
}

/*Delete the main_np_node whose pointer is passed as argument*/
int del_np_node(np_dcll * l, main_np_node * np_to_del) {
	char **np_key_val_arr;
	int i = 0;
	main_np_node *p,*temp, *q;

	pthread_rdwr_wlock_np(&(l->np_list_lock));
	temp = np_to_del;

	if ((temp == (l->head)) && (l->count == 1)) {
		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
	}

	else if ((temp == (l->head)) && (l->count > 1)) {
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
	temp->usage = NULL;
	free(temp->data);
	temp->data = NULL;
	free(temp);
	temp = NULL;
	l->count = (l->count - 1);
	pthread_rdwr_wunlock_np(&(l->np_list_lock));
	return 0;

}

/* Delete the np with the given name, if found */

int del_np(np_dcll * l, char *val)  {	
	char **np_key_val_arr;
	int i = 0;
	main_np_node *p, *temp, *q;
	temp = search_np(l, val);

	pthread_rdwr_wlock_np(&(l->np_list_lock));

	l->count = ( l->count - 1) ;
	if (temp == NULL) {
		errno = ENODEV;
		pthread_rdwr_wunlock_np(&(l->np_list_lock));
		return -1;
	}

	else if ((temp == (l->head)) && (l->count == 1)) {
		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
	}

	else if ((temp == (l->head)) && (l->count > 1)) {
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
	pthread_rdwr_wunlock_np(&(l->np_list_lock));
	return 0;

}

/* For the np with the given name, if found, find the count of the applications that have registered with it */

int get_np_app_cnt(np_dcll * l, char *nval)
{
	main_np_node *temp;

	temp = search_np(l, nval);

	pthread_rdwr_rlock_np(&(l->np_list_lock));

	if (temp == NULL) {
		errno = ENODEV;	
		pthread_rdwr_runlock_np(&(l->np_list_lock));
		return -1;
	}

	pthread_rdwr_runlock_np(&(l->np_list_lock));

	return temp->app_count;
}

/* For the np with the given name, if found, increment the count of the applications that have registered with it */

void incr_np_app_cnt(np_dcll * l, char *nval)   {

	main_np_node *temp;

	temp = search_np(l, nval);
	pthread_rdwr_wlock_np(&(l->np_list_lock));

	if (temp == NULL) {
		errno = ENODEV;	
		pthread_rdwr_wunlock_np(&(l->np_list_lock));
		return;
	}

	temp->app_count++;
	pthread_rdwr_wunlock_np(&(l->np_list_lock));
}

/* For the np with the given name, if found, decrement the count of the applications that have registered with it */

void decr_np_app_cnt(np_dcll * l, char *nval)   {

	main_np_node *temp;
	temp = search_np(l, nval);
	pthread_rdwr_wlock_np(&(l->np_list_lock));
	if (temp == NULL) {
		errno = ENODEV;	
		pthread_rdwr_wunlock_np(&(l->np_list_lock));
		return;
	}

	temp->app_count = (temp->app_count - 1);
	printf("> %s %d decr_np_app_cnt() :\t%d\n", __FILE__, __LINE__, temp->app_count);
	pthread_rdwr_wunlock_np(&(l->np_list_lock));
}

/****************************************** Hash functions ****************************************/

/* Add the NP to the hash structure */
int add_np_to_hash(hash_struct_np *hstruct, char *val, char *usage, char ***key_val_arr) {

    main_np_node *new, *s;
	new = (main_np_node *) malloc(sizeof(main_np_node));

	if (new == NULL) {
		errno = ECANCELED;
		perror("NP_DCLL : ERROR IN MALLOC");
		return -1;
	}

	new->data = (char *)malloc((strlen(val) + 1) * sizeof(char));
	if (new->data == NULL) {
		errno = ECANCELED;
		perror("NP_DCLL : ERROR IN MALLOC");
		pthread_rdwr_wunlock_np(&(hstruct->np_hash_lock));
		return -1;
	}


	new->usage = usage;
	new->key_val_arr = *key_val_arr;
	new->app_count = 0;
	strcpy(new->data, val);	
	pthread_rdwr_wlock_np(&(hstruct->np_hash_lock));	
	HASH_FIND_STR(hstruct->np_hash, new->data, s);  
	
	if (s == NULL) {
		HASH_ADD_KEYPTR( hh, hstruct->np_hash, new->data, strlen(new->data), new);  
	}
	else {
		errno = EEXIST;
		pthread_rdwr_wunlock_np(&(hstruct->np_hash_lock));
		return -1;
	}
	pthread_rdwr_wunlock_np(&(hstruct->np_hash_lock));
	return 0;
}

/* delete the NP from hash */
int del_np_from_hash(hash_struct_np *hstruct, char *val) {

	main_np_node *s;
	char **np_key_val_arr;
	int i = 0;

	pthread_rdwr_wlock_np(&(hstruct->np_hash_lock));
	
	/* check if NP exists*/	
	HASH_FIND_STR(hstruct->np_hash, val, s);  
	
	/* if it does, delete it and free memory */
	if(s != NULL) {
		np_key_val_arr = s->key_val_arr;
		i = 0;
		while(np_key_val_arr[i] != NULL) {
			free(np_key_val_arr[i]);
			np_key_val_arr[i] = NULL;
			i++;
		}
		free(s->usage);
		s->usage = NULL;
		free(s->data);
		s->data = NULL;
		HASH_DEL(hstruct->np_hash, s);
		free(s);
		s = NULL;
	}
	/* if it doesn't exist, error */
	else {
		 errno = EINVAL;
		 pthread_rdwr_wunlock_np(&(hstruct->np_hash_lock));
		 return -1;
	}
	pthread_rdwr_wunlock_np(&(hstruct->np_hash_lock));
	print_hash_np(hstruct);
	return 0;
}

/* print hash */
void print_hash_np(hash_struct_np *hstruct) {
	struct main_np_node *s;
	printf("\nPrinting NPs in hash :\n");
	pthread_rdwr_rlock_np(&(hstruct->np_hash_lock));
	for(s=hstruct->np_hash; s != NULL; s=(struct main_np_node*)(s->hh.next)) {
		printf("NP Name : %s\n", s->data);
	}
	pthread_rdwr_runlock_np(&(hstruct->np_hash_lock));
}

/* increment app_count of NP in the hash */
void incr_np_app_cnt_hash(hash_struct_np *hstruct, char *np_name){
	main_np_node *s;
	HASH_FIND_STR(hstruct->np_hash, np_name, s);  
	if(s != NULL) 
		(s->app_count)++;
}

/* decrement app_count of NP in the hash */
void decr_np_app_cnt_hash(hash_struct_np *hstruct, char *np_name) {
	main_np_node *s;
	HASH_FIND_STR(hstruct->np_hash, np_name, s);  /* id already in the hash? */
	if(s != NULL) 
		(s->app_count)--;
}

/* get app_count of NP */
int get_np_app_cnt_hash(hash_struct_np *hstruct, char* np_name) {

	main_np_node *s;
	
	HASH_FIND_STR(hstruct->np_hash, np_name, s);
	if(s != NULL)
		return s->app_count;
	else {
		errno = EINVAL;
		return -1;
	}
		
}

