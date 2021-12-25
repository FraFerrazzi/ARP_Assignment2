/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8070
#define MUL_SIZE 250000
char arrived;
#define LOWER 0
#define UPPER 9

/*
 *Function that handles possible errors 
 */
void error_handler(char *msg)
{
    perror(msg);
    exit(1);
}

/* 
 * Function used for the handling signals from client
 * producer receives a signal from client as soon as the child
 * finishes reading the data
 * when signal arrives, get the time
 */
void sig_handler(int signo)
{
    if(signo == SIGINT)
        arrived = 'a';
}

int main(int argc, char *argv[])
{
    // when signal arrives, sig_handler function is called
    signal(SIGINT, sig_handler);
    // file for getting the size from user
    FILE *stream;

    // defining structures for socket
    int sockfd, newsockfd, clilen, pidServer, size;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char bufPidServer[20];
    char bufSize[20];
    int data_array[MUL_SIZE];

    // open the file
    stream = fopen("./socketdata.txt","r");
    if (stream == NULL)
    {
        error_handler("fopen");
    }
    // read size from file
    fscanf(stream, "%d", &size);
    fclose(stream);

    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
    
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
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(PORT);
    // assigning a local address to a socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error_handler("bind");
    }
    // prepare server for incoming client request
    if (listen(sockfd,2) < 0)
    {
        error_handler("listen");
    }
    // defining the size of the protocol address of the client
    clilen = sizeof(cli_addr);
    // accepts a connection on a socket
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    { 
        error_handler("accept");
    }

    // sending size to ClientS
    bzero(bufSize, sizeof(bufSize));
    sprintf(bufSize, "%d", size);
    write(newsockfd, bufSize, sizeof(bufSize));

    bzero(bufPidServer, sizeof(bufPidServer));
    // getting client PID
    pidServer = getpid();
    sprintf(bufPidServer, "%d", pidServer);
    write(newsockfd, bufPidServer, sizeof(bufPidServer));

    bzero(data_array,MUL_SIZE);
    // create the array that will contain one MB of random data
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
	}
    // getting time before writing
    gettimeofday(&t_start, NULL);
    // write data from client to server
    for (int i=0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
			write(newsockfd, &data_array[j], sizeof(data_array[j]));
		}
	}
    
    // until the signal is not received means that client is still reading
    int counter = 0;
    bool not_received = true;
    while(not_received)
    {
        if (arrived == 'a') // when signal arrives enter the loop
        {
            gettimeofday(&t_end, NULL); // get time when client finishes reading
            not_received = false;
        }
        else
        {
            counter++;
        }
    }
    // calculate the time it took to write and read data
    double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
    // open the file
    stream = fopen("./socketdata.txt","w");
    if (stream == NULL)
    {
        error_handler("fopen");
    }
	fprintf(stream, "\n\nTime taken for transferring data with socket = %f sec\n\n", time_taken);
	fflush(stream);
    fclose(stream);

    // After reading the whole data close the socket
    close(sockfd);
}