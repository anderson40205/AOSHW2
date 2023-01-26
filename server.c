#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h>
#include "globalVariable.h"
#define OWNER 0
#define AOS 1
#define CSE 2
#define fileSize 20
// Function designed for chat between client and server.
struct fileInfo fi[fileSize];
struct capabilityLists cpList[3][fileSize];
uint8_t fileCount = 0;
char returnBuffer[BUFFERSIZE]={0};
void displayCapability() 
{
    printf("**************** Display capability ********************\n");
    printf("OWNER: \n");
    for (int i = 0; i < fileSize; i++) printf("%s|%s|%d|%d , ", cpList[OWNER][i].objFileName, cpList[OWNER][i].owner, cpList[OWNER][i].read, cpList[OWNER][i].write);
    printf("\nAOS: \n");
    for (int i = 0; i < fileSize; i++) printf("%s|%s|%d|%d , ", cpList[AOS][i].objFileName, cpList[OWNER][i].owner, cpList[AOS][i].read, cpList[AOS][i].write);
	printf("\nCSE: \n");
    for (int i = 0; i < fileSize; i++) printf("%s|%s|%d|%d , ", cpList[CSE][i].objFileName, cpList[OWNER][i].owner, cpList[CSE][i].read, cpList[CSE][i].write);
    printf("\n********************************************************\n");
}
void createHandler(int connfd,char *ownerName, uint8_t groupNumber, char *inputFilename, char *inputPermision)
{
	printf("createHandler!\nconnfd=%d\n",connfd);
	// printf("File owner is %s\n",ownerName);
	// printf("File name is %s\n",inputFilename);
	// printf("Permision is %s\n",inputPermision);
	strcpy(cpList[OWNER][fileCount].objFileName, inputFilename);
	strcpy(cpList[AOS][fileCount].objFileName, inputFilename);
	strcpy(cpList[CSE][fileCount].objFileName, inputFilename);
	if(inputPermision[0]=='r')
		cpList[OWNER][fileCount].read = 1;
	else
		cpList[OWNER][fileCount].read = 0;
	if(inputPermision[1]=='w')
		cpList[OWNER][fileCount].write = 1;
	else
		cpList[OWNER][fileCount].write = 0;
	strcpy(cpList[OWNER][fileCount].owner, ownerName);
	if(groupNumber == AOS)
	{
		printf("It's AOS group!\n");
		
		if(inputPermision[2]=='r')
		{ 
			cpList[AOS][fileCount].read = 1;
			//printf("cpList[0][fileCount].read=%d\n",cpList[0][fileCount].read);
		}
		else cpList[AOS][fileCount].read = 0;
		if(inputPermision[3]=='w') cpList[AOS][fileCount].write = 1;
		else cpList[AOS][fileCount].write = 0;
		//other 
		if(inputPermision[4]=='r') cpList[CSE][fileCount].read = 1;
		else cpList[CSE][fileCount].read = 0;
		if(inputPermision[5]=='w') cpList[CSE][fileCount].write = 1;
		else cpList[CSE][fileCount].write = 0;
		strcpy(cpList[AOS][fileCount].owner, ownerName);
		fi[fileCount].group = AOS;
	}
	else if(groupNumber == CSE)
	{
		printf("It's CSE group!\n");
		if(inputPermision[2]=='r') cpList[CSE][fileCount].read = 1;
		else cpList[CSE][fileCount].read = 0;
		if(inputPermision[3]=='w') cpList[CSE][fileCount].write = 1;
		else cpList[CSE][fileCount].write = 0;
		//other
		if(inputPermision[4]=='r') cpList[AOS][fileCount].read = 1;
		else cpList[AOS][fileCount].read = 0;
		if(inputPermision[5]=='w') cpList[AOS][fileCount].write = 1;
		else cpList[AOS][fileCount].write = 0;
		strcpy(cpList[CSE][fileCount].owner, ownerName);
		fi[fileCount].group = CSE;
	}
	strcpy(fi[fileCount].owner,ownerName);
	strcpy(fi[fileCount].fileName,inputFilename);
	//printf("%s is belong %d\n",fi[fileCount].fileName,fi[fileCount].group);
	fi[fileCount].writeCnt = false;
	fi[fileCount].readCnt = 0;
	
	fileCount++;
	
	displayCapability();
	
	FILE *fptr;
	// Use appropriate location if you are using MacOS or Linux
	fptr = fopen(inputFilename,"w");
	if(fptr == NULL)
	{
		printf("File open error!");
		exit(1);             
	}
	fprintf(fptr,"test by andy\n");//it's test
	strcpy(returnBuffer, "Create success!");
	fclose(fptr);
}

void readHandler(char *inputName, char *inputFilename, uint8_t groupNumber, char *returnBufferTemp)
{
	char fileContent[BUFFERSIZE];
	printf("\nreadHandler!\n");
	//printf("Comming name is %s, want read %s\n",inputName,inputFilename);
	uint8_t isPermission = 0;
	//printf("It's %d group!\n",groupNumber);
	uint8_t i;
	for(i=0;i<fileSize;i++)
	{
		//先看檔案存不存在
		if(strcmp(inputFilename,cpList[OWNER][i].objFileName)==0)
		{
			printf("File exist!\n");
			//先判斷是不是owner
			if(strcmp(inputName,fi[i].owner)==0)
			{
				printf("Is owner!\n");
				//是的話再看權限
				if(cpList[OWNER][i].read == 1)
				{
					isPermission = 1;break;
				}
			}
			else
			{
				if(fi[i].group==AOS)
				{
					if(groupNumber==AOS)
					{
						//是的話再看權限
						if(cpList[AOS][i].read == 1)
						{
							isPermission = 1;break;
						}
						else{isPermission = 0;break;}
					}
					else if(groupNumber==CSE)
					{
						//你是其他人
						if(cpList[CSE][i].read == 1)
						{
							isPermission = 1;break;
						}
						else{isPermission = 0;break;}
					}
					else{isPermission = 0;break;}
				}
				else if(fi[i].group==CSE)
				{
					if(groupNumber==CSE)
					{
						//是的話再看權限
						if(cpList[CSE][i].read == 1)
						{
							isPermission = 1;break;
						}
						else{isPermission = 0;break;}
					}
					else if(groupNumber==AOS)
					{
						if(cpList[CSE][i].read == 1)
						{
							isPermission = 1;
							break;
						}
						else{isPermission = 0;break;}
					}
					else{isPermission = 0;break;}			
				}
			}
		}
	}
	
	if(isPermission == 0)
	{
		printf("You do not have permission\n");
		strcpy(returnBufferTemp,"You do not have permission\n");
	}
	else if(isPermission == 1)
	{
		if(fi[i].writeCnt == true)
		{
			printf("Someone is writing.\n");
			strcpy(returnBufferTemp,"Someone is writing.\n");
		}
		else if(fi[i].writeCnt == false)
		{
			printf("Is read!\n");
			fi[i].readCnt++;
			FILE *fptr;
			fptr = fopen(inputFilename,"r");
			size_t nread;
			if(fptr)
			{
				while ((nread = fread(fileContent, 1, sizeof(fileContent), fptr)) > 0)
					fwrite(fileContent, 1, nread, stdout);
				//printf("fileContent:%s \n",fileContent);
			}
			strcpy(returnBufferTemp,fileContent);
			fclose(fptr);
			sleep(5);
			fi[i].readCnt--;
		}
	}
}
uint8_t checkFileExist(char *inputFilename)
{
	for(uint8_t i=0;i<fileSize;i++)
	{
		//先看檔案存不存在
		if(strcmp(inputFilename,cpList[OWNER][i].objFileName)==0)
		{
			return 1;
		}
	}
	return 0;
}
void writeHandler(char *inputName, char *inputFilename, char *inputPermision, uint8_t groupNumber, char *fileContent)
{
	printf("\nwriteHandler!\n");
	// printf("File owner is %s\n",inputName);
	// printf("File name is %s\n",inputFilename);
	// printf("Permision is %s\n",inputPermision);
	uint8_t isPermission = 0;
	//printf("It's %d group!\n",groupNumber);
	uint8_t fileExist = 0;
	uint8_t i;
	// printf("fileContent:\n");
	// for(int k=0;k<100;k++)
		// printf("%c ",fileContent[k]);
	
	for(i=0;i<fileSize;i++)
	{
		if(strcmp(inputFilename,cpList[OWNER][i].objFileName)==0)
		{
			if(strcmp(inputName,fi[i].owner)==0)
			{
				printf("Is owner!\n");
				//是的話再看權限
				if(cpList[OWNER][i].write == 1)
				{
					isPermission = 1;
					break;
				}
			}
			else
			{
				if(fi[i].group==AOS)
				{
					if(groupNumber==AOS)
					{
						//是的話再看權限
						if(cpList[AOS][i].write == 1)
						{
							isPermission = 1;
							break;
						}
					}
					else if(groupNumber==CSE)
					{
						//你是其他人
						if(cpList[CSE][i].write == 1)
						{
							isPermission = 1;
							break;
						}
					}
				}
				else if(fi[i].group==CSE)
				{
					if(groupNumber==CSE)
					{
						//是的話再看權限
						if(cpList[CSE][i].write == 1)
						{
							isPermission = 1;
							break;
						}
					}
					else if(groupNumber==AOS)
					{
						if(cpList[CSE][i].write == 1)
						{
							isPermission = 1;
							break;
						}
					}
				}
			}
		}
	}
	//開始寫檔
	//printf("300 isPermission=%d\n",isPermission);
	if(isPermission == 1)
	{
		if(fi[i].writeCnt == false && fi[i].readCnt == 0)
		{
			fi[i].writeCnt = true;//You can read
			printf("Is write!\n");
			FILE *fptr;
			if(inputPermision[0]=='o')
			{
				//overwrite
				fptr = fopen(inputFilename,"w");
			}
			else if(inputPermision[0]=='a')
			{
				//append
				fptr = fopen(inputFilename,"a");
			}
			if(fptr)
			{
				
				fwrite(fileContent, strlen(fileContent), 1, fptr);
				strcpy(returnBuffer,"Write succcess!\n");
			}
			fclose(fptr);
			sleep(5);
			fi[i].writeCnt = false;//You can read
			//strcpy(returnBuffer,"You do not have permission\n");
		}
		else if(fi[i].writeCnt == true || fi[i].readCnt != 0)
		{
			printf("Someone is reading or writing.\n");
			strcpy(returnBuffer,"Someone is reading or writing.\n");
		}
	}
	else
	{
		printf("You do not have permission\n");
		strcpy(returnBuffer,"You do not have permission\n");
	}
	
}
void modeHandler(char *ownerName, char *inputFilename, char *inputPermision, uint8_t groupNumberTemp)
{
	printf("modeHandler!\n");
	//printf("%c %c %c %c %c %c\n",inputPermision[0],inputPermision[1],inputPermision[2],inputPermision[3],inputPermision[4],inputPermision[5]);
	uint8_t i, modeFlag = 0;
	for(i=0;i<fileSize;i++)
	{
		if(strcmp(inputFilename,cpList[OWNER][i].objFileName) == 0)
		{//先看有沒有這個檔案
			if(strcmp(ownerName,fi[i].owner) == 0)
			{//在看是不是擁有者
				modeFlag = 1;
				// printf("You are owner!\n");
				// printf("inputFilename=%s\n",inputFilename);
				// printf("ownerName=%s\n",ownerName);
				if(inputPermision[0]=='r') cpList[OWNER][i].read = 1;
				else cpList[OWNER][i].read = 0;
				if(inputPermision[1]=='w') cpList[OWNER][i].write = 1;
				else cpList[OWNER][i].write = 0;
				if(groupNumberTemp == AOS)
				{
					//printf("You are AOS!\n");
					if(inputPermision[2]=='r') cpList[AOS][i].read = 1;
					else cpList[AOS][i].read = 0;
					if(inputPermision[3]=='w') cpList[AOS][i].write = 1;
					else cpList[AOS][i].write = 0;
					
					if(inputPermision[4]=='r') cpList[CSE][i].read = 1;
					else cpList[CSE][i].read = 0;
					if(inputPermision[5]=='w') cpList[CSE][i].write = 1;
					else cpList[CSE][i].write = 0;
				}
				else if(groupNumberTemp == CSE)
				{
					//printf("You are CSE!\n");
					if(inputPermision[2]=='r') cpList[CSE][i].read = 1;
					else cpList[CSE][i].read = 0;
					if(inputPermision[3]=='w') cpList[CSE][i].write = 1;
					else cpList[CSE][i].write = 0;
					
					if(inputPermision[4]=='r') cpList[AOS][i].read = 1;
					else cpList[AOS][i].read = 0;
					if(inputPermision[5]=='w') cpList[AOS][i].write = 1;
					else cpList[AOS][i].write = 0;
				}
				break;
			}
		}
	}
	if(modeFlag == 0)strcpy(returnBuffer,"You do not have permission\n");
	else if(modeFlag == 1)strcpy(returnBuffer,"Mode success!\n");
	displayCapability();
}
void *func(void *arg)
{
	char recvbuff[BUFFERSIZE];
	
	//char sendbuff[80];
	int n;
	int connfd = *(int *)arg;//把指標整數轉成真正的整數 這是對的
	//int *connfd = arg;//會導致pthread都共用這個 socket number
	// infinite loop for chat
	while(1) 
	{
		bzero(recvbuff, BUFFERSIZE);
		
		// read the message from client and copy it in buffer
		read(connfd, recvbuff, sizeof(recvbuff));
		if(strcmp(recvbuff,"exit")==0)
		{
			printf("receive exit!\n");
			shutdown(connfd,SHUT_WR);
			break;
		}
		struct sendInfo *si = (struct sendInfo *)recvbuff;
		// print buffer which contains the client contents
		char returnBufferTemp[BUFFERSIZE];
		
		char ownerNameTemp[20];
		char inputModeTemp[7];
		char inputFilenameTemp[20];
		char inputPermisionTemp[7];
		strcpy(ownerNameTemp,si->ownerName);
		uint8_t groupNumberTemp = (uint8_t)si->groupNumber;
		strcpy(inputModeTemp,si->inputMode);
		strcpy(inputFilenameTemp,si->inputFilename);
		strcpy(inputPermisionTemp,si->inputPermision);
		
		if(strcmp(inputModeTemp,"create")==0)
			createHandler(connfd, ownerNameTemp,groupNumberTemp,inputFilenameTemp,inputPermisionTemp);
		else if(strcmp(inputModeTemp,"read")==0)
		{
			readHandler(ownerNameTemp,inputFilenameTemp, groupNumberTemp,returnBufferTemp);
		}
		else if(strcmp(inputModeTemp,"write")==0)
		{
			char filecontentString[BUFFERSIZE]={0};
			if(checkFileExist(inputFilenameTemp)==1)
			{
				write(connfd, "File exist", 11);
				bzero(returnBuffer,BUFFERSIZE);
				bzero(recvbuff, BUFFERSIZE);
				//printf("sizeof(recvbuff) = %ld\n",sizeof(recvbuff));
				read(connfd, recvbuff, sizeof(recvbuff));
				//printf("recvbuff = %s\n",recvbuff);
				strcpy(filecontentString,recvbuff);
				int k=0;
				for(k=0;k<BUFFERSIZE;k++)
				{
					//printf("%c ",filecontentString[k]);
					if(filecontentString[k] == ' ')break;
				}
				writeHandler(ownerNameTemp,inputFilenameTemp,inputPermisionTemp, groupNumberTemp, filecontentString);
			}
			else
			{
				printf("File doesn't exist\n");
				strcpy(returnBuffer,"File doesn't exist\n");
			}
		}
		else if(strcmp(inputModeTemp,"mode")==0)
			modeHandler(ownerNameTemp,inputFilenameTemp,inputPermisionTemp,groupNumberTemp);
		//printf("End handler\n");
		if(strcmp(inputModeTemp,"read")==0)
		{
			printf("(%d)read: returnBufferTemp:%s\n",connfd,returnBufferTemp);
			write(connfd, returnBufferTemp, sizeof(returnBufferTemp));
			bzero(returnBuffer,BUFFERSIZE);
		}
		else
		{
			bzero(recvbuff, BUFFERSIZE);
			// send that buffer to client
			//printf("returnBuffer=%s\n",returnBuffer);
			write(connfd, returnBuffer, sizeof(returnBuffer));
			bzero(returnBuffer,BUFFERSIZE);
		}
		// if msg contains "Exit" then server exit and chat ended.
	}
}

int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	pthread_t id;
	//printf("1 pid=%lu\n",id);
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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));//防止ctrl-c port還繼續占用的情形
	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 20)) != 0) 
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	while(1)
	{
		len = sizeof(cli);
		// Accept the data packet from client and verification
		connfd = accept(sockfd, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("server accept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");
		
		if(pthread_create(&id,NULL,func, (void*)&connfd)!=0)//建一個thread holding client，主程式回到while上面accept下一個client
			printf("Failed to create thread\n");
	}
	// Function for chatting between client and server
	//func(connfd);
	// After chatting close the socket
	//close(sockfd);
	
}
