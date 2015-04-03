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
 *	This file contains code for registering the np with the nj using sockets
 */

#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#define NAME "./np_reg_sock"						/* Socket name for sending parametes */

int main(int argc, char *argv[])
{

	int sock;
	struct sockaddr_un server;
	char *m;
	char data[1024], trash[1024];

	if (argc < 2) {
		printf("NP_REG : Usage:%s npname::<npname>##usage\n", argv[0]);
		exit(1);
	}

	strcpy(data, argv[1]);
	strcpy(trash, data);

	m = strtok(trash, "##");
	if (m == NULL) {
		printf("Error : NP_REG : Enter usage\nExiting\n");
		exit(1);
	} else {
		m = strtok(NULL, "##");

		if (m == NULL) {
			printf("Error : NP_REG : Enter usage\nExiting\n");
			exit(1);
		}
	}

	sock = socket(AF_UNIX, SOCK_STREAM, 0);				/* Creates endpoint for communication */
	if (sock < 0) {
		perror("NP_REG : Opening Stream Socket");
		exit(1);
	}
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, NAME);					/* Setting name of socket */

	if (connect
			(sock, (struct sockaddr *)&server,
			 sizeof(struct sockaddr_un)) < 0) {
		close(sock);						/* Initiate connection on socket */
		perror("NP_REG : Connecting Stream Socket");
		exit(1);
	}
	if (write(sock, data, sizeof(data)) < 0)			/* Writing data on socket */
		perror("NP_REG : Writing On Stream Socket");
	close(sock);
	return 0;

}
