/*
*	This file is for the library that provides the option for non_block,
*	and also provides for the app to call nj internally, i.e. through the application's code
*
*/

#include "lib.h"

/* Register the application by writing the key-value string to the NJ using socket  */

int app_register(char *key_val_string)
{

	int sock;
	struct sockaddr_un server;
	char buf[1024];
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

int app_unregister(char *key_val_string)
{
	int sock;
	struct sockaddr_un server;
	char buf[1024];
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
}

/* Request for notifications from an np, the choice field is B for blocking and N for non-blocking */

int app_getnotify(int pid, char *key_val_string, char choice)
{
	int sock, msgsock, rval;
	struct sockaddr_un server;
	char buf[1024];
	char data[1024];

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
		if ((rval = read(sock, buf, 1024)) < 0)
			perror
			    ("APP_GETNOTIFY : ERROR READING STREAM SOCKET IN CLIENT \n");
		else
			printf
			    ("APP_GETNOTIFY : NOTIFICATION RECEIVED BY GETNOTIFY : %s\n",
			     buf);
	}
	close(sock);
	return 1;
}
