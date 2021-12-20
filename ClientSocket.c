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
#define PORT 8080
char arrived;
#define LOWER 0
#define UPPER 9

/*
 *Function that handles possible errors 
 */
void error_handler(char *msg)
{
    perror(msg);
    exit(0);
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
    // defining structures for socket
    char *ip = "127.0.0.1";
    int sockfd, n, pidClient, size;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int data_array[MUL_SIZE];
    char bufPidClient[20];
    char bufSize[20];
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
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(PORT);
    // connecting to server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        error_handler("connect");
    }

    bzero(bufPidClient, sizeof(bufPidClient));
    // getting client PID
    pidClient = getpid();
    sprintf(bufPidClient, "%d", pidClient);
    write(sockfd, bufPidClient, sizeof(bufPidClient));
    
    bzero(bufSize, sizeof(bufSize));
    // getting input from user
    // this is done to define how many MB of data are going to be transferred
    bool correct_input = true;
    while(correct_input)
	{
		printf("Decide the amount of random Data that will be generated in Mb from 1 to 100: ");
		fflush(stdout);
		scanf("%d", &size);
		if (size <= 0 || size > 100)
		{
			printf("\nERROR!! Not the right input! Please choose an integer number between 1 and 100\n\n");
			correct_input = true;
		}
		else
		{
			correct_input = false;
		}
	}
    sprintf(bufSize, "%d", size);
    write(sockfd, bufSize, sizeof(bufSize));
    
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
			write(sockfd, &data_array[j], sizeof(data_array[j]));
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
	printf("\nTime taken for transferring data with socket = %f sec\n\n", time_taken);
	fflush(stdout);
    return 0;
}

