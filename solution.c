#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void server(char*);
void client(char*, char*);

/*
 * TCP client-server application
 */


/***************************************************************
ARGUMENTS:                                                     *
argv[0] = name of program                                      *
argv[1] = s or c (server or client)                            *
argv[2] = port for server or IP adress for conntction of client*
argv[3]<optional> = port for client                            *
***************************************************************/

int main(int argc, char* argv[]) {

	if (argc != 3 && argc != 4) {
		printf("ERROR: you should pass 3 or 4 arguments!\n");
		return EXIT_FAILURE;
	}

	if (argv[1][0] == 's') {

		server(argv[2]);

	} else if (argv[1][0] == 'c'){

		if (argc != 4) {
			printf("ERROR: you should pass third argument for client\n");
			return EXIT_FAILURE;
		}

		client(argv[2], argv[3]);

	} else {

		printf("ERROR: you shild puth second argument only [s, c]\n");
		return EXIT_FAILURE;

	}

	return EXIT_SUCCESS;

}

void server(char* port) {

	struct sockaddr_in local;

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	int client_socket;

	if (server_socket == -1) {

		printf("ERROR: can't start server\n");
		exit(EXIT_FAILURE);

	}

	local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_port = htons(atoi(port));
        local.sin_family = AF_INET;

        int result = bind(server_socket, (struct sockaddr*) &local, sizeof(local));
        if (result == -1) {
                printf("ERROR: bind returned -1\nCan't start server\n");
                exit(EXIT_FAILURE);
        }

	listen(server_socket, 1);

	client_socket = accept(server_socket, NULL, NULL);

	if (client_socket == -1) {
		printf("ERROR: can't connect to client\n");
		exit(EXIT_FAILURE);
	}

        char buf[BUFSIZ];

        for (;;) {
                int bytes = read(client_socket, buf, BUFSIZ - 1);
                buf[bytes] = '\0';

                if (strcmp(buf, "SUDO OFF SERVER\n") == 0 || bytes == 0) {
			sprintf(buf, "SUDO OFF CLIENT");
			write(client_socket, buf, strlen(buf) + 1);
			shutdown( client_socket, SHUT_RDWR );
                        shutdown( server_socket, SHUT_RDWR );
			close(client_socket);
                        close(server_socket);
			break;
                }

		printf("Getting string: %s\n", buf);
		sprintf(buf, "I am server #1. I am getting from you message.\n");
		write(client_socket, buf, strlen(buf) + 1);
        }

}

void client(char* ip, char* port) {

	struct sockaddr_in local;

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	inet_aton(ip, &local.sin_addr);
        local.sin_port = htons(atoi(port));
        local.sin_family = AF_INET;

	int result = connect(server_socket, (struct sockaddr*) &local, sizeof(local));
        if (result == -1) {
                printf("ERROR: bind returned -1\nCan't start client\n");
                exit(EXIT_FAILURE);
        }

        char buf[BUFSIZ];

        for(;;) {
		printf("Enter data that you want send to server: ");
                int bytes = read(0, buf, BUFSIZ - 1);
                buf[bytes] = '\0';
		write(server_socket, buf, bytes);
		read(server_socket, buf, BUFSIZ - 1);
		if (strcmp("SUDO OFF CLIENT", buf) == 0) {
			shutdown( server_socket, SHUT_RDWR );
			close(server_socket);
			break;
		} else {		
			printf("Answer from server: %s", buf);
		}
        }

}
