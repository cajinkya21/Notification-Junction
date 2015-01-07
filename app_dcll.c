#include "app_dcll.h"


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

/* INITIALISE APPLICATION LIST */

void init_app(app_dcll *l) {
	
	l->head = NULL;
	l->count = 0;
	//printf("APP_DCLL : LIST INITIALISED\n");

}


/* APPEND NEW APPLICATION-NODE TO THE LIST */

int addapp_node(app_dcll *l, char *val) {
	
	app_node *new, *tmp;
	tmp = search_app(l, val);
	
	if(tmp != NULL) return ALREXST;
	
	new = (app_node *)malloc(sizeof(app_node));
	
	if(new == NULL){
		perror("APP_DCLL : ERROR IN MALLOC");
		exit(1);
	}
	
	new->data = malloc(sizeof(val) + 1);
	
	if(new->data == NULL){
		perror("APP_DCLL : ERROR IN MALLOC");
		exit(1);
	}
	
	////printf("APP_DCLL :  COUNT IN ADD-APP-NODE : %d\n", l->count);
	
	new->np_count = 0;
	
	if(l->count == 0) {
	
		l->head = new;
		new->prev = new;
		new->next = new;
		strcpy(new->data, val);
		new->np_list_head = NULL;
		l->count++;

		//printf("APP_DCLL : COUNT AFTER APPEND : %d\n", l->count);
		return 0;
	}
	
	else if(l->count == 1) {
	
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
void print_app(app_dcll *l) {

	int i = l->count;
	if(l->count == 0) {
		printf("APP_DCLL : NO APPS TO PRINT\n");
	}	
	printf("APP_DCLL : COUNT : %d\n", l->count);
	app_node *ptr;
	np_node *np;
	ptr = l->head;
	while(i) {
		printf("APP_DCLL : NODE :");
		printf("APP_DCLL : %s ", ptr->data);
		printf("APP_DCLL : NP_CNT: %d\n", ptr->np_count);
		np = ptr->np_list_head;
		while(np != NULL) {
			printf("APP_DCLL : NP - %s\n", np->name);	
			np = np->next;		
		}
		ptr = ptr->next;
		printf("APP_DCLL :  ---------- \n");
		i--;
	}
//	//printf("APP_DCLL : \n\n");
	/*ptr = (l->head)->prev;
	i = l->count;
	while(i) {
		//printf("APP_DCLL : NODE :");
		//printf("APP_DCLL : %s ", ptr->data);
		np = ptr->np_list_head;
		//printf("APP_DCLL : NP's data is %s\n", np->name);
		while(np != NULL) {
			//printf("APP_DCLL : NP - %s", np->name);	
			np = np->next;		
		}
		ptr = ptr->prev;
		//printf("APP_DCLL :  <==> \n");
		i--;
	}
	//printf("APP_DCLL : \n");*/
}


/* SEARCH APP, RETURN POINTERTO NODE */

app_node* search_app(app_dcll *l, char *val) {

	int i = l->count;
	int found = 0;
	app_node *ptr;
	
	if(l->count == 0)	return;
	
	ptr = l->head;
	
	while(i) {
		if(strcmp(ptr->data, val) == 0) {
			found = 1;
			break;
		}
		i--;
		ptr = ptr->next;
	}	
	
	if(found == 0) {
		//printf("APP_DCLL : NOT FOUND : %s\n", val);
		return NULL;
	}
	
	else return ptr;
}

/* DELETE APP */

int del_app(app_dcll *l, char *val)	{
	
	app_node *p, *temp, *q;
	temp = search_app(l, val);
	//printf("APP_DCLL : DELETING VALUE : %s\n", temp->data);
	
	if(temp == NULL)	{
		//printf("APP_DCLL : NOT FOUND : %s\n", val);
		return NOTFND;
	}
	
	else if(temp == l->head && l->count == 1) {
		l->head->prev = NULL;
		l->head->next = NULL;
		l->head = NULL;
		free(temp);
		l->count--;
		return 0;
	}
	
	else if(temp == l->head  && l->count > 1) {

		//printf("APP_DCLL : DELETING HEAD\n");
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
		////printf("APP_DCLL : p:%s\n", p->data);
		q = temp->next;
		////printf("APP_DCLL : q:%s\n", q->data);
		p->next = q;
		q->prev = p;
		free(temp);
		l->count--;
		return 0;	
	}
}

/* ADD NP TO APPLICATION */

int add_np_to_app(app_dcll *l, char *aval, char *nval)	{



	//printf("APP_DCLL : ADDING TO APPLICATION %s, NP %s\n", aval, nval);
	app_node *temp = search_app(l, aval);	
	np_node *n, *m, *b;
	if(temp == NULL)	{	
		//printf("APP_DCLL : NOT FOUND %s\n", aval);
		return NOTFND;
	}
	
	
	np_node *np_ptr;
	np_ptr = temp->np_list_head;
	while(np_ptr != NULL) {
		if(!strcpy(np_ptr->name, nval)) {
			return ALREXST;
		}
		np_ptr = np_ptr->next;
	}
	
		
	//}
	
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
	}
	return 0;
}
int del_np_from_app(app_dcll *l, char *aval, char *nval)
{
	app_node *temp = search_app(l, aval);	
	np_node *n, *m, *b;
	if(l->count == 0)
		return NOTFND;
	if(temp == NULL)
	{	
		//printf("APP_DCLL : NOT FOUND %s\n", aval);
		return NOTFND;
	}
	if(temp->np_list_head == NULL)
	{
		//printf("APP_DCLL : NOT FOUND %s\n", nval);
		return 0;
	}
	if(temp->np_list_head->next == NULL) {
		m = temp->np_list_head;
		temp->np_list_head = m->next;
		temp->np_count--;
		free(m);
		return 0;
	}
	else if( !strcmp(temp->np_list_head->name, nval)) {
		m = temp->np_list_head;
		n = m->next;
		temp->np_list_head = n;
		temp->np_count--;
		free(m);
		return 0;
	}
	else
	{
		m = temp->np_list_head;
		b = m;
		if(temp->np_list_head == m && m->name == nval) {
					temp->np_list_head = m->next;
					temp->np_count--;
					free(m);			
					return 0;	
		}
		m = m->next;
		while(m != NULL)	
		{
			if(strcmp(m->name, nval) == 0)
			{
				
					b->next = m->next;
					temp->np_count--;
					free(m);
					return 0;
			}
			b = m;
			m = m->next;
		}
	}
}
