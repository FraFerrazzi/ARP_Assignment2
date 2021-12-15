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
char arrived;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

// signal handler for signals from server
// this is done to receive a signal from server as soon as finishes 
// reading the data
void sig_handler(int signo)
{
    if(signo == SIGINT)
        arrived = 'a';
}

int main(int argc, char *argv[])
{
    char *ip = "127.0.0.1";
    int sockfd, portno, n, pidClient;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int data_array[MUL_SIZE];
    char bufPidClient[20];
    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;

    // getting client PID
    pidClient = getpid();
    sprintf(bufPidClient, "%d", pidClient);
    
    // if there are not enough arguments, error
    if (argc < 2) 
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    // getting port number as argument given by server
    portno = atoi(argv[1]);
    printf("%d portno", portno);
    fflush(stdout);

    // creating an unbound socket in a communication domain
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    /*
    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    */
    // bzero function is used to set all the socket structures with null values
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    // connecting to server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR connecting");
    }
    
    // create the array that will contain one MB of random data
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = rand();
	}

    bzero(data_array,MUL_SIZE);
    
    // getting input from user
    // this is done to define how many MB of data are going to be transferred
    char buf_size[20];
    int size;
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
    sprintf(buf_size, "%d", size);
    // define arg list to send the size given by user to the server
    char *arglist[] = {"./ServerSocket", buf_size, bufPidClient, (char*)NULL};
    
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
	printf("\nTime taken for transferring data with named pipe = %f sec\n\n", time_taken);
	fflush(stdout);
    return 0;
}

