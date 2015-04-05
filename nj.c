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
 *	The file contains the code for the Notification-Junction implementation 
 *	It has functions for 
 *	app registration	
 *	app unregistration
 *	np registration
 *	np unregistration 
 *	app_registraion thread method
 *	app_unregistration thread method
 *	np_registration thread method
 *	np_unregistration thread method
 *	app getnotify thread method
 *	np getnotify thread method
 *	proceed getnotify thread method
 *	block getnotify thread method
 *	dec_all_np_counts
 *	print_stat
 *	sigint_handler
 *	print_list_on_sock_np
 *	print_list_on_sock_app
 *	print_np_key_val_stat
 *	print_hash_on_sock_np
 *	print_hash_on_sock_app
 *	extract_key_val
 *	extract_key
 *	extract_val
 *	compare_array
 *	forward_convert
 *	get_val_from_args
 *	get_filename
 *	count_args
 *	nj_exit
 *	dec_all_np_counts_hash
 */

#include "nj.h"

np_dcll npList;						            /* Head of global NP List */
FILE* logfd;						            /* File pointer of log file */
app_dcll appList;					            /* Head of global APP List */
int fd_pidnames;					            /* FD of file used for storing PID filenames */
void *handle;						            /* Handle of the function in shared library */
pthread_mutex_t getnotify_socket_mutex;			/* mutex used for locking buffer which app_getnotify thread uses */
struct thread_args stat_read, stat_write, app_reg, np_reg, app_unreg,np_unreg, app_getnotify;
struct hash_struct_np *hstruct_np;			    /*global Hash for NP*/
struct hash_struct_app *hstruct_app;			/*global Hash for App*/
extern np_dcll npList;						    /* Head of global NP List */
extern FILE* logfd;						        /* File pointer of log file */
extern app_dcll appList;					    /* Head of global APP List */
extern int fd_pidnames;					        /* FD of file used for storing PID filenames */
extern void *handle;						    /* Handle of the function in shared library */
extern pthread_mutex_t getnotify_socket_mutex;
extern struct thread_args stat_read, stat_write, app_reg, np_reg, app_unreg,np_unreg, app_getnotify;

/* main code */
int main()
{	
    int ret;
	sigset_t mask;
	pthread_t tid_stat, tid_app_reg, tid_app_unreg, tid_np_reg, tid_np_unreg, tid_app_getnotify;

	atexit(nj_exit);  		/*setting up the exit handler */

	if (!(logfd = fopen(LOGS, "a+"))) { /*opening log file*/
		perror("NJ.C   : not able to open  Log file\n");
		return 1;
	}

	hstruct_np = (hash_struct_np *)malloc(sizeof(hash_struct_np));
	hstruct_np->np_hash = NULL;

	hstruct_app = (hash_struct_app *)malloc(sizeof(hash_struct_app));
	hstruct_app->app_hash = NULL;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	signal(SIGINT, sigint_handler);		/*setting up sig_int handler*/

	/* first remove(unlink) the sockets if they already exist */
	unlink(StatSocket);
	unlink(AppRegSocket);
	unlink(AppUnRegSocket);
	unlink(NpRegSocket);
	unlink(NpUnRegSocket);
	unlink(AppGetnotifySocket);
	unlink(StatSocketPrint);

	/*open file to which we will write the entry for each file pid.txt that we open so that it can be deleted when its use is done*/
	fd_pidnames = open(PIDFILE, O_CREAT | O_RDWR, 0777);
    
    fprintf(logfd, "%s %d main(): fd_pidnames created.\n",__FILE__ , __LINE__);
	force_logs();
	
	/*Initialization of all mutexes */

	if (pthread_mutex_init(&getnotify_socket_mutex, NULL) != 0) {
		perror("NJ.C   : \n getnotify socket mutex init failed\n");
		exit(EXIT_FAILURE);
	}


	/*Initialize NP List */

	init_np(&npList);

	/*Initialize APP List */

	init_app(&appList);

    fprintf(logfd, "%s %d main(): Lists initialized.\n",__FILE__ , __LINE__);
	force_logs();

	/* Create socket for stat*/

	/* stat.sock : ; */
	stat_read.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	PRINTF("> %s %d main() :Socket stat_read created\n",__FILE__ , __LINE__);
	if (stat_read.sock < 0) {
		perror("opening stream socket");
		exit(EXIT_FAILURE);
	}
	stat_read.server.sun_family = AF_UNIX;
	strcpy(stat_read.server.sun_path, StatSocket);

	/*bind socket to StatSocket*/
	if (bind(stat_read.sock, (struct sockaddr *)&stat_read.server, sizeof(struct sockaddr_un))) {
		perror("binding stream socket");
		exit(EXIT_FAILURE);
	}

	/* create socket for application registration*/
	app_reg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	PRINTF("> %s %d main() : Socket app_reg created\n",__FILE__ , __LINE__);
	if (app_reg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(EXIT_FAILURE);
	}
	app_reg.server.sun_family = AF_UNIX;
	strcpy(app_reg.server.sun_path, AppRegSocket);

	/*bind socket to AppRegSocket */
	if (bind(app_reg.sock, (struct sockaddr *)&app_reg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(EXIT_FAILURE);
	}

	/* create socket for application unregistration */
	app_unreg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (app_unreg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(EXIT_FAILURE);
	}
	app_unreg.server.sun_family = AF_UNIX;
	strcpy(app_unreg.server.sun_path, AppUnRegSocket);
	/*bind socket to AppUnRegSocket*/
	if (bind(app_unreg.sock, (struct sockaddr *)&app_unreg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(EXIT_FAILURE);
	}

	/* socket for np registration */
	np_reg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (np_reg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(EXIT_FAILURE);
	}
	np_reg.server.sun_family = AF_UNIX;
	strcpy(np_reg.server.sun_path, NpRegSocket);
	/*bind socket to NpRegSocket*/
	if (bind(np_reg.sock, (struct sockaddr *)&np_reg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(EXIT_FAILURE);
	}

	/* socket for np unregistration */
	np_unreg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (np_unreg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(EXIT_FAILURE);
	}
	np_unreg.server.sun_family = AF_UNIX;
	strcpy(np_unreg.server.sun_path, NpUnRegSocket);
	/*Bind socket to NpUnRegSocket*/
	if (bind(np_unreg.sock, (struct sockaddr *)&np_unreg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(EXIT_FAILURE);
	}

	/* create socket for app side getnotify */
	app_getnotify.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (app_getnotify.sock < 0) {
		perror("NJ.C   : opening stream socket for getnotify");
		exit(EXIT_FAILURE);
	}
	app_getnotify.server.sun_family = AF_UNIX;
	strcpy(app_getnotify.server.sun_path, AppGetnotifySocket);
	/*Bind socket to AppGetnotifySocket*/
	if (bind(app_getnotify.sock, (struct sockaddr *)&app_getnotify.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(EXIT_FAILURE);
	}

    fprintf(logfd, "%s %d main(): Sockets created and bound.\n",__FILE__ , __LINE__);
	force_logs();

	/* create threads to listen  and accept */
	if ((ret = pthread_create(&tid_stat, NULL, &print_stat_method, (void *)&stat_read)) == 0) {
		PRINTF("> %s %d main(): Pthread_Creation successful for stat\n",__FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for stat thread:");
	}

	if ((ret = pthread_create(&tid_app_reg, NULL, &app_reg_method, (void *)&app_reg)) == 0) {
		PRINTF(">%s %d main() : Pthread_Creation successful for app_reg_method \n",__FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for app_reg_method :");
	}
	if ((ret = pthread_create(&tid_app_unreg, NULL, &app_unreg_method, (void *)&app_unreg)) == 0) {
		PRINTF("> %s %d main()    : Pthread_Creation successful for app_unreg_method\n",__FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for app_unreg method:");
	}
	if ((ret = pthread_create(&tid_np_reg, NULL, &np_reg_method, (void *)&np_reg)) == 0) {
		PRINTF("> %s %d main() : Pthread_Creation successful np_reg_method\n",__FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for np_reg_method:");
	}
	if (( ret = pthread_create(&tid_np_unreg, NULL, &np_unreg_method, (void *)&np_unreg)) == 0) {
		PRINTF("> %s %d main() : Pthread_Creation successful np_unreg_method\n",__FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for np_unreg_mehtod :");
	}
	if (( ret = pthread_create(&tid_app_getnotify, NULL, &app_getnotify_method, (void *)&app_getnotify)) == 0) {
		PRINTF(">%s %d main(): Pthread_creation of getnotify thread successful app_getnotify_method\n",__FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for app_getnotify_method :");
	}
	
	fprintf(logfd, "%s %d main(): Threads created.\n",__FILE__ , __LINE__);
	force_logs();

	/*Join all the threads */
	if((ret = pthread_join(tid_np_reg, NULL)) != 0 ) {
		errno = ret;
		perror("nj.c : Error in pthread_join joining Np Reg thread:");

	}
	if((ret = pthread_join(tid_np_unreg, NULL)) != 0) {
		errno = ret;
		perror("nj.c : Error in pthread_join  joing Np unreg thread");
	}
	if((ret = pthread_join(tid_app_reg, NULL)) != 0) {
		errno = ret;
		perror("Error in pthread_join joining app register thread");
	}
	if((ret = pthread_join(tid_app_unreg, NULL)) != 0 ) {
		errno = ret;
		perror("Error in pthread_join joining app Unregister thread");
	}

	if((ret = pthread_join(tid_app_getnotify, NULL)) != 0 ) {
		errno = ret;
		perror("Error in pthread_join joining app_getnotify thread");
	}

    fprintf(logfd, "%s %d main(): Threads joined.\n",__FILE__ , __LINE__);
	force_logs();

	exit(EXIT_SUCCESS);

}

/* method to read socket for statistics */
void *print_stat_method(void *arguments)
{
	struct thread_args *args = arguments;

	/*Listen to requests on socket */
	if(listen(args->sock, QLEN) != 0 ) {
		perror("Error in listening on stat socket");
	}
	for (;;) { /*till the program ends*/
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror(" Error accept on stat socket");
		else
			/* For every argument that might be given */
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval = read(args->msgsock, args->buf, 1024)) < 0)
					perror("NJ.C   : reading stream message");
				else {	
					print_stat();
					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}
	close(args->sock);
	unlink(StatSocket);

	pthread_exit(NULL);
	return NULL;
}


/* Read the app_register socket; runs in the thread forked for the purpose and calls the appropriate function */
void *app_reg_method(void *arguments)
{
	struct thread_args *args = arguments;

	/*Listen for requests on socket*/
	if ((listen(args->sock, QLEN)) != 0) {
		perror("Error in listening on appreg socket:");
	}
	int i = 0;
	/* Till the program runs */
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : Error in accept app reg socket");
		else
			/* For every argument that might be given */
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval = read(args->msgsock, args->buf, 1024)) < 0)
					perror("NJ.C   : reading stream message");
				else if (args->rval == 0)
					PRINTF("> %s %d app_reg_method()   : Ending connection\n",__FILE__ , __LINE__);
				/*code to register application ie add entry in list */
				else {	
					i = register_app(args->buf);
					if (i < 0) {
						PRINTF("> %s %d app_reg_method(): Error in registering, try again\n",__FILE__ , __LINE__);
						perror("Error in app_reg_method() : ");
					}
					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(AppRegSocket);

	pthread_exit(NULL);
	return NULL;
}

/* app unregister method that will run in thread forked for unregistration and call the appropriate function */
void *app_unreg_method(void *arguments)
{
	struct thread_args *args = arguments;
	int i = 0;
	/*Listen for requests on socket*/
	if((listen(args->sock, QLEN)) != 0) {
		perror("Error in listening on app_unreg socket");
	}

	for (;;) {/*Till program ends*/
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : Error in accept of app_unreg socket");
		else
			do {	/*for each requestz*/
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
							read(args->msgsock, args->buf, 1024)) < 0)
					perror
						("NJ.C   : reading stream message");
				else if (args->rval == 0)
					PRINTF(">%s %d app_unreg_method()  : Ending connection\n",__FILE__ , __LINE__);
				/* Call the function for the list */
				else {
					i = unregister_app(args->buf);

					if (i < 0)	{
						PRINTF("> %s %d app_unreg_method(): Error in registering\n",__FILE__ , __LINE__);
						perror("Error in app_reg_method() : ");
					}					

					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(AppUnRegSocket);

	pthread_exit(NULL);
	return NULL;
}

/*np register method that will run in thread forked for np registration*/
void *np_reg_method(void *arguments)
{
	struct thread_args *args = arguments;

	/*Listen for requests on socket*/
	if((listen(args->sock, QLEN)) != 0) {
		perror("Error in listening on np_reg socket");
	}
	int i = 0;
	for (;;) {/*Till the program ends*/
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : Error in accept of np_reg socket");
		else
			do {/*For each request*/
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
							read(args->msgsock, args->buf, 1024)) < 0)
					perror
						("NJ.C   : reading stream message");
				else if (args->rval == 0)
					PRINTF("> %s %d np_reg_method(): Ending connection\n",__FILE__ , __LINE__);
				/*code to register np ie add entry in list */
				else {	
					i = register_np(args->buf);
					if (i < 0) {
						PRINTF(">%s %d np_reg_method(): Error in registering\n",__FILE__ , __LINE__);
						perror("Error in np_reg_method() : ");

					}
					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(NpRegSocket);

	pthread_exit(NULL);
	return NULL;
}

/*np unregister method that will run in thread forked for np unregistration*/
void *np_unreg_method(void *arguments)
{
	struct thread_args *args = arguments;
	/*Listen for requests on socket*/
	if((listen(args->sock, QLEN)) != 0) {
		perror("Error in listening on np Unreg socket");
	}
	int i = 0;
	for (;;) {/*Till program ends*/
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : Error in accept of np unreg socket");
		else
			do {
				/*for each request*/
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
							read(args->msgsock, args->buf, 1024)) < 0)
					perror("NJ.C   : reading stream message");
				else if (args->rval == 0)
					PRINTF(">%s %d np_unreg_method(): Ending connection\n",__FILE__ , __LINE__);
				/*code to unregister NP entry in list */
				else {
					i = unregister_np(args->buf);

					if (i < 0) {
						PRINTF("> %s %d np_unreg_method(): Error in registering\n",__FILE__ , __LINE__);
						perror("Error in np_unreg_method() : ");
					}
					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(NpUnRegSocket);

	pthread_exit(NULL);
	return NULL;
}

/* app get notify method that will run in thread for get notification */
void *app_getnotify_method(void *arguments)
{
	struct thread_args *args = arguments;
	struct proceed_getn_thread_args *sendargs;
	struct sockaddr_un server;
	int i = 0;
	char choice;
	int ret, len;


	fprintf(logfd,"> %s %d app_getnotify_method(): In AppGetnotifySocket\n",__FILE__ , __LINE__);
	fprintf(logfd,"> %s %d AppGetNotifyMehod(): args -> msgsock - %d\n",__FILE__ , __LINE__, args->msgsock);
	/*Listen for requests on socket*/
	if((listen(args->sock, QLEN)) != 0) {
		perror("Error in listening on app_getnotify socket");
	}

	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, AppGetnotifySocket);
	pthread_t threadarr[APPLIMIT];

	for (;;) {/*Till program ends*/
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : Error in accept of app_getnotify_socket");
		else
			do {
				/*For each request*/
				if((pthread_mutex_lock(&getnotify_socket_mutex)) != 0 ) {
					perror("Error in lock getnotify_socket_mutex:");
				}
				if((sendargs = (struct proceed_getn_thread_args *)malloc(sizeof(struct proceed_getn_thread_args))) == NULL) {
					PRINTF("> %s %d app_getnotify_method(): malloc failed\n ",__FILE__, __LINE__);
					errno = ECANCELED;
					perror("Malloc failed");
				}

				bzero(args->buf, sizeof(args->buf));
				args->rval = read(args->msgsock, args->buf, 1024);
				strcpy(sendargs->buf, args->buf);
				fprintf(logfd, ">%s %d app_getnotify_method() : 1. buf is %s \n",__FILE__ , __LINE__,sendargs->buf);

				if (args->rval < 0) { /*Error case*/
					perror("NJ.C   : reading stream message");

					if((pthread_mutex_unlock(&getnotify_socket_mutex)) != 0) {
						perror("Error in unlock getnotify_socket_mutex:");
					}
				}    
				else if (args->rval == 0) { /*Nothing read*/
					PRINTF("> %s %d app_getnotify_method(): Ending connection\n",__FILE__ , __LINE__);
					if((pthread_mutex_unlock(&getnotify_socket_mutex)) != 0) {
						perror("Error in unlock getnotify_socket_mutex:");
					}
				}
				/* code to create a thread per getnotify() request and send the arguments to the thread's action */
				else {
					PRINTF("> %s %d app_getnotify_method(): %d is i in ProcerdGetnotify \n",__FILE__ ,__LINE__,i);
					// Get the choice here. 
					len = strlen(sendargs->buf);
					choice = sendargs->buf[len - 1];
					if(choice == NONBLOCKING) { /*for Nonblocking app_getnotify calls */
						if ((ret = pthread_create(&threadarr[i++], NULL, &proceed_getnotify_method, (void *)sendargs)) == 0) {
							PRINTF("> %s %d app_getnotify_method(): Pthread_Creations for proceed_getnotify_method\n",__FILE__, __LINE__);
						}
						else {
							errno = ret;
							perror("Error in pthread_create creating thread for proceed_getnotify_method");
						}
					}
					else if(choice == BLOCKING) {/*for Blocking app_getnotify calls */
						if ((ret = pthread_create(&threadarr[i++], NULL, &block_getnotify_method, (void *)sendargs)) == 0) {
							PRINTF("> %s %d app_getnotify_method(): Pthread_Creations for block_getnotify_method\n",__FILE__, __LINE__);
						}
						else {
							errno = ret;
							perror("Error in pthread_create creating thread for block_getnotify_method");
						}
					}
					if((pthread_mutex_unlock(&getnotify_socket_mutex)) != 0) {
						perror("Error in unlock getnotify_socket_mutex:");
					}

					break;
				}

			}while(args->rval > 0);

		close(args->msgsock);
	}
	while (i >= 0) { /*Join all the threads created in above block */

		if((ret = pthread_join(threadarr[i], NULL)) != 0) {
			errno = ret;
			perror("Error in pthread_join joining threads created by app_getnotify Method:");
		}
		i--;
	}

	close(args->sock);
	unlink(AppUnRegSocket);

	pthread_exit(NULL);
}

/*np get notify method that will run np get notify thread*/
void *np_getnotify_method(void *arguments)
{
	char args_send_copy[1024], args_send_copy_2[1024], rough[1024], r[1024], appname[64], np_name[64], one[512], library[256];
	char delimattr[3] = "##", delimval[3] = "::";
	char *error, *countkey, *filename;
	char **pointer;    	
	int j, filefd, al, count = -1, k, x;
	struct getnotify_thread_args *bargs, *args; 
	struct extr_key_val *temp, *m, *p;
	np_node *nptr;
	main_np_node *np_node;	
	int pid;

	bargs = (struct getnotify_thread_args *)arguments;
	if((args = (struct getnotify_thread_args *)malloc(sizeof(struct getnotify_thread_args))) == NULL) {
		PRINTF("> %s %d np_getnotify_method() : malloc failed\n", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");
	}

	*args = *bargs;

	fprintf(logfd, "> %s %d np_getnotify_method : 8. buf is %s \n",__FILE__ , __LINE__, args->argssend);
	void (*getnotify) (struct getnotify_thread_args *); /*call np specific function here*/


	PRINTF("> %s %d np_getnotify_method(): started args->argssend is %s\n",__FILE__ , __LINE__, args->argssend);
	strcpy(rough, args->argssend);
	strcpy(r, rough);

	strcpy(args_send_copy_2, args->argssend);

	filename = get_filename(args_send_copy_2);
	
	fprintf(logfd, "%s %d np_getnotify_mehod(): Buffer received - %s \n",__FILE__ , __LINE__, args->argssend);
	force_logs();

	strtok(r, delimattr);
	j = strlen(r);
	strcpy(r, args->argssend);
	strcpy(r, &(r[j + 2]));
	strcpy(args->argssend, r);

	/* Extract app_name and np_name from the arguments */
	strcpy(appname, strtok(rough, delimattr));

	pid = atoi(appname);


	fprintf(logfd, "> %s %d np_getnotify_method():77. pid is %d \n",__FILE__ , __LINE__, pid);


	strcpy(one, strtok(NULL, delimattr));
	strtok(one, delimval);	
	strcpy(np_name, strtok(NULL, delimval));

	/* Compare it with the arguments given during ./np_register to check the consistency */

	extract_key_val(args->argssend, &pointer);


	if(DATASTRUCT == LIST ) { /*when List datastructure used*/

		np_node = search_np(&npList, np_name);
		if(np_node != NULL) { /*node found case*/
			k = compare_array(&(np_node->key_val_arr), &pointer);
			if(k != 0) {
				PRINTF(">%s %d np_getnotify_method() :Array not matched..\n",__FILE__ , __LINE__);	
				errno = EINVAL;
				return NULL;
			}
		}
		else { /*np not registerd with nj case*/
			PRINTF(">%s %d np_getnotify_method() :NP not found in list..\n",__FILE__ , __LINE__);	
			errno = EINVAL;
			return NULL;
		}
		nptr = get_reg_list(&appList, appname, np_name);

		if (nptr) {
			PRINTF("\n> %s %d np_getnotify_method(): RETURNED nptr->name in list = %s\n",__FILE__ , __LINE__, nptr->name);
		}

		else {
			PRINTF(">%s %d np_getnotify_method():Regitration not found in list---------\n\n",__FILE__ , __LINE__);
			perror("Registration doesn't exist - ");
			/*and exit*/
			errno = ENODEV;
		}



	}
	/*search hash.*/
	if(DATASTRUCT == HASH ) { /*when Hash Datastructure used*/
		HASH_FIND_STR(hstruct_np->np_hash, np_name, np_node); 
		if(np_node != NULL) {
			k = compare_array(&(np_node->key_val_arr), &pointer);
			if(k != 0) {  /*np found case*/
				PRINTF(">%s %d np_getnotify_method() :Array not matched..\n",__FILE__ , __LINE__);	
				errno = EINVAL;
				return NULL;
			}
		}
		else { /*np not found case*/
			PRINTF(">%s %d np_getnotify_method() :NP not found in hash..\n",__FILE__ , __LINE__);	
			errno = EINVAL;
			return NULL;
		}

		nptr = get_reg_hash(hstruct_app, appname, np_name);

		if (nptr) { 
			PRINTF("\n> %s %d np_getnotify_method(): RETURNED in hash nptr->name = %s\n",__FILE__ , __LINE__, nptr->name);
		}

		else {
			PRINTF(">%s %d np_getnotify_method():Regitration not found in hash---------\n\n",__FILE__ , __LINE__);
			perror("Registration doesn't exist - ");
			//and exit
			errno = ENODEV;
		}




	}

	if((temp = (struct extr_key_val *)malloc(sizeof(struct extr_key_val))) == NULL) {
		PRINTF("> %s %d np_getnotify_method(): malloc failed \n", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");
		/* For every malloc() handling */
	}

	temp->next = NULL;

	/* In the registration in the trailing list, append to a list of key-value pairs for each getnotify() call */

	m = nptr->key_val_ptr;
	p = m;


	if (m == NULL) { /*first np case*/
		nptr->key_val_ptr = temp;
	} else {
		while (m != NULL) { /*other than first np case*/
			p = m; 
			m = m->next;
		}
		p->next = temp;
		temp->next = NULL;  
	}


	/* Set the pointers in the trailing list and convert the key-value array into an NP accepted format */

	temp->key_val_arr = pointer;
	forward_convert(&(np_node->key_val_arr), &pointer, args->argssend);
	if(DATASTRUCT == LIST) {
		x = get_np_app_cnt(&npList, np_name);
	}
	if(DATASTRUCT == HASH) {
		x = get_np_app_cnt_hash(hstruct_np, np_name);
	}
	/* Get the reg, malloc space for a getn registration key_val  */

	countkey = get_val_from_args(args->argssend, "count");
	count = atoi(extract_val(countkey));
	fprintf(logfd, ">%s %d np_getnotify_method() : count is %d! for buf %s!\n",__FILE__ , __LINE__, count, args->argssend);

	/* x = 0 for no registrations and x > 1 for existing registrations. When getnotify() is called for the first time (x = 1), the library needs to be opened. */

	strcpy(library, "./libnp");
	strcat(library, np_name);
	strcat(library, ".so");

	if (x == 1) {
		PRINTF(">%s %d np_getnotify_method(): Opening library.\n",__FILE__ , __LINE__);
		handle = dlopen(library, RTLD_LAZY); /*open the library specified*/
		if (!handle) {
			perror("NJ.C   : Problem with dlopen :");
			exit(EXIT_FAILURE);
		}

		PRINTF("> %s %d np_getnotify_method (): dlopen successful\n",__FILE__ , __LINE__);

	}

	getnotify = dlsym(handle, "getnotify");
	if ((error = dlerror()) != NULL) {
		perror("NJ.C   : Error in dlsym()");
		exit(EXIT_FAILURE);
	}


	strcpy(args_send_copy, args->argssend);	

	/* For the number of times the same arguments have to be monitored;
	 *	Open the file to prevent delayed write induced errors,
	 *	call getnotify() from the NP library
	 *	copy the notification returned,
	 *	send signal to the application,
	 *	close the file
	 */
	for(;count != 0; count--) {

		if (!(filefd = open(filename, O_CREAT | O_APPEND | O_RDWR , 0777))) {
			perror("NJ.C   : not able to open file\n");
			return NULL;
		}

		fprintf(logfd, "%s %d np_getnotify_method(): count - %d and buffer - %s \n",__FILE__ , __LINE__, count, args->argssend);
		force_logs();
		
		(*getnotify) (args);
		PRINTF(">%s %d np_getnotify_method(): Recd = %s\n",__FILE__ , __LINE__, args->argsrecv);

		al = write((int)filefd, args->argsrecv, strlen(args->argsrecv));
		if (al < 0)
			perror("NJ.C   : Fwrite failed");
		else	{
			PRINTF(">%s %d np_getnotify_method(): %s written  %d bytes WRITTEN\n",__FILE__ , __LINE__, filename, al);
		}
		close(filefd);

		strcpy(args->argssend, args_send_copy);

		if (kill(pid, SIGUSR1) == -1) { /*Send signal to pid of application alerting receival of Notification*/
			perror("Error in kill :\n");
		} 

	}

	PRINTF("> %s %d np_getnotify_method(): ARGSRECV IN THREAD HANDLER = %s\n",__FILE__ , __LINE__, args->argsrecv);
	strcpy(bargs->argsrecv, args->argsrecv);


	free(args);
	args = NULL;


	return NULL;

}
/*thread method for blocking getnotify calls*/
void *block_getnotify_method(void *argu) {

	int sock_block;
	struct sockaddr_un server_block;

	char *buf;
	struct proceed_getn_thread_args *temparguments, *args;
	char rough[1024];
	char sock_name[32];


	temparguments = (struct proceed_getn_thread_args *)argu;
	if((args = (struct proceed_getn_thread_args *)malloc(sizeof(struct proceed_getn_thread_args))) == NULL) {
		PRINTF("> %s %d block_getnotify_method(): malloc failed\n",__FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}

	*args = *temparguments;
	fprintf(logfd, "> %s %d block_getnotify_method():2. buf is %s \n",__FILE__ , __LINE__, args->buf);
	strcpy(rough, args->buf);

	if(args->buf == NULL)
		PRINTF("> %s %d block_getnotify_method(): args->buf is NULL\n\n",__FILE__ , __LINE__);

	/* Extract pid */
	strcpy(sock_name, strtok(rough, "##"));

	strcat(sock_name, "_sock");

	printf("sock_name in NJ is %s\n", sock_name);



	fprintf(logfd, "%s %d block_getnotify_method : Buffer : %s \n",__FILE__ , __LINE__, args->buf);  
	force_logs();

	if((buf = (char *)malloc(sizeof(char) * (strlen(args->buf) + 1 ))) == NULL ) {
		PRINTF("> %s %d block_getnotify_method(): malloc failed\n", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");
	}
	strcpy(buf, args->buf);

	char *notification;
	struct getnotify_thread_args *arguments;

	if((notification = (char *)malloc(1024 * sizeof(char))) == NULL) {
		PRINTF("> %s %d block_getnotify_method(): malloc failed",__FILE__,__LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	if((arguments = (struct getnotify_thread_args *)malloc(sizeof(struct getnotify_thread_args))) == NULL) {
		PRINTF("> %s %d block_getnotify_method() : malloc failed ", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	strcpy(arguments->argssend, buf);


	fprintf(logfd, "7. buf is %s \n", arguments->argssend);


	/*Call function np_getnotify_method instead of creating its separate thread as in non-blocking case*/
	np_getnotify_method(arguments);

	strcpy(notification, arguments->argsrecv);

	printf("NJ will write blocking notification as - %s\n", notification);

	sock_block = socket(AF_UNIX, SOCK_STREAM, 0);
	server_block.sun_family = AF_UNIX;
	strcpy(server_block.sun_path, sock_name);
	/*Connect to socket with name pid*/
	if (connect
			(sock_block, (struct sockaddr *)&server_block,
			 sizeof(struct sockaddr_un)) < 0) {
		close(sock_block);
		perror("block_getnotify_method(): ERROR CONNECTING STREAM SOCKET :");
		exit(1);
	}

	printf("NJ writing blocking notification as - %s\n", notification);
	/*Write received notification to socket with name pid*/
	if (write(sock_block, notification, sizeof(notification)) < 0)
		perror
			("block_getnotify_method() : ERROR WRITING COMMAND ON STREAM SOCKET :");

	free(buf);
	buf = NULL;

	free(arguments);
	arguments = NULL;

	return NULL;

}

/* Find the app in the list. If it is found, for every np in its trailing list; visit the np_dcll and decrement its count. */
void dec_all_np_counts(app_dcll * appList, np_dcll * npList, char *app_name)
{

	app_node *ptrapp;
	np_node *ptrnp;
	int cnt;

	PRINTF("> %s %d dec_all_np_counts() :All nps deleted, since np_name was passed NULL\n",__FILE__ , __LINE__);
	ptrapp = search_app(appList, app_name);

	if (ptrapp != NULL) {	/* App found */

		cnt = ptrapp->np_count;
		ptrnp = ptrapp->np_list_head;
		while (cnt) { /*for each np with which app is registered*/
			decr_np_app_cnt(npList, ptrnp->name);
			ptrnp = ptrnp->next;
			cnt--;
		}

	} else { /*App not found in list*/

		PRINTF("> %s %d dec_all_np_counts():Error Asked to delete non-existent application\n",__FILE__ , __LINE__);
		perror("App doesn't exist - ");

	}

}

/*It decrements all np counts in hash table with which app_name was registered */ 
void dec_all_np_counts_hash(hash_struct_app *hstruct_app, hash_struct_np *hstruct_np, char *app_name) {

	app_node *ptrapp;
	np_node *ptrnp;
	int cnt;

	PRINTF("> %s %d dec_all_np_counts_hash() :All nps deleted, since np_name was passed NULL\n",__FILE__ , __LINE__);

	HASH_FIND_STR(hstruct_app->app_hash, app_name, ptrapp);	
	if (ptrapp != NULL) {	/* App found */
		cnt = ptrapp->np_count;
		ptrnp = ptrapp->np_list_head;
		while (cnt) { /*for each np with which app is registered*/
			decr_np_app_cnt_hash(hstruct_np, ptrnp->name);
			ptrnp = ptrnp->next;
			cnt--;
		}

	} else { /*App not found case*/

		PRINTF("> %s %d dec_all_np_counts():Error Asked to delete non-existent application\n",__FILE__ , __LINE__);
		perror("App doesn't exist - ");

	}

}

/* Action function of the non-blocking getnotify() threads */
void *proceed_getnotify_method(void *arguments)
{
	char *received, *buf;
	struct proceed_getn_thread_args *temparguments, *args;
	char rough[1024];
	char spid[32];
	int j;	
	int len;
	char filename[64], filename1[64];


	temparguments = (struct proceed_getn_thread_args *)arguments;
	if((args = (struct proceed_getn_thread_args *)malloc(sizeof(struct proceed_getn_thread_args))) == NULL) {
		PRINTF("> %s %d proceed_getnotify_method(): malloc failed\n",__FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}

	*args = *temparguments;
	
	fprintf(logfd, "%s %d proceed_getnotify_method(): Buffer : %s \n",__FILE__ , __LINE__, args->buf);
	force_logs();

	strcpy(rough, args->buf);

	if(args->buf == NULL)
		PRINTF("> %s %d proceed_getnotify_method(): args->buf is NULL\n\n",__FILE__ , __LINE__);

	len = strlen(rough);

	/* Extract pid */
	strcpy(spid, strtok(rough, "##"));
	for(j = 0; j < len ; j++) {
		if(rough[j] == '#')
			if(rough[j+1] == '#')
				break;
	}

	j = strlen(rough);
	/*Create string for filename pid.txt*/
	strcpy(filename, "./");
	strcat(filename, spid);
	strcpy(filename1, spid);
	strcat(filename, ".txt");
	strcat(filename1, ".txt");
	strcat(filename1, "\n");

	fprintf(logfd, "%s %d proceed_getnotify_method : Buffer - %s \n",__FILE__ , __LINE__, args->buf);  
	force_logs();

	if((buf = (char *)malloc(sizeof(char) * (strlen(args->buf) + 1 ))) == NULL ) {
		PRINTF("> %s %d proceed_getnotify_method(): malloc failed\n", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");
	}
	strcpy(buf, args->buf);

	/* Subsequent calls to return the notification to "received" */
	received = getnotify_app(buf);

	while(received == NULL) {
		pthread_exit(NULL);
	}

	fprintf(logfd, "%s %d proceed_getnotify_method() : Notification received from getnotify_app - %s \n",__FILE__ , __LINE__, received); 
	force_logs();

	strcat(received, "\n");
	write((int)fd_pidnames, filename1, strlen(filename1));
	PRINTF("> %s %d proceed_getnotify_method(): PID filename is written successfully.\n",__FILE__ , __LINE__);

	free(received);
	received = NULL;
	free(temparguments);
	temparguments = NULL;
	free(args);
	args = NULL;

	pthread_exit(NULL);
}

/* Given key::val, get the key */
char* extract_key(char *key_val) {
	char *key, *ptr;
	char temp[128];

	strcpy(temp, key_val);
	ptr = strtok(temp, "::");
	if((key = (char *)malloc(sizeof(char) * (strlen(ptr) + 1))) == NULL) {
		PRINTF("> %s %d extract_key() : malloc failed \n", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	strcpy(key, ptr);
	return key;
}

/* Given key::val, extract the val */
char* extract_val(char *key_val) {

	char *ptr, *val;
	char temp[128] ;

	strcpy(temp, key_val);
	ptr = strtok(temp, "::");
	ptr = (key_val + strlen(ptr) + 2);
	if(!ptr) {
		PRINTF(">%s %d extract_val:RETURNED NULL",__FILE__ , __LINE__);
		errno = ENODEV;
		return NULL;
	}
	if((val = (char *)malloc(sizeof(char) * (strlen(ptr) + 1))) == NULL) {
		PRINTF("> %s %d extract_val(): malloc failed\n", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	strcpy(val, ptr);
	return val;
}

/* Compare keys in two arrays to check their validity, assume that keys given during ./np_register are correct */
int compare_array(char *** np_key_val_arr, char *** getn_key_val_arr) {
	char **one, **two, *key_one, *key_two;
	int found = 0;

	one = *getn_key_val_arr;
	while(*one != NULL) { /*Check for each key in getn_key_val_arr*/
		found = 0;
		key_one = extract_key(*one);
		two = *np_key_val_arr;
		while(*two != NULL) { /*check whether one is present in np_key_val_arr*/
			key_two = extract_key(*two);
			if(!(strcmp(key_one, key_two))) {
				found = 1;
				break;
			}
			two++;
		}
		if(found == 0) { /*if key in getn_key_val_arr not found*/
			PRINTF("> %s %d compare_array():ERROR NP cannot process key %s\n",__FILE__ , __LINE__, key_one);
			errno = EINVAL;
			free(key_one);
			free(key_two);
			return -1;
		}
		one++;
	}
	free(key_one);
	free(key_two);
	return 0;
}

/* Convert arguments from a char** array into a string */
void forward_convert(char ***np_key_val_arr,char ***getn_key_val_arr , char * fillit) {
	char **one, **two, *key_one, *key_two;
	int found = 0;
	char ret_string[512];

	strcpy(ret_string,"\0");
	two = *np_key_val_arr;
	while(*two != NULL) { /*for each key::val string in the np_key_val_arr*/
		found = 0;
		key_two = extract_key(*two);
		one = *getn_key_val_arr;
		while(*one != NULL) { /*check if val for above two present in getn_key_val_arr*/
			key_one = extract_key(*one);
			if(!(strcmp(key_one, key_two))) {
				strcat(ret_string, *one);
				strcat(ret_string, "##");
				found = 1;
				break;
			}
			one++;
		}
		if(found == 0) { /*put the default value that was provided during np registration*/
			strcat(ret_string, *two);
			strcat(ret_string, "##");
		}
		two++;
	}
	strcpy(fillit, ret_string);
	fillit[(strlen(fillit)) - 2] = '\0';
	return;
}

/* Given an argument string and a key, get the key::value corresponding to the key */
char* get_val_from_args(char *usage, char* key) {   
	char *occ, keycopy[128], *retstr, localkeyval[256];
	int i = 0;

	strcpy(keycopy, key);
	strcat(keycopy, "::");
	occ = strstr(usage, keycopy);
	while(*occ != '#') {
		localkeyval[i++] = *occ;
		occ++;    
	}
	localkeyval[i] = '\0';

	if((retstr = (char *)malloc(sizeof(char) * i)) == NULL) {
		PRINTF("> %s %d get_val_from_args() : malloc failed ", __FILE__, __LINE__);
	}

	strcpy(retstr, localkeyval);
	return retstr;
}

/* From an argument, extract thepid and make a filename from it */    
char *get_filename(char *argsbuf) {

	char rough[1024], *retstr;
	char spid[32], filename[64];

	strcpy(rough, argsbuf);
	strcpy(spid, strtok(rough, "##"));
	strcpy(filename, "./");
	strcat(filename, spid);
	strcat(filename, ".txt");
	PRINTF("> %s %d get_filename(): Filename:%s\n\n",__FILE__ , __LINE__, filename);
	if((retstr = (char*)malloc(sizeof(char) * (strlen(filename) + 1))) == NULL) {
		PRINTF("> %s %d get_filename(): malloc failed\n",__FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	strcpy(retstr, filename);
	return retstr;
}

/* Function to forcefully write to log file */
void force_logs(void) {
	fclose(logfd);
	if (!(logfd = fopen(LOGS, "a+"))) {
		perror("NJ.C : Unable to open Log file\n");
	}
	return;
}


/*prints np_list on socket for stats */
void print_list_on_sock_np(np_dcll *l) {

	int i = l->count;
	main_np_node *ptr;
	char buf[512];
	char **kptr;
	/*get read lock on list*/
	pthread_rdwr_rlock_np(&(l->np_list_lock));
	if (l->count == 0) {
		sprintf(buf, "> %s %d print_np() : NP_DCLL  : Nothing to print_np\n", __FILE__, __LINE__);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) {
			perror("STAT : Reading On Stream Socket"); 
		}
		pthread_rdwr_runlock_np(&(l->np_list_lock));

		return;
	}

	sprintf(buf, "\n> %s %d print_np() : Total number of NPs : %d\n", __FILE__, __LINE__, l->count);
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 
	ptr = l->head;
	kptr = ptr->key_val_arr;

	sprintf(buf, "\n> %s %d print_np() : NP\t\t\tApp_Count\t\t\tKeyValue", __FILE__, __LINE__);
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 
	sprintf(buf, "\n==================================================\n");
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 

	while (i) { /*for each node in list*/
		sprintf(buf, "> %s %d print_np() : %s   ", __FILE__, __LINE__, ptr->data);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		sprintf(buf, "\t\t\t%d\n", ptr->app_count);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 

		while (*kptr) { /*for each key::val string*/
			sprintf(buf, "\t\t\t\t%s\n", *kptr);
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 
			kptr++;
		}

		sprintf(buf, "\n");
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		ptr = ptr->next;
		i--;
	}
	sprintf(buf, "====================================xx======================================\n");
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 
	pthread_rdwr_runlock_np(&(l->np_list_lock));

}

/*prints app_list on socket for stats */
void print_list_on_sock_app(app_dcll *l) {
	int i;
	char buf[512];
	app_node *ptr;
	np_node *np;
	/*Acquire read lock on list*/
	pthread_rdwr_rlock_np(&(l->app_list_lock));	
	i = l->count;	
	if(i == 0) {/*App list empty case*/
		sprintf(buf, "> app_dcll.c print_app() : NO APPS TO PRINT\n");
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		pthread_rdwr_runlock_np(&(l->app_list_lock));	
		return;
	}

	ptr = l->head;

	sprintf(buf, "\n> %s %d print_app() : Total number of applications : %d\n",__FILE__, __LINE__, l->count);
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 
	sprintf(buf, "\n> %s %d print_app() :App\t\t\tNp_Count", __FILE__, __LINE__);
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 
	sprintf(buf, "\n===================================\n");
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 
	while (i) { /* While applications in the list are being traversed, print the contents of each application node */

		sprintf(buf, "%s\t\t\t", ptr->data);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		sprintf(buf, "%d\n", ptr->np_count);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		print_np_key_val_stat(ptr);
		sprintf(buf, "\n\n\n");
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		ptr = ptr->next;
		i--;
	}

	/* Print the registrations of each application */


	i = l->count;
	ptr = l->head;
	if(i != 0) {
		sprintf(buf, "\n> %s %d print_app() :App\t\t\tRegistered with", __FILE__, __LINE__);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		sprintf(buf, "\n===================================\n");
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
	}


	while (i) { 	/* While each application is being traversed */
		np = ptr->np_list_head;
		sprintf(buf, "\n%s\t\t\t", ptr->data);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 

		while (np != NULL) { /* While it's trailing np list is being traversed */
			sprintf(buf, "%s\n", np->name);
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 
			np = np->next;
			sprintf(buf, "\t\t\t");
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 
		}

		ptr = ptr->next;
		i--;
	}
	pthread_rdwr_runlock_np(&(l->app_list_lock));	
}

/*prints the np_key_val of all nps with which temp is registered*/
void print_np_key_val_stat(app_node * temp) {
	np_node *head = temp->np_list_head;
	extr_key_val *vptr;
	char **kptr;
	char buf[512];

	if(!head) { /*empty list*/
		sprintf(buf, "\n> %s %d print_np_key_val() : No head in temp list\n\n", __FILE__, __LINE__);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		return;
	}

	sprintf(buf, "> %s %d print_np_key_val() : HEAD DATA = %s\n", __FILE__, __LINE__, head->name);
	if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
		perror("STAT : Reading On Stream Socket"); 

	while (head) { /*for each np with which app_node temp is registered*/
		vptr = head->key_val_ptr;

		if(vptr == NULL) { /*no np_node*/
			sprintf(buf, "> %s %d print_np_key_val() : VPTR IS NULL\n", __FILE__, __LINE__);
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 
			head = head->next;
			continue;
		}

		while (vptr) {
			kptr = vptr->key_val_arr;

			if(kptr == NULL) { /*no key::val pair */
				sprintf(buf, "> %s %d print_np_key_val() : The key_val_arr is NULL\n\n", __FILE__, __LINE__);
				if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
					perror("STAT : Reading On Stream Socket"); 
				return;
			}

			sprintf(buf, "> %s %d print_np_key_val() : *KPTR is %s\n",__FILE__, __LINE__, *kptr);
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 

			while (*kptr) { /*for each key::val pair*/
				sprintf(buf, "\t\t\t\t%s\n", *kptr);
				if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
					perror("STAT : Reading On Stream Socket"); 
				kptr++;
			}

			vptr = vptr->next;
		}
		head = head->next;
	}
	return;
}

/* Function To Print Statistics Of Nj */
void print_stat()
{

	stat_write.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (stat_write.sock < 0) {
		perror("STATS : Opening Stream Socket");
		exit(1);
	}
	stat_write.server.sun_family = AF_UNIX;
	strcpy(stat_write.server.sun_path, StatSocketPrint);
	/*connect to socket with name as pid of app*/
	if (connect
			(stat_write.sock, (struct sockaddr *)&(stat_write.server),
			 sizeof(struct sockaddr_un)) < 0) {
		close(stat_write.sock);
		perror("STATS : Connecting Stream Socket");
		exit(1);
	}

	if(DATASTRUCT == LIST) {
		print_list_on_sock_np(&npList);
		print_list_on_sock_app(&appList);
	}

	if(DATASTRUCT == HASH) {
		print_hash_on_sock_np(hstruct_np);
		print_hash_on_sock_app(hstruct_app);
	}
	unlink(StatSocketPrint);
	close(stat_write.sock);
}

/*prints np hash table on socket for stats */
void print_hash_on_sock_np(hash_struct_np *hstruct_np) {

	printf("PRINTING NP HASH\n\n\n");
	struct main_np_node *s;
	char buf[512];

	for(s=hstruct_np->np_hash; s != NULL; s=(struct main_np_node*)(s->hh.next)) {
		sprintf(buf, "NP Name : %s\n", s->data);
		printf("%s", buf);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
	}
	printf("\n");

}
/*prints app hash table on socket for stats */
void print_hash_on_sock_app(hash_struct_app *hstruct_app) {

	printf("PRINTING APP_HASH\n\n\n");

	struct app_node *s, *ptr;
	np_node *np;
	char buf[512];
	for(s=hstruct_app->app_hash; s != NULL; s=(struct app_node*)(s->hh.next)) {
		sprintf(buf, "APP Name : %s\n", s->data);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket");
		sprintf(buf, "\n> %s %d print_app() :App\t\t\tRegistered with", __FILE__, __LINE__);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 
		sprintf(buf, "\n===================================\n");
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 


		/* While each application is being traversed */
		ptr = s;
		np = ptr->np_list_head;
		sprintf(buf, "\n%s\t\t\t", ptr->data);
		if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
			perror("STAT : Reading On Stream Socket"); 

		/* While it's trailing np list is being traversed */

		while (np != NULL) {
			sprintf(buf, "%s\n", np->name);
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 
			np = np->next;
			sprintf(buf, "\t\t\t");
			if (write(stat_write.sock, buf, sizeof(buf)) < 0) 
				perror("STAT : Reading On Stream Socket"); 
		}	

	}
	printf("\n");

}



/* Signal Handler Of Nj */
void sigint_handler(int signum)
{
	PRINTF("> %s %d sigint_handler() : In sigIntHandlere\n", __FILE__, __LINE__);
	if (signum == SIGINT) {				/* Handling SIGINT signal to remove PID files */
		exit(EXIT_SUCCESS);
	}
}

/*To Register App With Nj*/
int register_app(char *buff)
{
	char app_name[32], np_name[32], delim[3] = "::";
	char *s;
	int retval;
	main_np_node *s_np;	


	strcpy(app_name, strtok(buff, delim));
	s = strtok(NULL, delim);

	if (s != NULL) /*when np_name provided*/
		strcpy(np_name, s);
	else  /*when np_name not provided*/
		np_name[0] = '\0';

	if(DATASTRUCT == LIST) {
		if(np_name[0] != '\0') {	
			if (search_np(&npList, np_name) == NULL) { /* NP is not registerd */
				perror("Np not registered - ");
				PRINTF("> %s %d register_app(): Np not registered. Register NP first.\n", __FILE__ , __LINE__);
				errno = ENODEV;
				return -1;
			}	
		}
		retval = add_app_ref(&appList, app_name, np_name);
		if(retval == -1) {
			printf("> %s %d register_app: Error in adding app_node", __FILE__, __LINE__);
			return -1;
		}
		incr_np_app_cnt(&npList, np_name);
		print_app(&appList);
	}

	if(DATASTRUCT == HASH) {
		if(np_name[0] != '\0') {
			HASH_FIND_STR(hstruct_np->np_hash, np_name, s_np);
			if(s_np == NULL) {	/*Np is not registered*/
				perror("Np not registered - ");
				PRINTF("> %s %d register_app(): Np not registered. Register NP first.\n", __FILE__ , __LINE__);
				errno = ENODEV;
				return -1;
			}	
		}
		retval = add_app_ref_hash(hstruct_app, app_name, np_name);
		if(retval == -1) {
			printf("> %s %d register_app: Error in adding app_node", __FILE__, __LINE__);
			return -1;
		}
		incr_np_app_cnt_hash(hstruct_np, np_name);
		print_hash_app(hstruct_app);
		print_hash_np(hstruct_np);
	}
	return 1;
}
/*Function To Unregister An App*/
int unregister_app(char *buff)
{
	char app_name[32], np_name[32], delim[3] = "::";
	char *np_ptr;
	app_node* temp;
	int retval;
	main_np_node *s_np;

	strcpy(app_name, strtok(buff, delim));
	np_ptr = strtok(NULL, delim);

	/* If there is an np's name given */
	if (np_ptr != NULL) {
		strcpy(np_name, np_ptr);
	}
	else np_name[0] = '\0';


	if(DATASTRUCT == LIST) {
		temp = search_app(&appList, app_name);

		if(temp == NULL) { /*app not registered*/
			perror("Application does not exist :");
			errno = ENODEV;
			return -1;
		}

		/* Only appname is given */
		if (np_ptr == NULL) {
			PRINTF("> %s %d unregister_app() :np_name == NULL case in unregister app\n", __FILE__ , __LINE__);
			dec_all_np_counts(&appList, &npList, app_name);
			if(del_app_ref(&appList, temp, NULL) == -1) {
				perror("Unregister Application Error : ");       
			}
			PRINTF("> %s %d unregister_app():Unregistration done\n", __FILE__, __LINE__);
		}
		/* Given appname::npname */
		else {
			/* If the registration exists, delete it */
			if(search_np(&npList, np_name) == NULL) {
				errno =EINVAL;
				return -1;
			}
			else {
				PRINTF("> %s %d unregister_app(): REGISTRATION FOUND.\n", __FILE__ , __LINE__);
				decr_np_app_cnt(&npList, np_name);
				if(del_app_ref(&appList, temp, np_name) == -1) {
					perror("Unregister Application Error : ");       
				}
			}
		}

		print_app(&appList);
		print_np(&npList);
	}

	if(DATASTRUCT == HASH) {

		if(np_name[0] != '\0') {
			HASH_FIND_STR(hstruct_np->np_hash, np_name, s_np);
			if(s_np == NULL) {	
				perror("Np not registered - ");
				PRINTF("> %s %d register_app(): Np not registered. Register NP first.\n", __FILE__ , __LINE__);
				errno = ENODEV;
				return -1;
			}	
		}
		retval = del_app_ref_hash(hstruct_app, app_name, np_name);
		if(retval == -1) {
			printf("> %s %d unregister_app: Error in deleting app_node", __FILE__, __LINE__);
			return -1;
		}
		print_hash_app(hstruct_app);
		print_hash_np(hstruct_np);
	}

	return 1;
}

/*function to register an np*/
int register_np(char *buff)
{
	char np_name[32], * usage;
	char *s, buffcopy[256], extra[64];
	char **keyVal, *emptytest;
	int r;
	usage = (char * ) malloc (sizeof(char) * 512);
	strcpy(buffcopy, buff);
	
	emptytest = strtok(buffcopy, "##");
	
	if(emptytest == NULL) {
	    printf("Invalid argument to register_np. Exiting\n");
	    errno = EINVAL;
	    return -1;
	}
	strcpy(extra, emptytest);
	
	emptytest = strtok(extra, "::");
	
	if(emptytest == NULL) {
	    printf("Invalid argument to register_np. Exiting\n");
	    errno = EINVAL;
	    return -1;
	}
	
	strcpy(extra, emptytest);
	
	if(strcmp(extra, "npname")) {
	    printf("Invalid argument to register_np. Give npname as first argument. Exiting\n");
	    errno = EINVAL;
	    return -1
	    
	    ;
	}
	
	emptytest = strtok(NULL, "::");
	
	if(emptytest == NULL) {
	    printf("Invalid argument to register_np. Exiting\n");
	    errno = EINVAL;
	    return -1;
	}
	
	strcpy(np_name, emptytest);
	
	s = buff;

	strcpy(usage, s);
	extract_key_val(usage, &keyVal);

	/* if np exists, free and modify arguments.
	 *   if it doesnt, add it to the list */

	if(DATASTRUCT == LIST) {
		r = add_np(&npList, np_name, usage, &keyVal);
		if(r == -1) { /*error in adding np to list */
			errno = EAGAIN; /*ask to try again */
			return -1;
		}
		print_np(&npList);	    
	}
	if(DATASTRUCT == HASH) {
		r = add_np_to_hash(hstruct_np, np_name, usage,  &keyVal);
		if(r == -1) { /*error in adding np to hash */
			errno = EAGAIN; /*ask to try again */
			return -1;
		}
		print_hash_np(hstruct_np);
	}

	return 1;
}


/*It extracts an array of key::val strings from usage and stores it in array keyVal*/
void extract_key_val(char *usage, char ***keyVal)
{
	int cnt = 0, i = 0;
	char copyusage[512];
	char *ptr;


	strcpy(copyusage, usage);
	cnt = count_args(copyusage, "::");		/* Counting number of arguments */

	if((*keyVal = (char **)malloc((cnt + 1) * sizeof(char *))) == NULL) {
		PRINTF(" >%s %d extract_key_val() : malloc failed", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	ptr = strtok(copyusage, "##");

	if(((*keyVal)[i] = (char *)malloc(sizeof(char) * (strlen(ptr) + 1))) == NULL) {
		PRINTF("> %s %d extract_key_val() : malloc failed ", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}

	strcpy((*keyVal)[i], ptr);

	/* Extract key::value pairs and save them in keyVal */
	for (i = 1; i < cnt; i++) {
		ptr = strtok(NULL, "##");
		if (!ptr) {
			break;
		}
		if (((*keyVal)[i] = (char *)malloc(sizeof(char) * 128)) == NULL) {
			PRINTF("> %s %d extract_key_val(): Error in malloc", __FILE__, __LINE__);
			errno = ECANCELED;
			perror("Malloc failed");
		}
		strcpy((*keyVal)[i], ptr);
	}
	(*keyVal)[i] = NULL;
	return;
}

/* Count number of key::value pairs in the argument */
int count_args(char *myString, char *delim) {
	int count = 0;
	const char *tmp = myString;

	while ((tmp = (strstr(tmp, delim)))) {
		count++;
		tmp++;
	}
	return count;
}

/*function to unregister an np*/
int unregister_np(char *buff)
{
	int flag = 0;
	struct main_np_node * temp;
	char *np_name = (char *) malloc (sizeof(char) * 32);
	char delim[3] = "::";
	int app_cnt;
	struct app_node *s, *aptr;
	np_node *p, *q;

	strcpy(np_name, strtok(buff, delim));

	if(DATASTRUCT == HASH) {
		// delete np from registered app trailing list

		for(s = hstruct_app->app_hash; s != NULL; s=(struct app_node*)(s->hh.next)) {

			p = s->np_list_head;

			while(p != NULL) {

				if(!strcmp(p->name, np_name)) {	
					PRINTF("> %s %d unregister_np():Np node found : %s\n", __FILE__ , __LINE__, np_name);
					flag = 1;
					break;	
				}
				q = p;
				p = p->next;
			}
			if(flag == 0) {
				printf("NP node not found\n");
				errno = EINVAL;
				return -1;
			}
			if(s->np_count == 1) {
				s->np_list_head = NULL;
				free(p);
				s->np_count--;
			}
			else if(p == s->np_list_head) {						/* If node to be deleted is the first node */
				printf("I AM HERE, DELETING p = %s\n\n", p->name);
				//q = p->next;
				s->np_list_head = p->next;
				free(p);
				//p = NULL;
				s->np_count--;
			} 

			else {
				q->next = p->next;
				free(p);
				s->np_count--;
			}

		}

		del_np_from_hash(hstruct_np, np_name);
		print_hash_np(hstruct_np);
		print_hash_app(hstruct_app);

	}
	if(DATASTRUCT == LIST) { /* For List Datastructure case*/
		app_cnt = appList.count;

		/* Check if the NP exists */    
		temp = search_np(&npList, np_name);
		if(app_cnt == 0) { /*no app registered */
			if(temp != NULL) /*np_name found in np list */
				del_np_node(&npList, temp);
			print_np(&npList);
			print_app(&appList);	
			return 0;
		}
		if( temp != NULL) { /*np_name found and app list is not empty*/
			while(app_cnt != 0) { /* For every application it is registered with */
				/* Search every application's trailing list for a registration with that NP; remove that registration and reduce that application's count by one */			

				if(search_reg(&appList, aptr->data, np_name) == -1) {
					p = aptr->np_list_head;

					while(p != NULL) {
						q = p;

						if(!strcmp(p->name, np_name)) {	
							PRINTF("> %s %d unregister_np():Np node found : %s\n", __FILE__ , __LINE__, np_name);
							break;	
						}

						p = p->next;
					}
					if(aptr->np_count == 1) {
						aptr->np_list_head = NULL;
						free(p);
						p = NULL;
						aptr->np_count--;
					}
					else if(p == q) {		/* If node to be deleted is the first node */
						q = p->next;
						aptr->np_list_head = q;
						free(p);
						p = NULL;
						aptr->np_count--;
					}

					else {
						q->next = p->next;
						free(p);
						p = NULL;
						aptr->np_count--;
					}
				}
				aptr = aptr->next;
				app_cnt--;	   	
			}
			del_np_node(&npList, temp);

		}
		else {
			PRINTF(">%s %d unregister_np(): Np not registerd.\n", __FILE__ , __LINE__);
			perror("Not found NP - ");
			errno = ENODEV;
			return -1;
		}

		print_np(&npList);
	}
	return 1;
}

/*Function to get notification for app*/
char *getnotify_app(char *buff)
{	int ret;
	char *notification;
	struct getnotify_thread_args *arguments;
	pthread_t tid_app_np_gn;

	fprintf(logfd, "%s %d getnotify_app() : Buffer - %s \n", __FILE__, __LINE__, buff);
	force_logs();

	if((notification = (char *)malloc(1024 * sizeof(char))) == NULL) {
		PRINTF("> %s %d getnotify_app(): malloc failed",__FILE__,__LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	if((arguments = (struct getnotify_thread_args *)malloc(sizeof(struct getnotify_thread_args))) == NULL) {
		PRINTF("> %s %d getnotify_app() : malloc failed ", __FILE__, __LINE__);
		errno = ECANCELED;
		perror("Malloc failed");

	}
	strcpy(arguments->argssend, buff);

	fprintf(logfd, "%s %d getnotify_app() : Buffer - %s \n", __FILE__, __LINE__, arguments->argssend);
	force_logs();
	
	
	
	/* create thread to invoke the NP's code with the arguments given in the structure 'arguments', which contains argssend and argsrecv */
	if ((ret = pthread_create(&tid_app_np_gn, NULL, &np_getnotify_method,(void *)arguments) == 0)) {
		PRINTF(" > %s %d getnotify_app() :Pthread_Creation successful for getnotify\n", __FILE__ , __LINE__);
	}
	else {
		errno = ret;
		perror("Error in pthread_create for np_getnotify_method:");
	}

	if(( ret = pthread_join(tid_app_np_gn, NULL)) != 0) {
		perror("Error in joining threads pthread_join created by  getnotify_app(proceed_getnotify_method):");
	}

	strcpy(notification, arguments->argsrecv);

	free(buff);
	buff = NULL;

	free(arguments);
	arguments = NULL;

	return notification;	/*it will be callers responsibility to free the malloced notification string */
}

/* When NJ finally exits */
void nj_exit(void) {
	PRINTF(">%s %d nj_exit() : NJ exiting \n",__FILE__, __LINE__);
	int ret;

	char buf2[64];
	close(fd_pidnames);
	fd_pidnames = open(PIDFILE, O_CREAT | O_RDWR, 0777);
	FILE *pidnames = fdopen(fd_pidnames, "r");
	/* While there are PIDs of applications, remove them one by one */
	while (fgets(buf2, sizeof(buf2), pidnames)) {
		buf2[strlen(buf2) - 1] = '\0';
		unlink(buf2);
		PRINTF("> %s %d sigint_handler() : %s removed\n",__FILE__,__LINE__, buf2);
	}
	unlink(PIDFILE);
	PRINTF("> %s %d sigint_handler() : File_PIDS.txt removed\n", __FILE__, __LINE__);

	/* For every np, delete its registration with the app first */
	int i = appList.count;
	int j = npList.count;
	struct app_node *temp, *temp1;
	struct main_np_node *temp2,*temp3;

	temp = appList.head;

	temp2 = npList.head;

	for(; i != 0;i--) {
		temp1 = temp->next;
		unregister_app(temp->data);
		PRINTF("> %s %d nj_exit(): %d app_count \n",__FILE__,__LINE__,appList.count);
		temp = temp1;
	}

	printf(">%s %d empty_app_list : App List deleted completely  %d \n ",__FILE__ , __LINE__, appList.count);


	print_app(&appList);

	print_np(&npList);


	for(; j != 0;j--) {

		printf("IN NP LOOP, before delete Unregistering %s \n", temp2->data);
		temp3 = temp2->next;

		temp2->app_count = 0;

		del_np_node(&npList, temp2);

		printf("Deleting\n");

		temp2 = temp3;

		printf("next\n");

	}


	print_app(&appList);

	print_np(&npList);

	if((ret = pthread_mutex_destroy(&getnotify_socket_mutex)) != 0) {
		errno = ret;
		perror("Error in destroying getnotify_socket_mutex");
	}

	main_np_node *s, *tmp;
	HASH_ITER(hh, hstruct_np->np_hash, s, tmp) {
		//first we need to free key_val_arr of np then free np
		HASH_DEL(hstruct_np->np_hash, s);
		free(s);
	}	

	//need to free whole app list
}

