/*
*	The function of this command is to print the registrations and related details, as listed below -
*	NPs registered
*	For every NP, print the count of apps registered with it
* 	Table - NP - Count of Apps
*	Apps registered and the NPs they have registered with
*	Table - App - Pid - Registered with 
*
*	Add as is thought
*
*	The idea is to extract the details from the lists in the NJ.
*
*/



#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define NAME "./statsock"


int main(int argc, char *argv[]) {

	int sock; 
 	struct sockaddr_un server; 
	//char buf[1024];
	//char data[1024];
	//strcpy(data, argv[1]);

	if (argc != 1) { 
		printf("STAT : Usage : %s ", argv[0]); 
		exit(1); 
	}

	sock = socket(AF_UNIX, SOCK_STREAM, 0); 
	if (sock < 0) { 
		perror("STAT : Opening Stream Socket"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, NAME);

	if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		close(sock); 
		perror("STAT : Connecting Stream Socket");  
	 	exit(1); 
	} 
	/*if (write(sock, data, sizeof(data)) < 0) 
		perror("STAT : Writing On Stream Socket"); 
	*/
	close(sock); 
	return 0; 

} 
