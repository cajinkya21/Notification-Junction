#include "lib.h"

int app_register(char *key_val_string) 
{	int sock; 
 	struct sockaddr_un server; 
	char buf[1024];
	char data[1024];
	if(key_val_string == NULL) {
		printf("Provide arg string\n");		
		return -1;
		
	}
	strcpy(data, key_val_string);
	
	

	sock = socket(AF_UNIX, SOCK_STREAM, 0); 
	if (sock < 0) { 
		perror("APP_REG : Opening Stream Socket"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, AppReg);

	if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		close(sock); 
		perror("APP_REG : Connecting Stream Socket");  
	 	exit(1); 
	} 
	if (write(sock, data, sizeof(data)) < 0) 
		perror("APP_REG : Writing On Stream Socket"); 
	close(sock); 
	return 1;
 } 

int app_unregister(char *key_val_string) {
	int sock; 
 	struct sockaddr_un server; 
	char buf[1024];
	char data[1024];
	
	if(key_val_string == NULL) {
		printf("Provide arg string\n");		
		return -1;
		
	}
	strcpy(data, key_val_string);

	sock = socket(AF_UNIX, SOCK_STREAM, 0); 
	if (sock < 0) { 
		perror("APP_UNREG : Opening Stream Socket"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, AppUnReg);

	if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		close(sock); 
		perror("APP_UNREG : Connecting Stream Socket");  
	 	exit(1); 
	} 
	if (write(sock, data, sizeof(data)) < 0) 
		perror("APP_UNREG : Writing On Stream Socket"); 
	close(sock); 
}

int app_getnotify(char *key_val_string/*, choice:BLOCK 0r NON_BLOCK*/) {
	int sock,msgsock,rval; 
 	struct sockaddr_un server; 
	char buf[1024];
	char data[1024];

	if(key_val_string == NULL) {
		printf("Provide arg string\n");		
		return -1;
		
	}
		
	strcpy(data, key_val_string);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	//call socket with socket(AF_UNIX, SOCK_STREAM | O_NONBLOCK ,0)
	if (sock < 0) { 
		perror("APP_GETNOTIFY : ERROR OPENING STREAM SOCKET :"); 
		exit(1); 
	} 
	server.sun_family = AF_UNIX; 
	strcpy(server.sun_path, AppGetnotify);
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
  	return 1;
}
