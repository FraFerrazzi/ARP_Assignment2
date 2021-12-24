#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define MUL_SIZE 250000
#define PORT 8070

/*
 *Function that handles possible errors 
 */
void error_handler(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    // defining structures for socket
    char *ip = "127.0.0.1";
    int sockfd, n, pidServer, size;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int data_array[MUL_SIZE];
    char bufPidServer[20];
    char bufSize[20];

    // creating an unbound socket in a communication domain
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error_handler("socket");
    }
    
    // bzero function is used to set all the socket structures with null values
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // store server address and port number in a string variable
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(PORT);
    // connecting to server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        error_handler("connect");
    }

    bzero(bufSize, sizeof(bufSize));
    // getting client PID 
    read(sockfd, bufSize, sizeof(bufSize));
    size = atoi(bufSize);
    
    bzero(bufPidServer, sizeof(bufPidServer));
    // getting size from client
    read(sockfd, bufPidServer, sizeof(bufPidServer));
    pidServer = atoi(bufPidServer);
    
    // initialize array to store random data
    int rd_data_array[MUL_SIZE];
    bzero(rd_data_array,MUL_SIZE);
    // read all the data produced
    for (int i = 0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(sockfd, &rd_data_array[j], sizeof(rd_data_array[j]));
		}
	}
    // send signal to Producer
    kill(pidServer, SIGINT);

    return 0;
}

