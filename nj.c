/*Notification-Junction is an Interface between multiple Applications and multiple Notification Providers.
Copyright (C) 2015  Navroop Kaur<watwanichitra@gmail.com> , Shivani Marathe<maratheshivani94@gmail.com> , Kaveri Sangale<sangale.kaveri9@gmail.com>
	All Rights Reserved
	
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
   
 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA 
*/

/* 
*	The file contains the code for the Notification-Junction implementation 
*	It has functions for 
*	app registration	
*	app unregistration
*	np registration
*	np unregistration 
*	app_registraion thread routine
*	app_unregistration thread routine
*	np_registration thread routine
*	np_unregistration thread routine
*	app getnotify 
*	app getnotify thead routine
*	np getnotify thread routine
*
*/

#include "nj.h"

/* Declare global NP list */

np_dcll npList;			/*head of npList */

/*Declare global app List*/
app_dcll appList;		/*head of appList */
int fd_pidnames;
void *handle;

/*Mutexes to be used for synchronizing list*/

pthread_mutex_t app_list_mutex, np_list_mutex, app_list_count_mutex,
    np_list_count_mutex, getnotify_socket_mutex;
void print_stat()
{

	//printf("NP\t\t\t\t\tCOUNT OF REGISTERED APPs");
	//printf("---------------------------------------------------------------------------------------------------------");
	print_np(&npList);
	//printf("---------------------------------------------------------------------------------------------------------");
	print_app(&appList);
	//printf("---------------------------------------------------------------------------------------------------------");

}

//To be called while NJ is exiting to remove all pid files
void sigintHandler(int signum)
{
	printf("In sigIntHandlere\n");
	if (signum == SIGINT) {
		close(fd_pidnames);

		fd_pidnames = open("File_PIDS.txt", O_CREAT | O_RDWR, 0777);
		int r_cnt, sys_r;
		char buf1[64], buf2[64];
		FILE *pidnames = fdopen(fd_pidnames, "r");
		//printf("Removing pid files\n");
		while (fgets(buf2, sizeof(buf2), pidnames)) {
			buf2[strlen(buf2) - 1] = '\0';
			/*strcpy(buf1, "rm ");
			   strcat(buf1, buf2);
			   sys_r = system(buf1); */
			unlink(buf2);
			printf("%s removed\n", buf2);
		}
		unlink("File_PIDS.txt");
		printf("File_PIDS.txt removed\n");
		//printf("Removed pid files\n\n");
		exit(0);

	}
}

/*TO REGISTER APP with NJ*/
int register_app(char *buff)
{
	int i;
	char app_name[32], np_name[32];
	char *s, *n;
	char delim[3] = "::";
	int retval;

	strcpy(app_name, strtok(buff, delim));
	s = strtok(NULL, delim);
	if (s != NULL)
		strcpy(np_name, s);

	//strtok(buff, "##");
	//n = strtok(NULL, "##");
	//printf("ARGUMENTS LIST _ %s\n", n);

	/*Check if NP provided is registered or not */

	if (search_np(&npList, np_name) == NULL && np_name != NULL) {	/* HANDLE IF NP IS NULL, CHECK FOR STRTOK */
		printf
		    ("NJ.C   :  NJ : Np not registered. Register NP first.\n");
		printf("NJ.C   : AppList:\n");
		print_app(&appList);
		printf("NJ.C   : NPList:\n");
		print_np(&npList);
		return 1;
	}

	/*Check if that registration already exists */

	if (search_app(&appList, app_name) == NULL) {

		addapp_node(&appList, app_name);
		printf("Added for the first time\n");

	}

	/*HANDLED IN LIST */
	retval = searchReg(&appList, app_name, np_name);

	if (retval == -1) {

		printf("NJ.C   : NJ : EXISTING REGISTRATION\n");

	}

	/*need to change this function to return appropriate values */
	if (retval != -1) {	/*IF APP IS NOT PREVIOUSLY REGISTERED */
		pthread_mutex_lock(&np_list_count_mutex);
		incr_np_app_cnt(&npList, np_name);
		add_np_to_app(&appList, app_name, np_name);

		pthread_mutex_unlock(&np_list_count_mutex);
		if (np_name == NULL) {
			pthread_mutex_lock(&app_list_mutex);
			pthread_mutex_unlock(&app_list_mutex);
			printf("NJ.C   : NJ : Added successfully\n");
		}
	}

	printf("NJ.C   : AppList:\n");
	print_app(&appList);
	printf("NJ.C   : NPList:\n");
	print_np(&npList);
	return 1;
}

/*FUNCTION TO UNREISTER AN APP*/

int unregister_app(char *buff)
{
	/*if app_unregister ie buff passes app1::np1 it will remove np1 .......else if buff is app1 ie app_unregister app1 then remove app node directly */
	char app_name[32], np_name[32];
	int retval;
	char *s;
	char delim[3] = "::";

	strcpy(app_name, strtok(buff, delim));

	printf("App name - %s\n", app_name);

	s = strtok(NULL, delim);

	if (s != NULL) {
		strcpy(np_name, s);
		printf("np name - %s\n", np_name);

	}

	if (s == NULL) {
		printf("NJ.C   : np_name == NULL case in unregister app\n");
		pthread_mutex_lock(&app_list_mutex);
//This function decrements counts for all NPs with that app, we have to write this function in the nj because it accesses both lists
		dec_all_np_counts(&appList, &npList, app_name);

		del_app(&appList, app_name);

// DEC HERE FOR ALL NPs with that app.

		pthread_mutex_unlock(&app_list_mutex);
	} else {

		retval = searchReg(&appList, app_name, np_name);

		if (retval == -1) {

			printf("NJ.C   : NJ : REGISTRATION FOUND.\n");

			pthread_mutex_lock(&app_list_mutex);

// LOCK THE NP LIST
			decr_np_app_cnt(&npList, np_name);
			del_np_from_app(&appList, app_name, np_name);
// DEC HERE for the np given.

			pthread_mutex_unlock(&app_list_mutex);

		}
		/*need to change this function to return appropriate values */
		else {
			printf
			    ("NJ : Invalid argument to app_unregister : Registration not found\n");
		}

	}
	print_app(&appList);
	print_np(&npList);
	return 1;
}

/*FUNCTION TO REGISTER AN NP*/
int register_np(char *buff)
{
	char np_name[32];
	char delimkeyval[3] = "::";
	char delimusage[3] = "==";
	char usage[512];
	char *s;
	char **keyVal;
	main_np_node *ptr;

	strcpy(np_name, strtok(buff, delimusage));
	s = strtok(NULL, delimusage);

	if (s == NULL) {
		printf("Enter usage\nExiting\n");
		return -1;
	}

	strcpy(usage, s);

	extractKeyVal(usage, &keyVal);

	pthread_mutex_lock(&np_list_mutex);
	add_np(&npList, np_name, usage, &keyVal);
	pthread_mutex_unlock(&np_list_mutex);

	/*
	   ptr = search_np(&npList, np_name);
	   printf("Found ptr with name %s\n", ptr->data);
	   ptr->usage = (char*)malloc(sizeof(char) * 512);
	   strcpy(ptr->usage, usage);
	   printf("ptr->usage = %s\n", ptr->usage);
	 */

	print_np(&npList);
	return 1;
}

void extractKeyVal(char *usage, char ***keyVal)
{

	printf("NJ : EXTRACTVAL : usage is %s\n", usage);
	//keyVal = (char ***)malloc(sizeof(char **));
	int cnt = 0, i = 0;
	char copyusage[512];
	char *ptr;
	strcpy(copyusage, usage);
	cnt = countArgs(copyusage, "::");
	printf("EXTRACT KEYVAL : NJ : The count is %d\n", cnt);

	*keyVal = (char **)malloc((cnt + 1) * sizeof(char *));

	ptr = strtok(copyusage, "##");

	printf("EXTRACT : NJ : PTR[%d] is %s\n", i, ptr);

	(*keyVal)[i] = (char *)malloc(sizeof(char) * strlen(ptr));
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

int countArgs(char *myString, char *delim)
{

	int count = 0;
	const char *tmp = myString;
	while (tmp = strstr(tmp, delim)) {
		count++;
		tmp++;
	}
	return count;
}

/*FUNCTION TO UNREGISTER AN NP*/
int unregister_np(char *buff)
{
	char np_name[32];
	char delim[3] = "::";
	strcpy(np_name, strtok(buff, delim));
	/*while(s != NULL ) 
	   {
	   printf("NJ.C   : %s\n", s);
	   s = strtok(NULL, delim);
	   } */

	pthread_mutex_lock(&np_list_mutex);
	del_np(&npList, np_name);
	pthread_mutex_unlock(&np_list_mutex);

	print_np(&npList);
	return 1;
}

/*FUNCTION TO GET NOTIFICATION FOR APP*/

char *getnotify_app(char *buff)
{
	char *notification;
	notification = (char *)malloc(1024 * sizeof(char));
	struct getnotify_threadArgs arguments;
	strcpy(arguments.argssend, buff);
	pthread_t tid_app_np_gn;

	/* Fork thread to invoke the NP's code with the arguments given in the structure 'arguments', which contains argssend and argsrecv */
	if (pthread_create
	    (&tid_app_np_gn, NULL, &NpGetNotifyMethod,
	     (void *)&arguments) == 0) {
		printf("NJ.C   : Pthread_Creation successful for getnotify\n");
	}
	pthread_join(tid_app_np_gn, NULL);
	strcpy(notification, arguments.argsrecv);
	return notification;	/*it will be callers responsibility to free the malloced notification string */
}

/* MAIN CODE */
int main(int argc, char *argv[])
{
	/* first remove(unlink) the sockets if they already exist */

	signal(SIGINT, sigintHandler);
	sigset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);

	unlink(StatSocket);
	unlink(AppReg);
	unlink(AppUnReg);
	unlink(NpReg);
	unlink(NpUnReg);
	unlink(AppGetnotify);

	printf("NJ.C   : In main\n");

	fd_pidnames = open("File_PIDS.txt", O_CREAT | O_RDWR, 0777);
	printf("NJ.C   : Files_PIDs.txt created.\n");
	printf("NJ.C   : fd = %d", fd_pidnames);

	/*Initialization of all mutexes */

	if (pthread_mutex_init(&app_list_mutex, NULL) != 0) {
		printf("NJ.C   : \n mutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&np_list_mutex, NULL) != 0) {
		printf("NJ.C   : \n mutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&app_list_count_mutex, NULL) != 0) {
		printf("NJ.C   : \n mutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&np_list_count_mutex, NULL) != 0) {
		printf("NJ.C   : \n mutex init failed\n");
		return 1;
	}

	/*Initialize NP List */
	init_np(&npList);
	printf("NJ.C   : Initialization of NPlist\n");
	/*Initialize APP List */
	init_app(&appList);
	printf("NJ.C   : initialization of app lsit \n");
	printf("initialization of app lsit \n");

	/* CREATE SOCKET FOR STAT */

	struct threadArgs stat;

	stat.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	printf("Socket stat created\n");

	if (stat.sock < 0) {
		perror("opening stream socket");
		exit(1);
	}

	stat.server.sun_family = AF_UNIX;
	strcpy(stat.server.sun_path, StatSocket);

	if (bind
	    (stat.sock, (struct sockaddr *)&stat.server,
	     sizeof(struct sockaddr_un))) {
		perror("binding stream socket");
		exit(1);
	}

	printf("Socket has name %s\n", stat.server.sun_path);

	/* CREATE SOCKET FOR APPLICATION REGISTRATION */

	struct threadArgs app_reg;

	app_reg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	printf("NJ.C   : Socket app_reg created\n");

	if (app_reg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}

	app_reg.server.sun_family = AF_UNIX;
	strcpy(app_reg.server.sun_path, AppReg);

	if (bind
	    (app_reg.sock, (struct sockaddr *)&app_reg.server,
	     sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	printf("NJ.C   : Socket has name %s\n", app_reg.server.sun_path);

	/* CREATE SOCKET FOR APPLICATION UNREGISTRATION */

	struct threadArgs app_unreg;

	app_unreg.sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (app_unreg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}

	app_unreg.server.sun_family = AF_UNIX;
	strcpy(app_unreg.server.sun_path, AppUnReg);

	if (bind
	    (app_unreg.sock, (struct sockaddr *)&app_unreg.server,
	     sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	printf("NJ.C   : Socket has name %s\n", app_unreg.server.sun_path);

	/* SOCKET FOR NP REGISTRATION */

	struct threadArgs np_reg;

	np_reg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	printf("NJ.C   : Socket np_reg created\n");

	if (np_reg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}

	np_reg.server.sun_family = AF_UNIX;
	strcpy(np_reg.server.sun_path, NpReg);

	if (bind
	    (np_reg.sock, (struct sockaddr *)&np_reg.server,
	     sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	printf("NJ.C   : Socket has name %s\n", np_reg.server.sun_path);

	/* SOCKET FOR NP UNREGISTRATION */

	struct threadArgs np_unreg;

	np_unreg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	printf("NJ.C   : Socket np_unreg created\n");

	if (np_unreg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}

	np_unreg.server.sun_family = AF_UNIX;
	strcpy(np_unreg.server.sun_path, NpUnReg);

	if (bind
	    (np_unreg.sock, (struct sockaddr *)&np_unreg.server,
	     sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	printf("NJ.C   : Socket has name %s\n", np_unreg.server.sun_path);
	/* CREATE SOCKET for APP SIDE GETNOTIFY */

	struct threadArgs app_getnotify;

	app_getnotify.sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (app_getnotify.sock < 0) {
		perror("NJ.C   : opening stream socket for getnotify");
		exit(1);
	}

	app_getnotify.server.sun_family = AF_UNIX;
	strcpy(app_getnotify.server.sun_path, AppGetnotify);

	if (bind
	    (app_getnotify.sock, (struct sockaddr *)&app_getnotify.server,
	     sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	printf("NJ.C   : Socket has name %s\n", app_getnotify.server.sun_path);

	/* FORK THREADS TO LISTEN  AND ACCEPT */

	pthread_t tid_stat;

	if (pthread_create(&tid_stat, NULL, &PrintStat, (void *)&stat) == 0) {
		printf("Pthread_Creation successful for stat\n");
	}

	pthread_t tid_app_reg;

	if (pthread_create(&tid_app_reg, NULL, &AppRegMethod, (void *)&app_reg)
	    == 0) {
		printf("NJ.C   : Pthread_Creation successful\n");
	}

	pthread_t tid_app_unreg;

	if (pthread_create
	    (&tid_app_unreg, NULL, &AppUnRegMethod, (void *)&app_unreg) == 0) {
		printf("NJ.C   : Pthread_Creation successful\n");
	}

	pthread_t tid_np_reg;

	if (pthread_create(&tid_np_reg, NULL, &NpRegMethod, (void *)&np_reg) ==
	    0) {
		printf("NJ.C   : Pthread_Creation successful\n");
	}

	pthread_t tid_np_unreg;

	if (pthread_create
	    (&tid_np_unreg, NULL, &NpUnRegMethod, (void *)&np_unreg) == 0) {
		printf("NJ.C   : Pthread_Creation successful\n");
	}

	pthread_t tid_app_getnotify;
	if (pthread_create
	    (&tid_app_getnotify, NULL, &AppGetNotifyMethod,
	     (void *)&app_getnotify) == 0) {
		printf
		    ("NJ.C   : Pthread_creation of getnotify thread successful\n");
	}

	/*Join all the threads */
	pthread_join(tid_np_reg, NULL);
	pthread_join(tid_np_unreg, NULL);
	pthread_join(tid_app_reg, NULL);
	pthread_join(tid_app_unreg, NULL);
	pthread_join(tid_app_getnotify, NULL);

	pthread_mutex_destroy(&app_list_mutex);
	pthread_mutex_destroy(&np_list_mutex);
	pthread_mutex_destroy(&app_list_count_mutex);
	pthread_mutex_destroy(&np_list_count_mutex);
	return 0;

}

/* APP REGISTER METHOD THAT WILL RUN IN THREAD FORKED FOR APP REGISTRATION*/

void *PrintStat(void *arguments)
{
	struct threadArgs *args = arguments;
	printf("In STAT\n");
	printf("args -> msgsock - %d\n", args->msgsock);

	listen(args->sock, QLEN);
	int i = 0;
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("accept");
		else
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
				     read(args->msgsock, args->buf, 1024)) < 0)
					perror("reading stream message");
				else if (args->rval == 0) {
					printf("\nPrinting Statistics :\n");
					print_stat();
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(StatSocket);

	pthread_exit(NULL);
	return NULL;
}

void *AppRegMethod(void *arguments)
{
	struct threadArgs *args = arguments;
	printf("NJ.C   : In Reg\n");
	printf("NJ.C   : args -> msgsock - %d\n", args->msgsock);

	listen(args->sock, QLEN);
	int i = 0;
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
				     read(args->msgsock, args->buf, 1024)) < 0)
					perror
					    ("NJ.C   : reading stream message");
				else if (args->rval == 0)
					printf("NJ.C   : Ending connection\n");
				else {	/*code to register application ie add entry in list */
					printf("NJ.C   : -->%s\n", args->buf);
					i = register_app(args->buf);
					if (i < 0) {
						printf
						    ("NJ.C   : Error in registering, try again\n");
					}
					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(AppReg);

	pthread_exit(NULL);
	return NULL;
}

/*APP UNREGISTER METHOD THAT WILL RUN IN THREAD FORKED FOR UNREGISTRATION*/
void *AppUnRegMethod(void *arguments)
{
	struct threadArgs *args = arguments;
	printf("NJ.C   : In UnReg\n");
	printf("NJ.C   : args -> msgsock - %d\n", args->msgsock);

	listen(args->sock, QLEN);
	int i = 0;
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
				     read(args->msgsock, args->buf, 1024)) < 0)
					perror
					    ("NJ.C   : reading stream message");
				else if (args->rval == 0)
					printf("NJ.C   : Ending connection\n");
				else {
					i = unregister_app(args->buf);
					printf("NJ.C   : -->%s\n", args->buf);

					if (i < 0)
						printf
						    ("NJ.C   : Error in registering\n");

					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(AppUnReg);

	pthread_exit(NULL);
	return NULL;
}

/*NP REGISTER METHOD THAT WILL RUN IN THREAD FORKED FOR NP REGISTRATION*/
void *NpRegMethod(void *arguments)
{
	struct threadArgs *args = arguments;
	printf("NJ.C   : In NpReg\n");
	printf("NJ.C   : args -> msgsock - %d\n", args->msgsock);

	listen(args->sock, QLEN);
	int i = 0;
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
				     read(args->msgsock, args->buf, 1024)) < 0)
					perror
					    ("NJ.C   : reading stream message");
				else if (args->rval == 0)
					printf("NJ.C   : Ending connection\n");
				else {	/*code to register application ie add entry in list */
					printf("NJ.C   : -->%s\n", args->buf);
					i = register_np(args->buf);
					if (i < 0)
						printf
						    ("NJ.C   : Error in registering\n");
					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(NpReg);

	pthread_exit(NULL);
	return NULL;
}

/*NP UNREGISTER METHOD THAT WILL RUN IN THREAD FORKED FOR NP UNREGISTRATION*/
void *NpUnRegMethod(void *arguments)
{
	struct threadArgs *args = arguments;
	printf("NJ.C   : In NpUnReg\n");
	printf("NJ.C   : args -> msgsock - %d\n", args->msgsock);

	listen(args->sock, QLEN);
	int i = 0;
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval =
				     read(args->msgsock, args->buf, 1024)) < 0)
					perror
					    ("NJ.C   : reading stream message");
				else if (args->rval == 0)
					printf("NJ.C   : Ending connection\n");
				else {
					i = unregister_np(args->buf);
					printf("NJ.C   : -->%s\n", args->buf);

					if (i < 0)
						printf
						    ("NJ.C   : Error in registering\n");

					break;
				}
			} while (args->rval > 0);

		close(args->msgsock);
	}

	close(args->sock);
	unlink(NpUnReg);

	pthread_exit(NULL);
	return NULL;
}

/*APP GET NOTIFY METHOD THAT WILL RUN IN THREAD FOR GET NOTIFICATION*/
void *AppGetNotifyMethod(void *arguments)
{
	struct threadArgs *args = arguments;
	printf("NJ.C   : In AppGetNotify\n");
	printf("NJ.C   : args -> msgsock - %d\n", args->msgsock);
	int i = 0;
	struct proceedGetnThreadArgs *sendargs;
	sendargs = (struct proceedGetnThreadArgs *)
	    malloc(sizeof(struct proceedGetnThreadArgs));

	listen(args->sock, QLEN);
	char *argstring;
	FILE *fp;
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, AppGetnotify);
	pthread_t threadarr[APPLIMIT];

	argstring = (char *)malloc(sizeof(char) * 1024);
	for (;;) {

		
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {	pthread_mutex_lock(&getnotify_socket_mutex);
				bzero(args->buf, sizeof(args->buf));
				args->rval =
				    read(args->msgsock, args->buf, 1024);
				strcpy(sendargs->buf, args->buf);
				printf("\n\n\nsendargs has buf = %s\n",
				       sendargs->buf);
				pthread_mutex_unlock(&getnotify_socket_mutex);
				if (args->rval < 0)
					perror
					    ("NJ.C   : reading stream message");
				else if (args->rval == 0)
					printf("NJ.C   : Ending connection\n");
				else {
					printf
					    ("ARGS SENDING TO PROCEEDGETN are %s\n\n\n",
					     args->buf);
					printf("%d is i in ProcerdGetnotify \n",
					       i);
					if (pthread_create
					    (&threadarr[i++], NULL,
					     &ProceedGetnotifyMethod,
					     (void *)sendargs) == 0) {
						perror
						    ("NJ.C   : Pthread_Creations for ProceedgetnotifyMethod\n");
					}
					//ProceedGetnotifyMethod(arguments);
					break;
				}

			} while (args->rval > 0);
		printf("NJ.C   :  after exit ing args->rval is %d\n",
		       args->rval);
		close(args->msgsock);
	}
	while (i >= 0) {
		pthread_join(threadarr[i], NULL);
		i--;
	}

	close(args->sock);
	unlink(AppUnReg);

	pthread_exit(NULL);
	return NULL;
}

/*NP GET NOTIFY METHOD THAT WILL RUN NP GET NOTIFY THREAD*/
void *NpGetNotifyMethod(void *arguments)
{				/*here we need to call Np_specific getnotify to get the notificationstring..this notificationstring is then copied to arguments->argsrecv
				   for now I am sending the received strig directly as notificationstring for now I have simply returned the string receive1::value1 */
	struct getnotify_threadArgs *args = arguments;
	int j;

	void (*getnotify) (struct getnotify_threadArgs *);
	char *error;
	char rough[1024], r[1024];
	strcpy(rough, args->argssend);
	strcpy(r, rough);
	np_node *nptr;
	char appname[64];

	/* INOTIFY needs npname::<npname>##pathname::<pathname>##flags::<flags> */

	printf("NJ.C   : Args received by getnotify - %s\n", args->argssend);
	int i, count;
	char np_name[64], dir_name[256], flag_set[512], one[512], two[512],
	    three[512];
	char delimattr[3] = "##";
	char delimval[3] = "::";
	struct extr_key_val *temp, *m, *p;
	char **pointer;

	strtok(r, delimattr);
	j = strlen(r);
	strcpy(r, args->argssend);
	strcpy(r, &(r[j + 2]));
	strcpy(args->argssend, r);

	strcpy(appname, strtok(rough, delimattr));
	printf("N.C : appname = %s\n", appname);

	strcpy(one, strtok(NULL, delimattr));
	strtok(one, delimval);	/* HERE */
	strcpy(np_name, strtok(NULL, delimval));

	printf("N.C : npname = %s\n", np_name);
	/* Get the reg, Malloc space for a getn registration key_val  */

	nptr = getReg(&appList, appname, np_name);
	if (nptr) {
		printf("\nRETURNED nptr->name = %s\n", nptr->name);
	}

	printf("NJ : ARGSSEND BEFORE EXTRACT : %s\n", args->argssend);

	temp = (struct extr_key_val *)malloc(sizeof(struct extr_key_val));
	temp->next = NULL;
	m = nptr->key_val_ptr;
	p = m;

pthread_mutex_lock(&app_list_mutex);

	if (m == NULL) {
		nptr->key_val_ptr = temp;
	} else {
		printf("EXTRAct : Next case else \n");
		while (m != NULL) {
            p = m; 
			m = m->next;
		}
		p->next = temp;
		//m->next = temp;
		temp->next = NULL; // 
	}
	// Save the keyvalarr
	//
	//
	//

	printf("PRINTING BEFORE EXTRACT\n");

pthread_mutex_unlock(&app_list_mutex);


	extractKeyVal(args->argssend, &pointer);
	temp->key_val_arr = pointer;
	print_app(&appList);
	printf("PRINTING AFTER EXTRACT\n");
//    print_app(&appList);

	//
	//
	//

	/* HANDLE NO NAME AND STUFF */
	

	printf("NJ.C   : App Count = %d for NP = %s\n",
	       get_np_app_cnt(&npList, np_name), np_name);

	/* App count will be 0 initially. App can call getnotify only after NP has registered, and doing App_getnotify for the first time increments the count to 1. Therefore compare count with 1 to do dlopen() for the first time */
	if (get_np_app_cnt(&npList, np_name) == 1) {
		/* Do dlopen() */
		printf("NJ.C   : Opening\n....\n");
		handle = dlopen("./libinotify.so", RTLD_LAZY);
		if (!handle) {
			perror("NJ.C   : Problem with dlopen :");
			exit(1);
		}

		printf("NJ.C   : dlopen successful\n");

	}
	/* Do dlsym() for getnotify() */

	getnotify = dlsym(handle, "getnotify");
	if ((error = dlerror()) != NULL) {
		perror("NJ.C   : chukla");
		exit(1);
	}

	printf("NJ.C   : Handle Successful\n");

	(*getnotify) (args);

	printf("NJ.C   : In NPMethod, Recd = %s\n", args->argsrecv);

	/*Dlclose       */
	/* dlclose(handle); */
	/*HANDLE CLOSING PROPERLY. WHEN TO CLOSE */

	printf("NJ.C   : ARGSRECV IN THREAD HANDLER = %s\n", args->argsrecv);

	pthread_exit(NULL);
	return NULL;

	/* Functions have to be written such that the argument will be the address of struct args */
}

void dec_all_np_counts(app_dcll * appList, np_dcll * npList, char *app_name)
{

	app_node *ptrapp;
	np_node *ptrnp;
	int cnt;
	// Find the app in the list. If it is found, for every np in its trailing list; visit the np_dcll and decrement its count.
	printf("All nps deleted, since np_name was passed NULL\n");

	ptrapp = search_app(appList, app_name);

	if (ptrapp != NULL) {	// App found

		cnt = ptrapp->np_count;
		ptrnp = ptrapp->np_list_head;
		printf("Application %s found\n", ptrapp->data);
		printf("NPs are \n");
		while (cnt) {
			printf("\t%s\t", ptrnp->name);
			decr_np_app_cnt(npList, ptrnp->name);
			// printf("\t%d\n", ptrnp->);
			ptrnp = ptrnp->next;
			cnt--;
		}

	} else {

		printf
		    ("Error : NJ : Asked to delete non-existent application\n");

	}

}

void *ProceedGetnotifyMethod(void *arguments)
{
	char *received;
	struct proceedGetnThreadArgs *args = arguments;
	printf("ARGS SENDING TO PROCEEDGETN are %s\n\n\n", args->buf);
	int pid;
	char rough[1024];
	char choice;
	int j;
	char *ptr;
	strcpy(rough, args->buf);
	printf("NJ.C   :  %s  args-.buf received from library call\n",
	       args->buf);
	int len = strlen(rough);
	choice = rough[len - 1];
	char spid[32];
	strcpy(spid, strtok(rough, "##"));
	pid = atoi(spid);
/*for(j = 0; j < len ; j++) {
			if(rough[j] == '#')
			if(rough[j+1] == '#')
					break;
		}*/

	j = strlen(rough);
	//strcpy(rough, args->buf);
	//strcpy(rough , &(rough[j+2]));
	//strcpy(args->buf, rough);
	printf("NJ.C   : Received args->buf is %s\n", args->buf);
	received = getnotify_app(args->buf);
	int fd, al;
	char filename[64], filename1[64];
	strcpy(filename, "./");
	strcat(filename, spid);
	strcpy(filename1, spid);
	strcat(filename, ".txt");
	strcat(filename1, ".txt");
	strcat(filename1, "\n");
	printf("NJ.C   : Filename:%s\n\n", filename);
	if (choice == 'N') {
		if (!(fd = open(filename, O_CREAT | O_APPEND | O_RDWR, 0777))) {
			perror("NJ.C   : not able to open file\n");
			return;
		}
		printf("NJ.C   : OPEN::%d is FD for %s file\n\n", fd, filename);

		printf("NJ.C   : %s \n", received);
		strcat(received, "\n");
		write((int)fd_pidnames, filename1, strlen(filename1));
		printf("\n\nPID filename is written successfully.....\n\n");
		al = write((int)fd, received, strlen(received));
		if (al < 0)
			perror("NJ.C   : Fwrite failed");
		else
			printf("NJ.C   : %s madhe %d bytes WRITTEN\n", filename,
			       al);

		printf("NJ.C   : Before Sig, %d sending to pid = %d \n",
		       getpid(), pid);
		if (kill(pid, SIGUSR1) == -1) {
			perror("Error in kill :\n");
			// CHANGE TO PRINTF LATER
		}
		printf("NJ.C   : After Sig \n");
	}
	printf("NJ.C   : -->%s\n", args->buf);
	printf("NJ.C   : received notification is %s \n", received);
	/*send the socket to get_notify client */
	/*if (connect(args->msgsock, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) < 0) {
	   close(args->msgsock);
	   perror("NJ.C   : connecting stream socket");
	   exit(1);
	   } */
	printf("NJ.C   : REPORTING TO CLIENT EVENT : %s\n", received);
	if (choice == 'B') {
		/*if (write(args->msgsock, received, 1024) < 0) {
		   perror("NJ.C   : writing on stream socket");
		   } */

		// WRITE TO FILE HERE

		if (!(fd = open(filename, O_CREAT | O_APPEND | O_RDWR, 0777))) {
			perror("NJ.C   : not able to open file\n");
			return;
		}
		printf("NJ.C   : OPEN::%d is FD for %s file\n\n", fd, filename);

		printf("NJ.C   : %s \n", received);
		strcat(received, "\n");
		write((int)fd_pidnames, filename1, strlen(filename1));
		printf("\n\nPID filename is written successfully.....\n\n");
		al = write((int)fd, received, strlen(received));
		if (al < 0)
			perror("NJ.C   : Fwrite failed");
		else
			printf("NJ.C   : %s madhe %d bytes WRITTEN\n", filename,
			       al);

		printf("NJ.C   :  Before freeing \n");
		free(received);
		return;

	}
}
