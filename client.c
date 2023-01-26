#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <inttypes.h>//For SCNu8
#include "globalVariable.h"
//參考https://stackoverflow.com/questions/63185837/tcp-what-if-client-call-close-before-server-accept
void func(int sockfd)
{
	uint8_t buffer[BUFFERSIZE];
	char inputMode[7];
	char inputFilename[20];
	char inputPermision[7];
	struct sendInfo si;
	memset(&si, 0, sizeof(struct sendInfo));
	printf("Please enter your name: ");
	scanf("%s",si.ownerName);
	printf("\nPlease enter the selected group number (1:AOS 2:CSE):");
	//scanf("%d",si.groupNumber);
	scanf("%" SCNu8, &(si.groupNumber));
    while(1)
	{
		bzero(buffer, BUFFERSIZE);
		//read(connfd, buffer, sizeof(buff));
		printf("\nPlease enter the following command:\n");
		printf("create filename permision (ex: create homework2.c rwr---)\n");
		printf("read filename (ex: read homework2.c)\n");
		printf("write filename overrite/append (ex: write homework2.c o/a)\n");
		printf("mode filename permision (ex: mode homework2.c rw----)\n");
		printf("exit\n\n");
        //scanf("%s%s%s",si.inputMode,si.inputFilename,si.inputPermision);
        scanf("%s",si.inputMode);
		if(strcmp(si.inputMode,"exit")==0)
		{
			write(sockfd, "exit", 5);
			sleep(1);
			return;
		}
        scanf("%s",si.inputFilename);
		if(strcmp(si.inputMode,"read")!=0) scanf("%s",si.inputPermision);
		else strcpy(si.inputPermision,"");
		// printf("%s,%s,%s\n\n",si.inputMode,si.inputFilename,si.inputPermision);
		getchar();
        write(sockfd, &si, sizeof(struct sendInfo));
        bzero(buffer, sizeof(buffer));
		//From Server : 
        read(sockfd, buffer, sizeof(buffer));
		//printf("\nsock%d:%s\n",sockfd,buffer);
		if(strcmp(buffer,"File exist")==0)
		{
			char filecontentString[BUFFERSIZE];
			printf("Enter your file content:\n");
			scanf("%[^\n]",filecontentString);
			//printf("Client check:%s\n",filecontentString);
			write(sockfd, filecontentString, BUFFERSIZE);
			read(sockfd, buffer, sizeof(buffer));
			//printf("\n%s\n",buffer);
		}
		
		if(strcmp(si.inputMode,"read")==0)
		{
			if(strncmp(buffer,"You do not have permission",26)!=0)
				printf("File content :\n%s\n\n", buffer);
			else
				printf("\n%s\n",buffer);
		}
		else
			printf("\n%s\n",buffer);
    }
}
int main() 
{
	int sockfd;
    struct sockaddr_in servaddr;
 
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
 
    // function for chat
    func(sockfd);
 
    // close the socket
    close(sockfd);
	
    return 0;
}
