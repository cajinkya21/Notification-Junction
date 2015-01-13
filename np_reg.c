/*
*	This file contains code for registering the np with the nj using sockets
*/


#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define NAME "./np_reg"

int main(int argc, char *argv[]) {	
	
	int sock; 
 	struct sockaddr_un server; 
	char buf[1024];
	char data[1024];
	char trash[1024];
	strcpy(data, argv[1]);
	strcpy(trash, data);
	
	char *m;
	m = strtok(trash, "##");
	if(m == NULL) {
	    printf("Error : NP_REG : Enter usage\nExiting\n");
	    exit(1);
	}
	else {
	    m = strtok(NULL, "##");
	    printf("Error : NP_REG : Enter usage\nExiting\n");
	    if(m == NULL) {
	    exit(1);
	}
	}

	if (argc < 2) { 
		printf("NP_REG : Usage:%s <np_name>", argv[0]); 
		exit(1); 
	}

	sock = socket(AF_UNIX, SOCK_STREAM, 0); 
	if (sock < 0) { 
		perror("NP_REG : Opening Stream Socket"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, NAME);

	if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		close(sock); 
		perror("NP_REG : Connecting Stream Socket");  
	 	exit(1); 
	} 
	if (write(sock, data, sizeof(data)) < 0) 
		perror("NP_REG : Writing On Stream Socket"); 
	close(sock); 
	return 0; 

} 
