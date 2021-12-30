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
#include <time.h>
#include <arpa/inet.h>

#define MUL_SIZE 250000
//#define PORT 8070
#define PORT 8080
//#define PORT 8090

//Initilize and open pointers for writing in logfile
FILE *logfile;
// initialize variables for time informations for log file
time_t rawtime;
struct tm * timeinfo;

/*
 *Function that handles possible errors 
 */
void error_handler(char *msg)
{
    perror(msg);
    fprintf(logfile, "%s", msg);
    fflush(logfile);
    exit(0);
}

int main(int argc, char *argv[])
{

    logfile = fopen("./../logfile/Socket/logfileClientSocket.txt", "w");
    if (logfile == NULL)
    {
        printf("Error opening the logfile!\n");
        exit(1);
    }
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "%sCLIENT PROGRAM STARTS\n", asctime(timeinfo));
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
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sServer's Pid and chosen size are received:\n", asctime(timeinfo));
    fprintf(logfile,"PID: [%d]\n", pidServer);
    fprintf(logfile,"Size: [%d]\n", size);
    fflush(logfile);
    // initialize array to store random data
    int rd_data_array[MUL_SIZE];
    bzero(rd_data_array,MUL_SIZE);
    // read all the data produced
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sReading:", asctime(timeinfo));
    fflush(logfile);
    for (int i = 0; i < size; i++)
	{
        time ( &rawtime );
  	    timeinfo = localtime ( &rawtime );
        fprintf(logfile, "\n\n%sMB number %d:\n", asctime(timeinfo),i+1);
        fflush(logfile);
	    //read 1MB data by data
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(sockfd, &rd_data_array[j], sizeof(rd_data_array[j]));
			//print one value every 5000 on the logfile
            if (j % 5000 == 0)
            {
                fprintf(logfile, "[%d]-%d; ", j, rd_data_array[j]);
                fflush(logfile);
            }
		}
	}
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n\n%sEnd of reading\n", asctime(timeinfo));
    fflush(logfile);
    // send signal to Producer
    kill(pidServer, SIGINT);
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sSending signal of finishing consuming data\n", asctime(timeinfo));
    fprintf(logfile, "\n%sConsumer program exiting...\n", asctime(timeinfo));
    fflush(logfile);
    fclose(logfile);
    return 0;
}

