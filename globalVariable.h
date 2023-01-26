//ref: https://www.geeksforgeeks.org/socket-programming-cc/
#include <stdbool.h>
#define PORT 8081
#define SA struct sockaddr
#define BUFFERSIZE 10000
struct capabilityLists
{
	char objFileName[20];
	char owner[20];
	uint8_t read;
	uint8_t write;
};
struct fileInfo
{
	char fileName[20];
	char owner[20];
	uint8_t group;
	uint8_t readCnt;
	bool writeCnt;
};
struct sendInfo
{
	char ownerName[20];
	uint8_t groupNumber;
	char inputMode[7];
	char inputFilename[20];
	char inputPermision[7];
};
