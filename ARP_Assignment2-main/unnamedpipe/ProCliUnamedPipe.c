#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h> 
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MUL_SIZE 250000
#define ReadEnd  0
#define WriteEnd 1
#define LOWER 0
#define UPPER 9
/*
Function that handles possible errors 
*/
void report_and_exit(const char* msg) 
{
  perror(msg);
  exit(-1);/** failure **/
}


int main (int argc, char *argv[]) 
{
	// initialize variables for time informations for log file
	time_t rawtime;
  	struct tm * timeinfo;
	//Initilize and open pointers for writing in Producer and Consumer logfile
	FILE *logfileProd;
	FILE *logfileCons;
    logfileProd = fopen("./../logfile/logfileProdUnnamedPipe.txt", "w");
	if (logfileProd == NULL)
    {
        printf("Error opening the Producer logfile!\n");
        exit(1);
    }

	logfileCons = fopen("./../logfile/logfileConsUnnamedPipe.txt", "w");
	if (logfileCons == NULL)
    {
        printf("Error opening the Producer logfile!\n");
        exit(1);
    }
	fprintf(logfileProd, "PRODUCER PROCESS STARTS:\n");
    fflush(logfileProd);
	// initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
	// initialize unnamed pipe
	int fd_unnamed[2];
	if (pipe(fd_unnamed) < 0) 
	{
		report_and_exit("fd_unnamed");
		fprintf(logfileProd, "fd_unnamed\n");
    	fflush(logfileProd);
	}
	int size, status;
	pid_t terminated;
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
		else{
			correct_input = false;
		}
	}
	
	
	pid_t pid = fork();

	if (pid > 0) 
	{
		/*I'm in the parent process*/
		fprintf(logfileProd, "\nProducer forked consumer\n");
		fflush(logfileProd);
		if (pid < 0) // error in fork()
		{
			report_and_exit("fork");
			fprintf(logfileProd, "fork");
			fflush(logfileProd);
		}	
		
		// initialize the array that will contain a MB of random data
		int data_array[MUL_SIZE];
		for (int i=0; i < MUL_SIZE; i++)
		{
			data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER; // rand data from 1 to 9
		}
		fprintf(logfileProd,"\ndata_array for writing filled\n");
    	fflush(logfileProd);
		//get time before starting to write on the pipe
		gettimeofday(&t_start, NULL);
		fprintf(logfileProd,"\nGet time before starting to write on the pipe\n");
		fflush(logfileProd);

		// close pipe for reading 
		close(fd_unnamed[ReadEnd]); 
		fprintf(logfileProd,"\nWriting stage:\n");
    	fflush(logfileProd);
		for (int i=0; i < size; i++)
		{
			fprintf(logfileProd,"\nMB number %d:\n", i+1);
    		fflush(logfileProd);
			for (int j=0; j < MUL_SIZE; j++)
			{
				write(fd_unnamed[WriteEnd], &data_array[j], sizeof(data_array[j]));
				if( j % 5000 == 0)
				{
					fprintf(logfileProd,"[%d] %d", j, data_array[j]);
					fflush(logfileProd);
				}
			}
		}
		
		// child process waits until parent finishes
		terminated = waitpid(pid, &status, 0);
		if (terminated == -1) 
		{
			report_and_exit("waitpid()");
			fprintf(logfileProd, "\n\nwaitpid()\n");
			fflush(logfileProd);
		}
		// getting time as soon as i end reading from pipe
		gettimeofday(&t_end, NULL);
		fprintf(logfileProd,"\n\nGet time at the end of transmission\n");
		fflush(logfileProd);
		close(fd_unnamed[ReadEnd]);
		//time in sec
		double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
		printf("\nTime taken for transferring data = %f sec\n", time_taken);
		fflush(stdout);
		fprintf(logfileProd,"\nTime taken for transferring data = %f sec\n", time_taken);
		fprintf(logfileProd,"\nProducer process exiting...\n");
		fflush(logfileProd);

	}
	
	if(pid == 0)//I'm in the child process
	{
		fprintf(logfileCons, "CONSUMER PROCESS STARTS: \n");
		fflush(logfileCons);
		
		// close pipe for writing
		close(fd_unnamed[WriteEnd]);
		int rd_data_array[MUL_SIZE];
		
		fprintf(logfileCons, "\nReading stage:\n");
		fflush(logfileCons);
		for (int i = 0; i < size; i++){
			fprintf(logfileCons, "\nMB number %d:\n", i+1);
			fflush(logfileCons);
			for (int j = 0; j < MUL_SIZE; j++)
			{
				read(fd_unnamed[ReadEnd], &rd_data_array[j], sizeof(rd_data_array[j]));
				if( j % 5000 == 0)
				{
					fprintf(logfileCons,"[%d] %d", j, rd_data_array[j]);
					fflush(logfileCons);
				}
			}
		}
		fprintf(logfileCons,"\n\nConsumer process exiting...\n");
		fflush(logfileCons);
	}
	
	return 0;
}
