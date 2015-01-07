#include "np_dcll.h"


/*int main() {

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

/* INITIALISE */
void init_np(np_dcll *l) {

	////printf("NP_DCLL  : In NP Init\n");	
	l->head = NULL;
	l->count = 0;
	//printf("NP_DCLL : Initialization done\n");
}


int add_np(np_dcll *l, char *val) {
	main_np_node *new;
	new = search_np(l, val);
	if(new != NULL){
		//printf("NP_DCLL : Existing NP\n");
		return ALREXST;
	}
	new = (main_np_node *)malloc(sizeof(main_np_node));
	if(new == NULL){
		perror("NP_DCLL : ERROR IN MALLOC");
		exit(1);
	}
	new->data = malloc(sizeof(val) + 1);
	if(new->data == NULL){
		perror("NP_DCLL : ERROR IN MALLOC");
		exit(1);
	}
	
	new->app_count = 0; 	
	////printf("NP_DCLL  : COUNT in add_main_np_node: %d\n", l->count);
	
	if(l->count == 0) {
		l->head = new;
		new->prev = new;
		new->next = new;
		strcpy(new->data, val);
		l->count++;
		////printf("NP_DCLL  : COUNT after add: %d\n", l->count);
		return 0; 
	}
	
	else if(l->count == 1) {
		////printf("NP_DCLL  : l head %s\n", l->head->data);
		l->head->prev = new;
		l->head->next = new;
		new->prev = l->head;
		new->next = l->head;
		strcpy(new->data, val);
		////printf("NP_DCLL  : new data %s\n", l->head->next->data);
		l->count++;	
		////printf("NP_DCLL  : COUNT after add: %d\n", l->count);
		return 0;
	}
	
	else {
		l->head->prev->next = new;
		new->prev = l->head->prev;
		new->next = l->head;
		l->head->prev = new;
		strcpy(new->data, val);
		////printf("NP_DCLL  : new data %s\n", l->head->next->data);
		l->count++;	
		////printf("NP_DCLL  : COUNT after add: %d\n", l->count);
		return 0;
	}
	
}

void print_np(np_dcll *l) {

	int i = l->count;
	if(l->count == 0) {
		printf("NP_DCLL  : Nothing to print_np\n");
		return ;
	}	
	printf("NP_DCLL  : COUNT : %d\n", l->count);
	main_np_node *ptr;
	ptr = l->head;
	while(i) {
		printf("NP_DCLL  : NODE :");
		printf("NP_DCLL  : %s   ", ptr->data);
		printf("NP_DCLL  : APP_CNT:%d\n", ptr->app_count);
		ptr = ptr->next;
		printf("NP_DCLL  :  <==> \n");
		i--;
	}
	//printf("NP_DCLL  : \n");
}

main_np_node* search_np(np_dcll *l, char *val) {
	int i = l->count;
	int found = 0;
	main_np_node *ptr;
	if(l->count == 0)	return NULL;
	ptr = l->head;
	while(i) {
		if(strcmp(ptr->data,val) == 0) {
			found = 1;
			break;
		}
		i--;
		ptr = ptr->next;
	}	
	if(found == 0) {
		//printf("NP_DCLL  : Not found %s\n", val);
		return NULL;
	}
	else return ptr;
}

int del_np(np_dcll *l, char *val)
{
	main_np_node *p, *temp;
	temp = search_np(l, val);
	////printf("NP_DCLL  : Deleting THIS VALUE - %s\n\n", temp->data);
	if(temp == NULL)	
	{
		//printf("NP_DCLL  : Not found %s\n", val);
		return NOTFND;
	}
	if(l->count == 1) {
		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
		free(temp);
		l->count--;
		return 0;
	}
	else if(temp == l->head) {
		//printf("NP_DCLL  : Asked to delete HEAD\n");
		l->head = temp->next;
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
		free(temp);
		l->count--;
		return 0;	
	}
	else
	{
		p = temp->prev;
		(temp->next)->prev = p;
		p->next = temp->next;
		free(temp);
		l->count--;
		return 0;	
	}
}

int get_np_app_cnt(np_dcll *l, char *nval)
{
	main_np_node *temp;
	temp = search_np(l, nval);
	if(temp == NULL)
		return NOTFND;
	return temp->app_count; 
}

void incr_np_app_cnt(np_dcll *l, char *nval)
{
	main_np_node *temp;
	temp = search_np(l, nval);
	if(temp == NULL)
		return;
	temp->app_count++; 	
	////printf("NP_DCLL  : App_cnt of %s = %d\n",nval, temp->app_count);
}
