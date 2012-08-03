/*
 * Powerservice - Android service that manages power status.
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
#define LOG_TAG "PowerService"
#include <utils/Log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
	LOGI("Power Service started");
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	FILE *wake_lock = NULL;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		portno = 10000;
	} else {
		portno = atoi(argv[1]);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
			error("ERROR on binding");
	listen(sockfd,5);
	while(1)
	{  
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, 
				(struct sockaddr *) &cli_addr, 
				&clilen);
		if (newsockfd < 0)
		{
			error("ERROR on accept");
			break;
		}
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0)
		{
			error("ERROR reading from socket");
			break;
		}
		buffer[strlen(buffer)] = '\0';
		LOGI("Here is the message: [%s]",buffer);
		if (strcmp(buffer, "EXIT") == 0)
		{
			LOGI("EXIT");
			break;
		}
		if (strcmp(buffer, "WAKE_LOCK_TRUE") == 0)
		{
			LOGI("WAKE_LOCK_TRUE.");
			wake_lock = fopen("/sys/power/wake_lock","w+");
			if (wake_lock == NULL)
			{
				LOGI("System does not support Wakelocks.");
				return -1;		
			} else {
				fprintf(wake_lock, "%s", "PowerService");
				fclose(wake_lock);
			}
		}
		if (strcmp(buffer, "WAKE_LOCK_FALSE") == 0)
		{
			LOGI("WAKE_LOCK_FALSE.");
			wake_lock = fopen("/sys/power/wake_unlock","w+");
			if (wake_lock == NULL)
			{
				LOGI("System does not support Wakelocks.");
				return -1;		
			} else {
				fprintf(wake_lock, "%s", "PowerService");
				fclose(wake_lock);
			}
		}
		n = write(newsockfd,"I got your message",18);
		if (n < 0)
		{
			error("ERROR writing to socket");
			break;
		}
	}
	close(newsockfd);
	close(sockfd);
	return 0; 
}
