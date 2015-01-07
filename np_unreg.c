#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define NAME "./np_unreg"

main(int argc, char *argv[]) 
{	int sock; 
 	struct sockaddr_un server; 
	char buf[1024];
	char data[1024];
	strcpy(data, argv[1]);

	if (argc < 2) { 
		printf("APP_GETNOTIFY : Usage: %s <np_name>", argv[0]); 
		exit(1); 
	}

	sock = socket(AF_UNIX, SOCK_STREAM, 0); 
	if (sock < 0) { 
		perror("APP_GETNOTIFY : Opening Stream Socket"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, NAME);

	if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		close(sock); 
		perror("APP_GETNOTIFY : Connecting Stream Socket");  
	 	exit(1); 
	} 
	if (write(sock, data, sizeof(data)) < 0) 
		perror("APP_GETNOTIFY : Writing On Stream Socket"); 
	close(sock); 
 } 
