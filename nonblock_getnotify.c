#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>


#define NAME "./app_getnotify"

main(int argc, char *argv[]) 
{	int sock,msgsock,rval; 
 	struct sockaddr_un server; 
	char buf[1024];
	char data[1024];
	strcpy(data, argv[1]);

	if (argc < 2) { 
		printf("APP_GETNOTIFY : usage : %s NPNAME::<NPNAME>## AND OTHER ARGS", argv[0]); 
		exit(1); 
	}

		sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK , 0); 
	if (sock < 0) { 
		perror("APP_GETNOTIFY : ERROR OPENING STREAM SOCKET :"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, NAME);
	if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		close(sock); 
		perror("APP_GETNOTIFY : ERROR CONNECTING STREAM SOCKET :");  
	 	exit(1); 
	} 
	if (write(sock, data, sizeof(data)) < 0) 
		perror("APP_GETNOTIFY : ERROR WRITING COMMAND ON STREAM SOCKET :"); 
	
	if( (rval =  read(sock, buf, 1024)) < 0 )
		perror("APP_GETNOTIFY : ERROR READING STREAM SOCKET IN CLIENT \n");
	else 
		printf("APP_GETNOTIFY : NOTIFICATION RECEIVED BY GETNOTIFY : %s\n",buf);
	
	close(sock); 
 } 
