#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h> 
#include <signal.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <time.h>
#include <stdbool.h>

#define MUL_SIZE 250000

int main(int argc, char *argv[])
{
    // if there are not enough arguments, error
    if (argc < 3) 
    {
       fprintf(stderr,"Not all arguments were received by client");
       exit(0);
    }

    // getting arguments of PID and size from Producer
    int pidProducer = atoi(argv[1]);
    int size = atoi(argv[2]);

    // initialize array to store random data
    int rd_data_array[MUL_SIZE];

     // defining a pipe for interprocess communication
    char* f_client = "/tmp/f_client";

    // open the pipe
    int fd_client;	
    fd_client = open(f_client, O_RDONLY);
	if (fd_client < 0) 
	{
        perror("fd_client");
        return -1;
    }

    // read all the data produced
    for (int i = 0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(fd_client, &rd_data_array[j], sizeof(rd_data_array[j]));
		}
	}
    close(fd_client);
    // send signal to Producer
    kill(pidProducer, SIGINT);
    
}
