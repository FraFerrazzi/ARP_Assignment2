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


/*
 * Function that handles possible errors 
 */
void error_handler(const char* msg) 
{
  perror(msg);
  // with log file is sufficient to add here the fprintf for error handling
  exit(-1);    /** failure **/
}

int main(int argc, char *argv[])
{
    //Open a file pointer for writing
    FILE *logfile;
    logfile = fopen("./../logfile/logfileClientNamedPipe.txt", "w");
    if (logfile == NULL)
    {
        printf("Error opening the logfile!\n");
        exit(1);
    }
    fprintf(logfile, "Client program starts\n");
    fflush(logfile);
    // if there are not enough arguments, error
    if (argc < 3) 
    {
       fprintf(stderr,"\nNot all arguments were received by client\n");
       exit(0);
    }

    // getting arguments of PID and size from Producer
    int pidProducer = atoi(argv[1]);
    int size = atoi(argv[2]);
    fprintf(logfile,"\nProducer Pid and chosen size received:\n");
    fprintf(logfile,"PID: [%d]\n", pidProducer);
    fprintf(logfile,"Size: [%d]\n", size);
    fflush(logfile);

    // initialize array to store random data
    int rd_data_array[MUL_SIZE];

     // defining a pipe for interprocess communication
    char* f_client = "/tmp/f_client";

    // open the pipe
    int fd_client;	
    fd_client = open(f_client, O_RDONLY);
	if (fd_client < 0) 
	{
        error_handler("fd_client\n");
    }
    fprintf(logfile,"\nFile descriptor for reading opened\n");
    fflush(logfile);
    // read all the data produced
    for (int i = 0; i < size; i++)
	{
        fprintf(logfile,"\n\nMB number %d: \n", i+1);
        fflush(logfile);
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(fd_client, &rd_data_array[j], sizeof(rd_data_array[j]));
            if (j % 5000 == 0)
            {
                fprintf(logfile,"[%d] %d, ", j, rd_data_array[j]);
                fflush(logfile);
            }
		}
	}
    close(fd_client);
    fprintf(logfile,"\n\nEnd of reading\n");
    fflush(logfile);
    // send signal to Producer
    fprintf(logfile, "\nSending signal of finishing consuming data\n");
    fflush(logfile);
    kill(pidProducer, SIGINT);
}
