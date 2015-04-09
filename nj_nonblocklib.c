/*
*	This file is for the library that provides the option for non_block,
*	and also provides for the app to call nj internally, i.e. through the application's code
*
*/

#include "lib.h"

/* Register the application by writing the key-value string to the NJ using socket  */

int appRegister(char *key_val_string)
{

	int sock;
	struct sockaddr_un server;
	char data[1024];
	if (key_val_string == NULL) {
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

	if (connect
	    (sock, (struct sockaddr *)&server,
	     sizeof(struct sockaddr_un)) < 0) {
		close(sock);
		perror("APP_REG : Connecting Stream Socket");
		exit(1);
	}
	if (write(sock, data, sizeof(data)) < 0)
		perror("APP_REG : Writing On Stream Socket");
	close(sock);
	return 1;
}

/* UnRegister the application by writing the key-value string to the NJ using socket  */

int appUnregister(char *key_val_string)
{
	int sock;
	struct sockaddr_un server;
	char data[1024];

	if (key_val_string == NULL) {
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

	if (connect
	    (sock, (struct sockaddr *)&server,
	     sizeof(struct sockaddr_un)) < 0) {
		close(sock);
		perror("APP_UNREG : Connecting Stream Socket");
		exit(1);
	}
	if (write(sock, data, sizeof(data)) < 0)
		perror("APP_UNREG : Writing On Stream Socket");
	close(sock);
	return 1;//proper return without error
}

/* Request for notifications from an np, the choice field is B for blocking and N for non-blocking */

int appGetnotify(int pid, char *key_val_string, char choice)
{
	int sock;
	struct sockaddr_un server;
	
	int sock_block,  rval_block, msgsock_block;
	struct sockaddr_un server_block;
	
	
	char buf[1024];
	char data[1024];
	char sock_name[512];
	sprintf(sock_name, "%d", pid);
	printf("pid as a string - %s\n", sock_name);

	if(choice == 'B') {
	// make a socket with pid_sock as the name
		strcat(sock_name, "_sock");
		printf("sock_name is %s\n", sock_name);
	}

	if (key_val_string == NULL) {
		printf("Provide arg string\n");
		return -1;

	}

	printf("Received :\npid = %d, keyvalstr = %s, choice = %c\n", pid,
	       key_val_string, choice);
	char str[16];
	sprintf(str, "%d", pid);
	printf("STRING OF PID IS %s\n", str);
	//strcpy(data, "pid::");
	strcpy(data, str);
	strcat(data, "##");
	strcat(data, key_val_string);
	if (choice == 'B') {
		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		strcat(data, "##TYPE::B");
	} else if (choice == 'N') {
		sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
		strcat(data, "##TYPE::N");
	}

	if (sock < 0) {
		perror("APP_GETNOTIFY : ERROR OPENING STREAM SOCKET :");
		exit(1);
	}
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, AppGetnotify);
	if (connect
	    (sock, (struct sockaddr *)&server,
	     sizeof(struct sockaddr_un)) < 0) {
		close(sock);
		perror("APP_GETNOTIFY : ERROR CONNECTING STREAM SOCKET :");
		exit(1);
	}
	printf(" DATA SEND by nonblock library %s \n", data);
	if (write(sock, data, sizeof(data)) < 0)
		perror
		    ("APP_GETNOTIFY : ERROR WRITING COMMAND ON STREAM SOCKET :");
	if (choice == 'B') {
	
		sock_block = socket(AF_UNIX, SOCK_STREAM, 0);
		
		server_block.sun_family = AF_UNIX;
		strcpy(server_block.sun_path, sock_name);
	
		if (bind(sock_block, (struct sockaddr *)&server_block, sizeof(struct sockaddr_un))) {
		perror("binding stream socket");
		exit(EXIT_FAILURE);
		}
		
		printf("Socket bound\n");
		
		if ((listen(sock_block, QLEN)) != 0) {
		perror("Error in listening on pid_sock socket:");
		}
		
		msgsock_block = accept(sock_block, 0, 0);
	    do {
		    if ((rval_block = read(msgsock_block, buf, 1024)) < 0)
			    perror
			        ("APP_GETNOTIFY : ERROR READING STREAM SOCKET IN CLIENT \n");
		    else
			    printf
			        ("APP_GETNOTIFY : NOTIFICATION RECEIVED BY GETNOTIFY.\n");
		    
	    }while(rval_block >= 0);	
	    
	}
	unlink(sock_name);
	close(sock);
	return 1;
}
