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

/*This is header file for nj.c (notification juncitoncode)*/
#ifndef _NJ_H
#define _NJ_H	1

/*Macro*/
#define PRINTF printf 

#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <dlfcn.h>
#include<errno.h>
#define ALREXST -1
#define NOTFND -2
#define ERROR -1
#define SUCCESS 1
#include "app_dcll.h"
#include "np_dcll.h"
#define NONBLOCKING 'N'
#define BLOCKING 'B'
#define LOGS "logs"
#define PIDFILE "File_PIDS.txt"
/* Create 2 sockets - app_reg, app_unreg*/
/* Fork 2 threads - app_reg, app_unreg*/
#define AppGetnotifySocket "app_getn_sock"
#define AppRegSocket "app_reg_sock"
#define AppUnRegSocket "app_unreg_sock"
#define NpRegSocket "np_reg_sock"
#define NpUnRegSocket "np_unreg_sock"
#define StatSocket "statsock"
#define APPLIMIT 32
#define QLEN 32

/*STRUCTURE FOR ARGUMENTS TO BE PASSED TO THREAD*/
typedef struct thread_args {
	int sock, msgsock, rval;
	struct sockaddr_un server;
	char buf[1024];
	/*
	 * sock //
	 * msgsock //
	 * rval is to store return value of read() on the socket
	 * server // 
	 * buf is to send key_val pair argument to proceed_getnotify_method thread
	 */	
} thread_args;

typedef struct proceed_getn_thread_args	 {
	char buf[1024];
	/*
	 * buf is to send key_val pair argument to proceed_getnotify_method thread
	 */
} proceed_getn_thread_args;

/*STRUCTURE FOR ARGUMENTS TO BE PASSED TO GETNOTIFY THREAD*/
typedef struct getnotify_thread_args {
	char argssend[1024];	
	char argsrecv[1024];
	/*
	 * argssend is to send string of required notification request to getnotify thread
	 * argsrecv  tois fill the notification response by getnotify thread
	 */
} getnotify_thread_args;

/* function returns negative value if error in registering app This function will add node in the app list  for first registration of app with particular NP*/
int register_app(char *buff);

/*function returns negative value if error in registering np this function will add node in the np list for np registration of np */
int register_np(char *buff);

/* function returns negative value if error in unregistering app eg(app not found in list)*/
int unregister_app(char *buff);

/*function returns negative valuse if error in unregistering np eg(np not found in list)*/
int unregister_np(char *buff);

/*function to get notification */
char *getnotify_app(char *buff);

/*Function declarations*/

void force_logs();
void *print_stat(void *arguments);
void *app_reg_method(void *arguments);
void *app_unreg_method(void *arguments);
void *np_reg_method(void *arguments);
void *np_unreg_method(void *arguments);
void *app_getnotify_method(void *arguments);
void *np_getnotify_method(void *arguments);
void *proceed_getnotify_method(void *arguments);
void dec_all_np_counts(app_dcll * appList, np_dcll * npList, char *app_name);
void printStat();
void sigint_handler(int signum);

void extract_key_val(char *usage, char ***keyVal);

char* extract_key(char *key_val);

char* extract_val(char *key_val);

int compare_array(char *** np_key_val_arr, char *** getn_key_val_arr);

void forward_convert(char ***np_key_val_arr,char ***getn_key_val_arr, char * fillit);

char *get_val_from_args(char *usage, char* key);

char *get_filename(char *argsbuf);

int count_args(char *usage, char *count_occurence_of);
void nj_exit(void); /*function called whenever NJ exits*/

#endif				/*nj.h */
