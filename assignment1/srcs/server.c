/*****************************************************************************
 *
 *     This file is part of Purdue CS 536.
 *
 *     Purdue CS 536 is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Purdue CS 536 is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Purdue CS 536. If not, see <https://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/*
 * server.c
 * Name: Shourya Verma
 * PUID: 36340138
 * worked with Ishaan Jain 30962784
 * also discussed with Arpan Mahapatra and Swathi Jayaprakash
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048

int server(char *server_port) {
	// variables
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;
	int sockfd, new_fd, rv;
	socklen_t sin_size;
	char buf[RECV_BUFFER_SIZE];
	char s[INET6_ADDRSTRLEN];
	int yes = 1;

	// get address info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, server_port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through results and bind to first
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		// allow reuse of port
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			close(sockfd);
			continue;
		}

		// bind to port
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	// check if bind was successful
	if (p == NULL) {
		fprintf(stderr, "\nserver: bind failed\n");
		return 2;
	}

	// listen for incoming connections
	if (listen(sockfd, QUEUE_LENGTH) == -1) {
		perror("listen");
		return 3;
	}

	// print message
	fprintf(stderr, "\nserver: waiting for connections on port %s...\n", server_port);

	while(1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size); // accept incoming connection
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		// print message IP address of client
		inet_ntop(their_addr.ss_family, &(((struct sockaddr_in*)&their_addr)->sin_addr), s, sizeof s);
		fprintf(stderr, "\nserver: connected from %s\n", s);

		int total_bytes = 0;
		int numbytes;

		// receive data from client
		while (1) {
			int numbytes = recv(new_fd, buf, RECV_BUFFER_SIZE - 1, 0);
			if (numbytes <= 0) {
				if (numbytes < 0) perror("recv");
				break;
			}

			// write data to stdout
			if (write(STDOUT_FILENO, buf, numbytes) != numbytes) {
				fprintf(stderr, "error writing to stdout\n");
				break;
			}
			fflush(stdout);
			total_bytes += numbytes;
		}

		// handle errors and close connection
		if (numbytes == -1) {
			perror("recv");
		} else if (numbytes == 0) {
			fprintf(stderr, "\nserver: client disconnected\n");
		}

		fprintf(stderr, "\nserver: recvd total of %d bytes\n", total_bytes);
		close(new_fd);
	}

	return 0;
}

/*
 * main():
 * Parse command-line arguments and call server function
 */
int main(int argc, char **argv)
{
  char *server_port;

  if (argc != 2)
  {
	fprintf(stderr, "Usage: ./server-c (server port)\n",argv[0]);
	exit(EXIT_FAILURE);
  }

  server_port = argv[1];
  return server(server_port);
}