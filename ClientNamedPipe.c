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
    // controllo sul fatto che arrivino gli argomenti

    // getting arguments of PID and size from Producer
    int pidProducer = atoi(argv[1]);
    int size = atoi(argv[2]);

    int rd_data_array[MUL_SIZE];

     // defining a pipe for interprocess communication
    char* f_client = "/tmp/f_client";

    // open the pipe
    int fd_client;	
    fd_client = open(f_client, O_WRONLY);
	if (fd_client < 0) 
	{
        perror("fd_client");
        return -1;
    }
    printf("Culo4");
    fflush(stdout);

    for (int i = 0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(fd_client, &rd_data_array[j], sizeof(rd_data_array[j]));
		}
	}
    close(fd_client);
    printf("Culo5");
    fflush(stdout);
    // send signal to Producer
    kill(pidProducer, SIGINT);
    
}