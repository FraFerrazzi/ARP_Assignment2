#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h> 
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MUL_SIZE 250000
#define ReadEnd  0
#define WriteEnd 1
#define LOWER 0
#define UPPER 9

//Initilize and open pointers for writing in Producer and Consumer logfile
FILE *logfileProd;
FILE *logfileCons;
// initialize variables for time informations for log file
time_t rawtime;
struct tm * timeinfo;
/*
Function that handles possible errors 
*/
void error_handler_child(char *msg)
{
    perror(msg);
    fprintf(logfileCons, "%s", msg);
    fflush(logfileCons);
    exit(0);
}
void error_handler_father(char *msg)
{
    perror(msg);
    fprintf(logfileProd, "%s", msg);
    fflush(logfileProd);
    exit(0);
}

int main (int argc, char *argv[]) 
{
	
	
    logfileProd = fopen("./../logfile/UnnamedPipe/logfileProdUnnamedPipe.txt", "w");
	if (logfileProd == NULL)
    {
        printf("Error opening the Producer logfile!\n");
        exit(1);
    }

	logfileCons = fopen("./../logfile/UnnamedPipe/logfileConsUnnamedPipe.txt", "w");
	if (logfileCons == NULL)
    {
        printf("Error opening the Consumer logfile!\n");
        exit(1);
    }
	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
	fprintf(logfileProd, "%sPRODUCER PROCESS STARTS:\n", asctime(timeinfo));
    fflush(logfileProd);
	// initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
	// initialize unnamed pipe
	int fd_unnamed[2];
	if (pipe(fd_unnamed) < 0) 
	{
		error_handler_father("fd_unnamed");
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
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileProd, "\n%sProducer forked consumer\n", asctime(timeinfo));
		fflush(logfileProd);
		if (pid < 0) // error in fork()
		{
			error_handler_father("fork");
		}	
		
		// initialize the array that will contain a MB of random data
		int data_array[MUL_SIZE];
		for (int i=0; i < MUL_SIZE; i++)
		{
			data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER; // rand data from 1 to 9
		}
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileProd,"\n%sdata_array for writing filled\n", asctime(timeinfo));
    	fflush(logfileProd);
		//get time before starting to write on the pipe
		gettimeofday(&t_start, NULL);
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileProd,"\n%sGet time before starting to write on the pipe\n", asctime(timeinfo));
		fflush(logfileProd);

		// close pipe for reading 
		close(fd_unnamed[ReadEnd]); 
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileProd,"\n%sWriting stage:", asctime(timeinfo));
    	fflush(logfileProd);
		for (int i=0; i < size; i++)
		{
			time ( &rawtime );
  			timeinfo = localtime ( &rawtime );
			fprintf(logfileProd,"\n\n%sMB number %d:\n", asctime(timeinfo),i+1);
    		fflush(logfileProd);
			for (int j=0; j < MUL_SIZE; j++)
			{
				write(fd_unnamed[WriteEnd], &data_array[j], sizeof(data_array[j]));
				if( j % 5000 == 0)
				{
					fprintf(logfileProd,"[%d]-%d; ", j, data_array[j]);
					fflush(logfileProd);
				}
			}
		}
		
		// child process waits until parent finishes
		terminated = waitpid(pid, &status, 0);
		if (terminated == -1) 
		{
			error_handler_father("waitpid()");
		}
		// getting time as soon as i end reading from pipe
		gettimeofday(&t_end, NULL);
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileProd,"\n\n%sGet time at the end of transmission\n", asctime(timeinfo));
		fflush(logfileProd);
		close(fd_unnamed[ReadEnd]);
		//time in sec
		double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
		printf("\nTime taken for transferring data = %f sec\n", time_taken);
		fflush(stdout);
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileProd,"\n%sTime taken for transferring data = %f sec\n", asctime(timeinfo),time_taken);
		fprintf(logfileProd,"\n%sProducer process exiting...\n", asctime(timeinfo));
		fflush(logfileProd);
		fclose(logfileProd);
	}
	
	if(pid == 0)//I'm in the child process
	{
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileCons, "%sCONSUMER PROCESS STARTS: \n", asctime(timeinfo));
		fflush(logfileCons);
		// close pipe for writing
		close(fd_unnamed[WriteEnd]);
		int rd_data_array[MUL_SIZE];
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileCons, "\n%sReading stage:", asctime(timeinfo));
		fflush(logfileCons);
		for (int i = 0; i < size; i++){
			time ( &rawtime );
  			timeinfo = localtime ( &rawtime );
			fprintf(logfileCons, "\n\n%sMB number %d:\n", asctime(timeinfo),i+1);
			fflush(logfileCons);
			for (int j = 0; j < MUL_SIZE; j++)
			{
				read(fd_unnamed[ReadEnd], &rd_data_array[j], sizeof(rd_data_array[j]));
				if( j % 5000 == 0)
				{
					fprintf(logfileCons,"[%d]-%d; ", j, rd_data_array[j]);
					fflush(logfileCons);
				}
			}
		}
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
		fprintf(logfileCons,"\n\n%sConsumer process exiting...\n", asctime(timeinfo));
		fflush(logfileCons);
		fclose(logfileCons);
	}
	return 0;
}
