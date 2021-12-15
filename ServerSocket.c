/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define MUL_SIZE 250000

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    char *ip = "127.0.0.1";
    int sockfd, newsockfd, portno, clilen;
    char data_array[MUL_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    // if there are not enough arguments, error
    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // creating an unbound socket in a communication domain
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    // bzero function is used to set all the socket structures with null values
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // getting port number as argument given by user
    portno = atoi(argv[1]);
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

    // create the array that will contain one MB of random data
	int data_array[MUL_SIZE];
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

    
    n = read(newsockfd,buffer,255);
    if (n < 0)
    { 
        error("ERROR reading from socket");
    }
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0)
    { 
        error("ERROR writing to socket");
    }
    return 0; 
}
