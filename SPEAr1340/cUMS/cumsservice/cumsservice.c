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
#define LOG_TAG "cUMSService"
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
	LOGI("Compatibility USB Media Storage started");
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char line[512];
	char device[512];
	char e_device[512];
	char buffer[256];
	char mount_status[256];
	int index = 0;
	pid_t pID;
	
	char *cums_args[] = {
   				"setprop",
   				"sys.usb.config",
   				"mass_storage,adb",
   				(char *) 0 
   			};
   	
   	char *adb_args[] = {
   				"setprop",
   				"sys.usb.config",
   				"adb",
   				(char *) 0 
   			};
	
	FILE *cums = NULL;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		portno = 10001;
	} else {
		portno = atoi(argv[1]);
	}
	LOGI("Open cUMS.conf file");
	cums = fopen("/system/etc/cUMS.conf","r");
	if (cums == NULL)
	{
		LOGI("Unable to Open cUMS.conf file");
		return -1;
	} else {
		while (!feof(cums))
		{
			fscanf(cums, "%s", line);
			if (line[0] != '#')
			{
				if (index == 0)
				{
					strcpy(device, line);
					index++;
				} else {
					strcpy(e_device, line);
					break;
				}	
			}	
		}
		fclose(cums);
		LOGI("%s", device);
		LOGI("%s", e_device);
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
		} else
		if (strcmp(buffer, "ADB") == 0)
		{
			LOGI("ADB");
			pID = fork();
			if (pID == 0)
				execvp("setprop", adb_args);
			else 
				wait();
		} else
		if (strcmp(buffer, "CUMS_STATUS") == 0)
		{
			LOGI("CUMS_STATUS.");
			cums = fopen("/sys/class/android_usb/android0/f_mass_storage/lun/file","r+");
			if (cums == NULL)
			{
				LOGI("System does not support USB media storage.");
				return -1;		
			} else {
				fscanf(cums, "%s", line);
				fclose(cums);
				if (strcmp(line, "") == 0)
					strcpy(mount_status, "UMOUNTED\r\n");
			}
			n = write(newsockfd,
				mount_status,
				strlen(mount_status));
		} else
		if (strcmp(buffer, "CUMS_MOUNT") == 0)
		{
			LOGI("CUMS_MOUNT.");
			pID = fork();
			if (pID == 0)
				execvp("setprop", cums_args);
			else
				wait();
			cums = fopen("/sys/class/android_usb/android0/f_mass_storage/lun/file","w+");
			if (cums == NULL)
			{
				LOGI("System does not support USB media storage.");
				return -1;		
			} else {
				fprintf(cums, "%s", device);
				fclose(cums);
			}
			strcpy(mount_status, "MOUNTED\r\n");
		} else
		if (strcmp(buffer, "CUMS_UMOUNT") == 0)
		{
			LOGI("CUMS_UMOUNT.");
			cums = fopen("/sys/class/android_usb/android0/f_mass_storage/lun/file","w+");
			if (cums == NULL)
			{
				LOGI("System does not support USB media storage.");
				return -1;		
			} else {
				fprintf(cums, "%s", "");
				fclose(cums);
			}
			strcpy(mount_status, "UMOUNTED\r\n");
		} else
		if (strcmp(buffer, "CUMS_E_MOUNT") == 0)
		{
			LOGI("CUMS_E_MOUNT.");
			pID = fork();
			if (pID == 0)
				execvp("setprop", cums_args);
			else
				wait();
			cums = fopen("/sys/class/android_usb/android0/f_mass_storage/lun/file","w+");
			if (cums == NULL)
			{
				LOGI("System does not support USB media storage.");
				return -1;		
			} else {
				fprintf(cums, "%s", e_device);
				fclose(cums);
			}
			strcpy(mount_status, "E_MOUNTED\r\n");	
		} else
		if (strcmp(buffer, "CUMS_E_UMOUNT") == 0)
		{
			LOGI("CUMS_E_UMOUNT.");
			cums = fopen("/sys/class/android_usb/android0/f_mass_storage/lun/file","w+");
			if (cums == NULL)
			{
				LOGI("System does not support USB media storage.");
				return -1;		
			} else {
				fprintf(cums, "%s", "");
				fclose(cums);
			}
			strcpy(mount_status, "UMOUNTED\r\n");
		} else
		if (n < 0)
		{
			error("ERROR writing to socket");
			break;
		} else {
			LOGI("Unknown Command.");
		}
	}
	close(newsockfd);
	close(sockfd);
	return 0; 
}
