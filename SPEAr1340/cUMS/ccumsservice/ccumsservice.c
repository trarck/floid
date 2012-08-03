/*
 * Compatibility UMS Client - Android 4 service that manages USB Media Storage.
 * Copyright (C) 2012 Vincenzo Frascino <vincenzo.frascino@st.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see .
 *
 */
#define LOG_TAG "cUMSClient"
#include <utils/Log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
			error("ERROR connecting");
	if(argc == 4)
	{
		if (strlen(argv[3]) < 256)
		{
			LOGI("len: [%d]", (int)strlen(argv[3]));
			strncpy(buffer, argv[3], strlen(argv[3]));
			buffer[strlen(buffer) + 1] = '\0';
		} else {
			strncpy(buffer, argv[3], 254);
			buffer[255] = '\0';
		}
	} else {
		error("ERROR on init parameters");
	}
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
		error("ERROR writing to socket");
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0) 
		error("ERROR reading from socket");
	LOGI("%s",buffer);
	close(sockfd);
	return 0;
}
