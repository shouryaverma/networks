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
 * client.c
 * Name: Shourya Verma
 * PUID: 36340138
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
#include <errno.h>

#define SEND_BUFFER_SIZE 2048

int client(char *server_ip, char *server_port) {
	int sockfd, numbytes, rv;
	struct addrinfo hints, *servinfo, *p;
	char buf[SEND_BUFFER_SIZE];

	// get address info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(server_ip, server_port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through results and connect to first
	for(p = servinfo; p != NULL; p = p->ai_next) {

		// create socket
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		// connect to server
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	// check if connection was successful
	if (p == NULL) {
		fprintf(stderr, "\nclient: connection failed\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	fprintf(stderr, "\nclient: connected\n"); // print message

	// read from stdin and send to server
	while ((numbytes = read(STDIN_FILENO, buf, SEND_BUFFER_SIZE)) > 0) {
		int total = 0;
		while (total < numbytes) {
			int n = send(sockfd, buf + total, numbytes - total, 0);
			if (n == -1) {
				perror("send");
				close(sockfd);
				return 3;
			}
			total += n;
		}
	}

	// check if read was successful
	if (numbytes == -1) {
		perror("read");
	}

	// close socket
	close(sockfd);
	fprintf(stderr, "\nclient closed\n");
	return 0;
}

/*
 * main()
 * Parse command-line arguments and call client function
 */
int main(int argc, char **argv)
{
  char *server_ip;
  char *server_port;

  if (argc != 3)
  {
	fprintf(stderr, "Usage: ./client-c (server IP) (server port) < (message)\n", argv[0]);
	exit(EXIT_FAILURE);
  }

  server_ip = argv[1];
  server_port = argv[2];
  return client(server_ip, server_port);
}