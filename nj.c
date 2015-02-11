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

np_dcll npList;						/* Head of global NP List */
FILE* logfd;						/* File pointer of log file */
app_dcll appList;					/* Head of global APP List */
int fd_pidnames;					/* FD of file used for storing PID filenames */
void *handle;						/* Handle of the function in shared library */
pthread_mutex_t app_list_mutex, np_list_mutex,  getnotify_socket_mutex;
							/*Mutexes to be used for synchronizing list*/

/* FUNCTION TO FORCEFULLY WRITING TO LOG FILE */
void force_logs(void) {
	fclose(logfd);
	if (!(logfd = fopen("Logs", "a+"))) {
		perror("NJ.C   : not able to open  Log file\n");
	}
	return;
}
 
/* FUNCTION TO PRINT STATISTICS OF NJ */
void printStat()
{
	pthread_mutex_lock(&np_list_mutex);
	printNp(&npList);
	pthread_mutex_unlock(&np_list_mutex);
	pthread_mutex_lock(&app_list_mutex);
	printApp(&appList);
	pthread_mutex_unlock(&app_list_mutex);
}

/* SIGNAL HANDLER OF NJ */
void sigintHandler(int signum)
{
	char buf2[64];
	PRINTF("In sigIntHandlere\n");
	if (signum == SIGINT) {				/* Handling SIGINT signal to remove PID files */
		close(fd_pidnames);
		fd_pidnames = open("File_PIDS.txt", O_CREAT | O_RDWR, 0777);
		FILE *pidnames = fdopen(fd_pidnames, "r");
		while (fgets(buf2, sizeof(buf2), pidnames)) {
			buf2[strlen(buf2) - 1] = '\0';
			unlink(buf2);
			PRINTF("%s removed\n", buf2);
		}
		unlink("File_PIDS.txt");
		PRINTF("File_PIDS.txt removed\n");
		exit(0);
	}
}

/*TO REGISTER APP with NJ*/
int registerApp(char *buff)
{
	char app_name[32], np_name[32], delim[3] = "::";
	char *s;
	int retval;

	strcpy(app_name, strtok(buff, delim));
	s = strtok(NULL, delim);
	if (s != NULL)
		strcpy(np_name, s);
		
	pthread_mutex_lock(&app_list_mutex);
    	pthread_mutex_lock(&np_list_mutex);
    	
    	if(np_name != NULL) {
    		/*Check if NP provided is registered or not */
		if (searchNp(&npList, np_name) == NULL) {				/* NP is not registerd */
			PRINTF("> %s %d registerApp NJ.C Np not registered. Register NP first.\n", __FILE__ , __LINE__);
			return -1;
		}
		
		/*Check if app is already registered */
		if (searchApp(&appList, app_name) == NULL) {				/* Registering app for the first time */
			addAppNode(&appList, app_name);
			PRINTF("> %s %d registerApp: Added for the first time\n", __FILE__ , __LINE__);
			addNpToApp(&appList, app_name, np_name);                               
			incrNpAppCnt(&npList, np_name);
		}
		else {									/* Adding NP registration to already */
											/* existing App */
			retval = searchReg(&appList, app_name, np_name);
			if (retval == -1) {
				PRINTF("> %s %d registerApp : EXISTING REGISTRATION\n", __FILE__ , __LINE__);
				return -2;
			}
			else {	 
				addNpToApp(&appList, app_name, np_name);                               
				incrNpAppCnt(&npList, np_name);
			}
		}
	
	}
	else {
		addAppNode(&appList, app_name);
		PRINTF("> %s %d registerApp :Only app is added successfully.\n", __FILE__ , __LINE__);
	}
	
	
	pthread_mutex_unlock(&np_list_mutex);
    	pthread_mutex_unlock(&app_list_mutex);
    	
	return 1;
}

/*FUNCTION TO UNREISTER AN APP*/
int unregisterApp(char *buff)
{
	char app_name[32], np_name[32], delim[3] = "::";
	char *np_ptr;

	strcpy(app_name, strtok(buff, delim));
	np_ptr = strtok(NULL, delim);
	
	if (np_ptr != NULL) {
		strcpy(np_name, np_ptr);

	}
	
	pthread_mutex_lock(&app_list_mutex);
        pthread_mutex_lock(&np_list_mutex);

	if (np_ptr == NULL) {
		PRINTF("> %s %d unregisterApp :np_name == NULL case in unregister app\n", __FILE__ , __LINE__);
		if(searchApp(&appList, app_name) != NULL) {
			delApp(&appList, app_name);		
			decAllNpCounts(&appList, &npList, app_name);
			PRINTF("NJ.C : Unregistration done\n");
		}
		else {
			PRINTF("App not found\n");
			return -3;
		}
		
	}
	
	else {
	    	if (searchReg(&appList, app_name, np_name) == -1) {
			PRINTF("> %s %d unregisterApp: REGISTRATION FOUND.\n", __FILE__ , __LINE__);
			delNpFromApp(&appList, app_name, np_name);
			decrNpAppCnt(&npList, np_name);
		}
		else {
			PRINTF("NJ : Invalid argument to app_unregister : Registration not found\n");
		}

	}
	
	
	pthread_mutex_unlock(&np_list_mutex);
  	pthread_mutex_unlock(&app_list_mutex);
  	
	return 1;
}

/*FUNCTION TO REGISTER AN NP*/
int registerNp(char *buff)
{
	char np_name[32], usage[512], delimusage[3] = "==";
	char *s;
	char **keyVal;
	main_np_node *new;
	
	strcpy(np_name, strtok(buff, delimusage));
	s = strtok(NULL, delimusage);
	
	if (s == NULL) {
		PRINTF("Enter usage\nExiting\n");
		return -1;
	}
	
	strcpy(usage, s);
	extractKeyVal(usage, &keyVal);
	pthread_mutex_lock(&np_list_mutex);	
	new = searchNp(&npList, np_name);					/* Search for already existing registration */
	
	if(new != NULL) {
		delNp(&npList, np_name);					/* Delete already existing registration to modify */ 											/* parameters */
	}
	
	addNp(&npList, np_name, usage, &keyVal);
	printNp(&npList);
	pthread_mutex_unlock(&np_list_mutex);
	
	return 1;
}

void extractKeyVal(char *usage, char ***keyVal)
{
	int cnt = 0, i = 0;
	char copyusage[512];
	char *ptr;
	
	
	strcpy(copyusage, usage);
	cnt = countArgs(copyusage, "::");					/* Counting no. of arguments */
	
	*keyVal = (char **)malloc((cnt + 1) * sizeof(char *));
	ptr = strtok(copyusage, "##");

	(*keyVal)[i] = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));

	strcpy((*keyVal)[i], ptr);

	for (i = 1; i < cnt; i++) {
		ptr = strtok(NULL, "##");
		if (!ptr) {
			break;
		}
		if (!((*keyVal)[i] = (char *)malloc(sizeof(char) * 128)))
			perror("MALLOC IS THE CULPRIT");
		strcpy((*keyVal)[i], ptr);
	}
	(*keyVal)[i] = NULL;
	return;
}

int countArgs(char *myString, char *delim) {
	int count = 0;
	const char *tmp = myString;

	while ((tmp = (strstr(tmp, delim)))) {
		count++;
		tmp++;
	}
	return count;
}

/*FUNCTION TO UNREGISTER AN NP*/
int unregisterNp(char *buff)
{
	char np_name[32];
	char delim[3] = "::";
	int app_cnt;
	app_node *aptr = appList.head;
	np_node *p, *q;
	
	strcpy(np_name, strtok(buff, delim));

    	pthread_mutex_lock(&app_list_mutex);
    	pthread_mutex_lock(&np_list_mutex);


	app_cnt = appList.count;
    	if(searchNp(&npList, np_name) != NULL) {
		while(app_cnt != 0) {
		 	if(searchReg(&appList, aptr->data, np_name) == -1) {
		 		p = aptr->np_list_head;
		 		while(p != NULL) {
		 			q = p;
		 			if(!strcmp(p->name, np_name)) {	
		 				PRINTF("> %s %d unregisterNp:Np node found : %s\n", __FILE__ , __LINE__, np_name);
		 				break;	
		 			}
		 			p = p->next;
		 		}
		 		if(aptr->np_count == 1) {
		 			aptr->np_list_head = NULL;
		 			free(p);
		 			aptr->np_count--;
		 		}
		 		else if(p == q) {						/* If node to be deleted is the first node */
		 			q = p->next;
		 			aptr->np_list_head = q;
		 			free(p);
		 			aptr->np_count--;
		 		}
		 		
		 		else {
			 		q->next = p->next;
			 		free(p);
			 		aptr->np_count--;
			 	}
		 	}
			aptr = aptr->next;
			app_cnt--;	   	
    		}
    		delNp(&npList, np_name);
	
	}
	else {
		PRINTF(">%s %d unregisterNp:Np not registerd.\n", __FILE__ , __LINE__);
		return -1;
	}
	
	printNp(&npList);
	printApp(&appList);
    	pthread_mutex_unlock(&np_list_mutex);
    	pthread_mutex_unlock(&app_list_mutex);
    	
	return 1;
}

/*FUNCTION TO GET NOTIFICATION FOR APP*/
char *getnotifyApp(char *buff)
{	
	char *notification;
	struct getnotify_thread_args *arguments;
	pthread_t tid_app_np_gn;
	
	fprintf(logfd, "6. buf is %s \n", buff);
	force_logs();

	notification = (char *)malloc(1024 * sizeof(char));
	arguments = (struct getnotify_thread_args *)malloc(sizeof(struct getnotify_thread_args));
	strcpy(arguments->argssend, buff);


	/* Fork thread to invoke the NP's code with the arguments given in the structure 'arguments', which contains argssend and argsrecv */	fprintf(logfd, "7. buf is %s \n", arguments->argssend);
	
	force_logs();
	if (pthread_create(&tid_app_np_gn, NULL, &NpGetNotifyMethod,(void *)arguments) == 0) {
		PRINTF(" > %s %d getnotifyApp :Pthread_Creation successful for getnotify\n", __FILE__ , __LINE__);
	}
	
	pthread_join(tid_app_np_gn, NULL);
	
	strcpy(notification, arguments->argsrecv);
	
	free(buff);
	buff = NULL;
	
	free(arguments);
	arguments = NULL;
	
	return notification;	/*it will be callers responsibility to free the malloced notification string */
}

/* MAIN CODE */
int main()
{
	sigset_t mask;
	struct thread_args stat, app_reg, np_reg, app_unreg,np_unreg, app_getnotify;
	pthread_t tid_stat, tid_app_reg, tid_app_unreg, tid_np_reg, tid_np_unreg, tid_app_getnotify;
	
	if (!(logfd = fopen("Logs", "a+"))) {
		perror("NJ.C   : not able to open  Log file\n");
	    	return 1;
	}
	

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	signal(SIGINT, sigintHandler);
	
	/* first remove(unlink) the sockets if they already exist */
	unlink(StatSocket);
	unlink(AppReg);
	unlink(AppUnReg);
	unlink(NpReg);
	unlink(NpUnReg);
	unlink(AppGetnotify);

	fd_pidnames = open("File_PIDS.txt", O_CREAT | O_RDWR, 0777);

	/*Initialization of all mutexes */
	if (pthread_mutex_init(&app_list_mutex, NULL) != 0) {
		PRINTF("NJ.C   : \n mutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&np_list_mutex, NULL) != 0) {
		PRINTF("NJ.C   : \n mutex init failed\n");
		return 1;
	}
	
	if (pthread_mutex_init(&getnotify_socket_mutex, NULL) != 0) {
		PRINTF("NJ.C   : \n mutex init failed\n");
		return 1;
	}
	
	
	/*Initialize NP List */
	pthread_mutex_lock(&np_list_mutex);
	initNp(&npList);
	pthread_mutex_unlock(&np_list_mutex);	
	
	/*Initialize APP List */
	pthread_mutex_lock(&app_list_mutex);
	initApp(&appList);
	pthread_mutex_unlock(&app_list_mutex);
	
	/* CREATE SOCKET FOR STAT */
	stat.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	PRINTF("> %s %d main :Socket stat created\n",__FILE__ , __LINE__);
	if (stat.sock < 0) {
		perror("opening stream socket");
		exit(1);
	}
	stat.server.sun_family = AF_UNIX;
	strcpy(stat.server.sun_path, StatSocket);
	if (bind(stat.sock, (struct sockaddr *)&stat.server, sizeof(struct sockaddr_un))) {
		perror("binding stream socket");
		exit(1);
	}

	/* CREATE SOCKET FOR APPLICATION REGISTRATION */
	app_reg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	PRINTF("> %s %d main : Socket app_reg created\n",__FILE__ , __LINE__);
	if (app_reg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}
	app_reg.server.sun_family = AF_UNIX;
	strcpy(app_reg.server.sun_path, AppReg);
	if (bind(app_reg.sock, (struct sockaddr *)&app_reg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	/* CREATE SOCKET FOR APPLICATION UNREGISTRATION */
	app_unreg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (app_unreg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}
	app_unreg.server.sun_family = AF_UNIX;
	strcpy(app_unreg.server.sun_path, AppUnReg);

	if (bind(app_unreg.sock, (struct sockaddr *)&app_unreg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	/* SOCKET FOR NP REGISTRATION */
	np_reg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (np_reg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}
	np_reg.server.sun_family = AF_UNIX;
	strcpy(np_reg.server.sun_path, NpReg);
	if (bind(np_reg.sock, (struct sockaddr *)&np_reg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	/* SOCKET FOR NP UNREGISTRATION */
	np_unreg.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (np_unreg.sock < 0) {
		perror("NJ.C   : opening stream socket");
		exit(1);
	}
	np_unreg.server.sun_family = AF_UNIX;
	strcpy(np_unreg.server.sun_path, NpUnReg);
	if (bind(np_unreg.sock, (struct sockaddr *)&np_unreg.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}
	
	/* CREATE SOCKET for APP SIDE GETNOTIFY */
	app_getnotify.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (app_getnotify.sock < 0) {
		perror("NJ.C   : opening stream socket for getnotify");
		exit(1);
	}
	app_getnotify.server.sun_family = AF_UNIX;
	strcpy(app_getnotify.server.sun_path, AppGetnotify);
	if (bind(app_getnotify.sock, (struct sockaddr *)&app_getnotify.server, sizeof(struct sockaddr_un))) {
		perror("NJ.C   : binding stream socket");
		exit(1);
	}

	/* FORK THREADS TO LISTEN  AND ACCEPT */
	if (pthread_create(&tid_stat, NULL, &PrintStat, (void *)&stat) == 0) {
		PRINTF("> %s %d main: Pthread_Creation successful for stat\n",__FILE__ , __LINE__);
	}
	
	if (pthread_create(&tid_app_reg, NULL, &AppRegMethod, (void *)&app_reg) == 0) {
		PRINTF(">%s %d main : Pthread_Creation successful for AppRegMethod \n",__FILE__ , __LINE__);
	}
	if (pthread_create(&tid_app_unreg, NULL, &AppUnRegMethod, (void *)&app_unreg) == 0) {
		PRINTF("> %s %d main    : Pthread_Creation successful for AppUnRegMethod\n",__FILE__ , __LINE__);
	}
	if (pthread_create(&tid_np_reg, NULL, &NpRegMethod, (void *)&np_reg) == 0) {
		PRINTF("> %s %d main : Pthread_Creation successful NpRegMethod\n",__FILE__ , __LINE__);
	}
	if (pthread_create(&tid_np_unreg, NULL, &NpUnRegMethod, (void *)&np_unreg) == 0) {
		PRINTF("> %s %d : Pthread_Creation successful NpUnRegMethod\n",__FILE__ , __LINE__);
	}
	if (pthread_create(&tid_app_getnotify, NULL, &AppGetNotifyMethod, (void *)&app_getnotify) == 0) {
		PRINTF(">%s %d: Pthread_creation of getnotify thread successful AppGetNotifyMethod\n",__FILE__ , __LINE__);
	}

	/*Join all the threads */
	pthread_join(tid_np_reg, NULL);
	pthread_join(tid_np_unreg, NULL);
	pthread_join(tid_app_reg, NULL);
	pthread_join(tid_app_unreg, NULL);
	pthread_join(tid_app_getnotify, NULL);

	pthread_mutex_destroy(&app_list_mutex);
	pthread_mutex_destroy(&np_list_mutex);
	
	return 0;

}

/* APP REGISTER METHOD THAT WILL RUN IN THREAD FORKED FOR APP REGISTRATION*/
void *PrintStat(void *arguments)
{
	struct thread_args *args = arguments;
	

	listen(args->sock, QLEN);
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("accept");
		else {
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval = read(args->msgsock, args->buf, 1024)) < 0)
					perror("reading stream message");
				else if (args->rval == 0) {
					PRINTF("\n>%s %d PrintStat :Printing Statistics :\n",__FILE__ , __LINE__);
					printStat();
				}
			} while (args->rval > 0);

		}
		close(args->msgsock);
	}

	close(args->sock);
	unlink(StatSocket);

	pthread_exit(NULL);
	return NULL;
}

void *AppRegMethod(void *arguments)
{
	struct thread_args *args = arguments;
	

	listen(args->sock, QLEN);
	int i = 0;
	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {
				bzero(args->buf, sizeof(args->buf));
				if ((args->rval = read(args->msgsock, args->buf, 1024)) < 0)
					perror("NJ.C   : reading stream message");
				else if (args->rval == 0)
					PRINTF("> %s %d AppRegMethod   : Ending connection\n",__FILE__ , __LINE__);
				else {	/*code to register application ie add entry in list */
					i = registerApp(args->buf);
					if (i < 0) {
						PRINTF("> %s %d: Error in registering, try again\n",__FILE__ , __LINE__);
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
	struct thread_args *args = arguments;
	

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
					PRINTF(">%s %d AppUnRegMethod  : Ending connection\n",__FILE__ , __LINE__);
				else {
					i = unregisterApp(args->buf);

					if (i < 0)
						PRINTF("> %s %d AppUnRegMehod: Error in registering\n",__FILE__ , __LINE__);

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
	struct thread_args *args = arguments;
	

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
					PRINTF("> %s %d NpRegMethod: Ending connection\n",__FILE__ , __LINE__);
				else {	/*code to register application ie add entry in list */
					i = registerNp(args->buf);
					if (i < 0)
						PRINTF(">%s %d NpRegMethod: Error in registering\n",__FILE__ , __LINE__);
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
	struct thread_args *args = arguments;

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
					perror("NJ.C   : reading stream message");
				else if (args->rval == 0)
					PRINTF(">%s %d NpUnRegMethod: Ending connection\n",__FILE__ , __LINE__);
				else {
					i = unregisterNp(args->buf);

					if (i < 0)
						PRINTF("> %s %d NpUnRegMethod: Error in registering\n",__FILE__ , __LINE__);

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
	struct thread_args *args = arguments;
	struct proceed_getn_thread_args *sendargs;
	struct sockaddr_un server;
	int i = 0;
	
		
	
	fprintf(logfd,"> %s %d AppGetNotifyMethod: In AppGetNotify\n",__FILE__ , __LINE__);
	fprintf(logfd,"> %s %d AppGetNotifyMehod: args -> msgsock - %d\n",__FILE__ , __LINE__, args->msgsock);

	listen(args->sock, QLEN);

	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, AppGetnotify);
	pthread_t threadarr[APPLIMIT];

	for (;;) {
		args->msgsock = accept(args->sock, 0, 0);
		if (args->msgsock == -1)
			perror("NJ.C   : accept");
		else
			do {
				pthread_mutex_lock(&getnotify_socket_mutex);
				sendargs = (struct proceed_getn_thread_args *)malloc(sizeof(struct proceed_getn_thread_args));
			
				bzero(args->buf, sizeof(args->buf));
				args->rval = read(args->msgsock, args->buf, 1024);
				strcpy(sendargs->buf, args->buf);
			   	fprintf(logfd, ">%s %d AppGetNotifyMethod : 1. buf is %s \n",__FILE__ , __LINE__,sendargs->buf);
				if (args->rval < 0) {
					perror("NJ.C   : reading stream message");
					    
					     pthread_mutex_unlock(&getnotify_socket_mutex);
					}    
				else if (args->rval == 0) {
					PRINTF("> %s %d : Ending connection\n",__FILE__ , __LINE__);
					 pthread_mutex_unlock(&getnotify_socket_mutex);
			}
				else {
					PRINTF("> %s %d AppGetNofifyMethod %d is i in ProcerdGetnotify \n",__FILE__ ,__LINE__,i);
					if (pthread_create(&threadarr[i++], NULL, &ProceedGetnotifyMethod, (void *)sendargs) == 0) {
						perror("NJ.C   : Pthread_Creations for ProceedgetnotifyMethod\n");
					}
					pthread_mutex_unlock(&getnotify_socket_mutex);
					//ProceedGetnotifyMethod(arguments);
					break;
				}

			} while (args->rval > 0);
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
{
	char args_send_copy[1024], args_send_copy_2[1024], rough[1024], r[1024], appname[64], np_name[64], one[512];
	char delimattr[3] = "##", delimval[3] = "::";
	char *error, *countkey, *filename;
	char **pointer;    	
    	int j, filefd, al, count = -1, k, x;
	struct getnotify_thread_args *bargs, *args; 
	struct extr_key_val *temp, *m, *p;
 	np_node *nptr;
 	main_np_node *np_node;	
 	int pid;
 	pthread_mutex_lock(&app_list_mutex);
 	pthread_mutex_lock(&np_list_mutex);

	bargs = (struct getnotify_thread_args *)arguments;
	args = (struct getnotify_thread_args *)malloc(sizeof(struct getnotify_thread_args)); 



/*here we need to call Np_specific getnotify to get the notificationstring..this notificationstring is then copied to arguments->argsrecv
for now I am sending the received strig directly as notificationstring for now I have simply returned the string receive1::value1 */
	*args = *bargs;

	fprintf(logfd, "> %s %d NpGetNotifyMethod : 8. buf is %s \n",__FILE__ , __LINE__, args->argssend);
	void (*getnotify) (struct getnotify_thread_args *);
	
	
	PRINTF("> %s %d NpGetNotifyMethod started args->argssend is %s\n",__FILE__ , __LINE__, args->argssend);
	strcpy(rough, args->argssend);
	strcpy(r, rough);

    	strcpy(args_send_copy_2, args->argssend);

    	fprintf(logfd, "> %s %d NpGetNotifyMethod:9. buf is %s \n",__FILE__ , __LINE__, args->argssend);
    	filename = getFilename(args_send_copy_2);
    	fprintf(logfd, "> %s %d NpGetNotifyMehod:10. buf is %s \n",__FILE__ , __LINE__, args->argssend);
    	
	force_logs();
    
    
	/* INOTIFY needs npname::<npname>##pathname::<pathname>##flags::<flags> */



	strtok(r, delimattr);
	j = strlen(r);
	strcpy(r, args->argssend);
	strcpy(r, &(r[j + 2]));
	strcpy(args->argssend, r);

	strcpy(appname, strtok(rough, delimattr));
	pid = atoi(appname);
	fprintf(logfd, "> %s %d NpGetNotifyMethod:77. pid is %d \n",__FILE__ , __LINE__, pid);
	strcpy(one, strtok(NULL, delimattr));
	strtok(one, delimval);	/* HERE */
	strcpy(np_name, strtok(NULL, delimval));

	/* Get the reg, Malloc space for a getn registration key_val  */

	nptr = getReg(&appList, appname, np_name);
	
	if (nptr) {
		PRINTF("\n> %s %d NpGetNotifyMethod: RETURNED nptr->name = %s\n",__FILE__ , __LINE__, nptr->name);
	}
	else {
	
	    PRINTF(">%s %d NpGetNotifyMethod:Regitration not found---------\n\n",__FILE__ , __LINE__);
	
	}


	temp = (struct extr_key_val *)malloc(sizeof(struct extr_key_val));

	temp->next = NULL;
	
	
	m = nptr->key_val_ptr;
	p = m;


	if (m == NULL) {
		nptr->key_val_ptr = temp;
	} else {
		while (m != NULL) {
            		p = m; 
			m = m->next;
		}
		p->next = temp;
		temp->next = NULL;  
	}


   
	extractKeyVal(args->argssend, &pointer);
	np_node = searchNp(&npList, np_name);
	k = compareArray(&(np_node->key_val_arr), &pointer);
	
    	if(k == 0)
		;
    	else {
        	PRINTF(">%s %d NpGetNotifyMethod:Array not matched..\n",__FILE__ , __LINE__);	
        	return NULL;
	}
	temp->key_val_arr = pointer;
//	printApp(&appList);
    	forwardConvert(&(np_node->key_val_arr),&pointer, args->argssend);

	/* HANDLE NO NAME AND STUFF */
 	
 	countkey = getValFromArgs(args->argssend, "count");
    	count = atoi(extractVal(countkey));
    	fprintf(logfd, ">%s %d NpGetNotifyMethod :99. count is %d! for buf %s!\n",__FILE__ , __LINE__, count, args->argssend);
	
 	x = getNpAppCnt(&npList, np_name);

	/* App count will be 0 initially. App can call getnotify only after NP has registered, and doing App_getnotify for the first time increments the count to 1. Therefore compare count with 1 to do dlopen() for the first time */
	if (x == 1) {
		/* Do dlopen() */
		PRINTF(">%s %d NpGetNotifyMethod: Opening library.\n",__FILE__ , __LINE__);
		handle = dlopen("./libnpinotify.so", RTLD_LAZY);
		if (!handle) {
			perror("NJ.C   : Problem with dlopen :");
			exit(1);
		}

		PRINTF("> %s %d NpGetNotifyMethod : dlopen successful\n",__FILE__ , __LINE__);

	}
	/* Do dlsym() for getnotify() */

		getnotify = dlsym(handle, "getnotify");
		if ((error = dlerror()) != NULL) {
			perror("NJ.C   : chukla");
			exit(1);
		}

	//	PRINTF("NJ.C   : Handle Successful\n");
		
		strcpy(args_send_copy, args->argssend);	
	for(;count != 0; count--) {
	    
	    
	    //open
	    if (!(filefd = open(filename, O_CREAT | O_APPEND | O_RDWR , 0777))) {
			    perror("NJ.C   : not able to open file\n");
			    return NULL;
		    }
	    
			fprintf(logfd, ">%s %d NpGetNotifyMethod :11 %d. buf is %s \n",__FILE__ , __LINE__, count, args->argssend);
			
	force_logs();
		(*getnotify) (args);
		PRINTF(">%s %d NpGetNotifyMethod: Recd = %s\n",__FILE__ , __LINE__, args->argsrecv);
		
		//write
		al = write((int)filefd, args->argsrecv, strlen(args->argsrecv));
	    if (al < 0)
		    perror("NJ.C   : Fwrite failed");
	    else
		    PRINTF(">%s %d NpGetNotifyMethod : %s written  %d bytes WRITTEN\n",__FILE__ , __LINE__, filename, al);
		
		strcpy(args->argssend, args_send_copy);
		//send signal call kill pid 
		if (kill(pid, SIGUSR1) == -1) {
			perror("Error in kill :\n");
			    // CHANGE TO PRINTF LATER
		}
		//close
		close(filefd);
		
		//to do kill here
		
	}
	/*Dlclose       */
	/* dlclose(handle); */
	/*HANDLE CLOSING PROPERLY. WHEN TO CLOSE */

	PRINTF("> %s %d NpGetNotify: ARGSRECV IN THREAD HANDLER = %s\n",__FILE__ , __LINE__, args->argsrecv);
	strcpy(bargs->argsrecv, args->argsrecv);
	
	
	free(args);
	args = NULL;
	
	
	pthread_mutex_unlock(&np_list_mutex);
	pthread_mutex_unlock(&app_list_mutex);

	pthread_exit(NULL);

	/* Functions have to be written such that the argument will be the address of struct args */
}

void decAllNpCounts(app_dcll * appList, np_dcll * npList, char *app_name)
{

	app_node *ptrapp;
	np_node *ptrnp;
	int cnt;
	
	// Find the app in the list. If it is found, for every np in its trailing list; visit the np_dcll and decrement its count.
	PRINTF("> %s %d decAllNpCounts :All nps deleted, since np_name was passed NULL\n",__FILE__ , __LINE__);
	ptrapp = searchApp(appList, app_name);
	if (ptrapp != NULL) {	// App found

		cnt = ptrapp->np_count;
		ptrnp = ptrapp->np_list_head;
		while (cnt) {
			decrNpAppCnt(npList, ptrnp->name);
			ptrnp = ptrnp->next;
			cnt--;
		}

	} else {

		PRINTF("> %s %d decAllNpCounts:Error Asked to delete non-existent application\n",__FILE__ , __LINE__);

	}

}

void *ProceedGetnotifyMethod(void *arguments)
{
	char *received, *buf;
	struct proceed_getn_thread_args *temparguments, *args;
	//int pid;
	char rough[1024];
	char spid[32];
	char choice;
	int j;	
	int len;
	char filename[64], filename1[64];
	
	
	temparguments = (struct proceed_getn_thread_args *)arguments;
	args = (struct proceed_getn_thread_args *)malloc(sizeof(struct proceed_getn_thread_args));

	*args = *temparguments;
	fprintf(logfd, "> %s %d ProceedGetNotifyMethod2. buf is %s \n",__FILE__ , __LINE__, args->buf);
	
	
	force_logs();
	
	strcpy(rough, args->buf);
	
	if(args->buf == NULL)
		PRINTF("> %s %d ProceedGetnotifyMethod args->buf is NULL\n\n",__FILE__ , __LINE__);
	
	len = strlen(rough);
	choice = rough[len - 1];

	strcpy(spid, strtok(rough, "##"));
	//pid = atoi(spid);
    	for(j = 0; j < len ; j++) {
			if(rough[j] == '#')
			if(rough[j+1] == '#')
					break;
		}

	j = strlen(rough);
	

	strcpy(filename, "./");
	strcat(filename, spid);
	strcpy(filename1, spid);
	strcat(filename, ".txt");
	strcat(filename1, ".txt");
	strcat(filename1, "\n");
	   
	fprintf(logfd, "> %s %d ProceedGetnotifyMethod :3. buf is %s \n",__FILE__ , __LINE__, args->buf);  
	force_logs();
 
	buf = (char *)malloc(sizeof(char) * (strlen(args->buf) + 1 )); 
 	strcpy(buf, args->buf);
	received = getnotifyApp(buf);
	while(received == NULL) {
	        pthread_exit(NULL);
	}
	       
	fprintf(logfd, "> %s %d ProceedGetnotifyMethod :4. Notification received from getnotifyApp is %s \n",__FILE__ , __LINE__, received); 
	force_logs();
	
	if (choice == 'N') {
		force_logs();
		strcat(received, "\n");
		write((int)fd_pidnames, filename1, strlen(filename1));
		PRINTF("> %s %d ProceedGetnotifyMehod: PID filename is written successfully.....\n",__FILE__ , __LINE__);
		
	}
	
	if (choice == 'B') {
	    strcat(received, "\n");
	    write((int)fd_pidnames, filename1, strlen(filename1));
	    PRINTF("> %s %d ProceedGetnotifyMethod: PID filename is written successfully.....\n",__FILE__ , __LINE__);
	}
	
	free(received);
	received = NULL;
	free(temparguments);
	temparguments = NULL;
	free(args);
	args = NULL;
	
	pthread_exit(NULL);
}


char* extractKey(char *key_val) {
     	char *key, *ptr;
    	char temp[128];
    	
    	strcpy(temp, key_val);
	ptr = strtok(temp, "::");
	key = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));
	strcpy(key, ptr);
	return key;
}

char* extractVal(char *key_val) {
    
    	char *ptr, *val;
    	char temp[128] ;
    
    	strcpy(temp, key_val);
    	ptr = strtok(temp, "::");
    	ptr = (key_val + strlen(ptr) + 2);
    	if(!ptr) 
		PRINTF(">%s %d extractVal:RETURNED NULL",__FILE__ , __LINE__);
    	val = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));
    	strcpy(val, ptr);
    	return val;
}

int compareArray(char *** np_key_val_arr, char *** getn_key_val_arr) {
      	char **one, **two, *key_one, *key_two;
      	int found = 0;
      	
        one = *getn_key_val_arr;
        while(*one != NULL) {
            	found = 0;
            	key_one = extractKey(*one);
            	two = *np_key_val_arr;
            	while(*two != NULL) {
                	key_two = extractKey(*two);
                	if(!(strcmp(key_one, key_two))) {
                    		found = 1;
                    		break;
                	}
                	two++;
            	}
            	if(found == 0) {
                	PRINTF("> %s %d compareArray:ERROR NP cannot process key %s\n",__FILE__ , __LINE__, key_one);
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

void forwardConvert(char ***np_key_val_arr,char ***getn_key_val_arr , char * fillit) {
    	char **one, **two, *key_one, *key_two;
    	int found = 0;
    	char ret_string[512];

    	strcpy(ret_string,"\0");
    	two = *np_key_val_arr;
    	while(*two != NULL) {
        	found = 0;
        	key_two = extractKey(*two);
        	one = *getn_key_val_arr;
        	while(*one != NULL) {
            		key_one = extractKey(*one);
            		if(!(strcmp(key_one, key_two))) {
                    		strcat(ret_string, *one);
                    		strcat(ret_string, "##");
                    		found = 1;
                    		break;
                	}
            		one++;
        	}
        	if(found == 0) {
            		strcat(ret_string, *two);
            		strcat(ret_string, "##");
        	}
      		two++;
    	}
    	strcpy(fillit, ret_string);
    	fillit[(strlen(fillit)) - 2] = '\0';
    	return;
}


char* getValFromArgs(char *usage, char* key) {   
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

    	retstr = (char *)malloc(sizeof(char) * i);

    	strcpy(retstr, localkeyval);
    	return retstr;
}
    
char *getFilename(char *argsbuf) {

	char rough[1024], *retstr;
	char spid[32], filename[64];

	strcpy(rough, argsbuf);
	strcpy(spid, strtok(rough, "##"));
	strcpy(filename, "./");
	strcat(filename, spid);
	strcat(filename, ".txt");
	PRINTF("> %s %d getFilename: Filename:%s\n\n",__FILE__ , __LINE__, filename);
	retstr = (char*)malloc(sizeof(char) * (strlen(filename) + 1));
	strcpy(retstr, filename);
    	return retstr;
}
