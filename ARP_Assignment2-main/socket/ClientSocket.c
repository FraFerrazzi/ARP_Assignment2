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
    FILE *logfile;
    logfile = fopen("./../logfile/logfileClientSocket.txt", "w");
    if (logfile == NULL)
    {
        printf("Error opening the logfile!\n");
        exit(1);
    }
    fprintf(logfile, "Client program starts\n");
    fflush(logfile);
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
    fprintf(logfile, "\nReceiving PID and size from the server:\n");
    fprintf(logfile, "PID: %d\n",pidServer);
    fprintf(logfile, "Size: %d\n",size);
    fflush(logfile);
    // initialize array to store random data
    int rd_data_array[MUL_SIZE];
    bzero(rd_data_array,MUL_SIZE);
    // read all the data produced
    fprintf(logfile, "\nReading:\n");
    fflush(logfile);
    for (int i = 0; i < size; i++)
	{
        fprintf(logfile, "\nMB number %d:\n", i+1);
        fflush(logfile);
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(sockfd, &rd_data_array[j], sizeof(rd_data_array[j]));
            if (j % 5000 == 0)
            {
                fprintf(logfile, "[%d] %d", j, rd_data_array[j]);
                fflush(logfile);
            }
		}
	}
    fprintf(logfile,"\n\nEnd of reading\n");
    fflush(logfile);
    // send signal to Producer
    fprintf(logfile, "\nSending signal of finishing consuming data\n");
    fflush(logfile);
    kill(pidServer, SIGINT);

    return 0;
}

