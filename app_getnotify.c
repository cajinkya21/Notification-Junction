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

/* The file contains code for requesting the specific NP for specific notification 
 * by sending parameters in the form of key-value pair. 
 */

#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define NAME "./app_getn_sock"						/* Socket name for sending parametes */

int main(int argc, char *argv[])
{
	int sock, rval;
	char buf[1024], data[1024], filename[64];
	int fd;
	struct sockaddr_un server;

	if (argc < 2) {
		printf("APP_GETNOTIFY : usage : %s NPNAME::<NPNAME>## AND OTHER ARGS", argv[0]);
		exit(1);
	}

	strcpy(data, argv[1]);
	sock = socket(AF_UNIX, SOCK_STREAM, 0);				/* Creates endpoint for communication */
	if (sock < 0) {
		perror("APP_GETNOTIFY : ERROR OPENING STREAM SOCKET :");
		exit(1);
	}

	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, NAME);					/* Setting name of socket */

	if (connect
			(sock, (struct sockaddr *)&server,
			 sizeof(struct sockaddr_un)) < 0) {
		close(sock);						/* Initiate connection on socket */
		perror("APP_GETNOTIFY : ERROR CONNECTING STREAM SOCKET :");
		exit(1);
	}

	if (write(sock, data, sizeof(data)) < 0)			/* Writing data on socket */
		perror
			("APP_GETNOTIFY : ERROR WRITING COMMAND ON STREAM SOCKET :");

	strcpy(filename, strtok(data, "##"));
	strcat(filename, ".txt");
	printf("%s\n\n", filename);
	while ((fd = open(filename, O_RDONLY)) < 0) ;

	if ((rval = read(fd, buf, 1024)) < 0)				/* Reading data from socket */
		perror
			("APP_GETNOTIFY : ERROR READING STREAM SOCKET IN CLIENT \n");
	else
		printf
			("APP_GETNOTIFY : NOTIFICATION RECEIVED BY GETNOTIFY : %s\n",
			 buf);

	//After reading delete the file (truncate)

	close(sock);
	return 0;
}
