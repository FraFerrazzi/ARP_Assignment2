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


#define MUL_SIZE 250000

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    char *ip = "127.0.0.1";
    int sockfd, newsockfd, portno, clilen, pidClient, size;
    char bufPortNo[20];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    // creating an unbound socket in a communication domain
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }

    // asking user the value for port number
    printf("Write the port number: ");
    fflush(stdout);
    scanf("%d", &portno);
    sprintf(bufPortNo, "%d", portno);
    // define arg list to send the PID of server and i the port number to Client
    char *arglist[] = {"./ClientSocket", bufPortNo, (char*)NULL};
    
    // bzero function is used to set all the socket structures with null values
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // store server address and port number in a string variable
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip); //INADDR_ANY if doesn't work
    serv_addr.sin_port = htons(portno);
    // assigning a local address to a socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error("ERROR on binding");
    }
    // prepare server for incoming client request
    if (listen(sockfd,2) < 0)
    {
        error("ERROR on listen");
    }
    // defining the size of the protocol address of the client
    clilen = sizeof(cli_addr);
    // accepts a connection on a socket
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    { 
        error("ERROR on accept");
    }
    // getting size from Server's arg list
    size = atoi(argv[1]);
    // getting PID client
    pidClient = atoi(argv[2]);
    printf("PID: %d", pidClient);
    printf("size: %d", size);
    fflush(stdout);

    // initialize array to store random data
    int rd_data_array[MUL_SIZE];

    // read all the data produced
    for (int i = 0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(newsockfd, &rd_data_array[j], sizeof(rd_data_array[j]));
		}
	}

    // send signal to Producer
    kill(pidClient, SIGINT);
}
