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
//Open a file pointer for writing
FILE *logfile;

// initialize variables for time informations for log file
time_t rawtime;
struct tm * timeinfo;

/*
 * Function that handles possible errors 
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

    logfile = fopen("./../logfile/NamedPipe/logfileClientNamedPipe.txt", "w");
    if (logfile == NULL)
    {
        printf("Error opening the logfile!\n");
        exit(1);
    }
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "%sCONSUMER PROGRAM STARTS:\n", asctime(timeinfo));
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
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sProducer Pid and chosen size are received:\n", asctime(timeinfo));
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
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sFile descriptor for reading opened\n", asctime(timeinfo));
    fflush(logfile);
    // read all the data produced
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sReading:", asctime(timeinfo));
    fflush(logfile);
    for (int i = 0; i < size; i++)
	{
        time ( &rawtime );
  	    timeinfo = localtime ( &rawtime );
        fprintf(logfile,"\n\n%sMB number %d: \n",asctime(timeinfo), i+1);
        fflush(logfile);
	    //read 1MB data by data
		for (int j=0; j < MUL_SIZE; j++)
		{
			read(fd_client, &rd_data_array[j], sizeof(rd_data_array[j]));
			//print one value every 5000 on the logfile
            if (j % 5000 == 0)
            {
                fprintf(logfile,"[%d]-%d; ", j, rd_data_array[j]);
                fflush(logfile);
            }
		}
	}
    close(fd_client);
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n\n%sEnd of reading\n", asctime(timeinfo));
    fflush(logfile);
    // send signal to Producer
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sSending signal of finishing consuming data\n", asctime(timeinfo));
    fprintf(logfile, "\n%sConsumer program exiting...\n", asctime(timeinfo));
    fflush(logfile);
    fclose(logfile);
    kill(pidProducer, SIGINT);
}
