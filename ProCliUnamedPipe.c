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
 * Function that handles possible errors 
 */
void error_handler(const char* msg) 
{
  perror(msg);
  // with log file is sufficient to add here the fprintf
  exit(-1);    /** failure **/
}


int main (int argc, char *argv[]) 
{
	// initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
	// initialize unnamed pipe
	int fd_unnamed[2];
	if (pipe(fd_unnamed) < 0) 
	{
		error_handler("fd_unnamed");
	}
	int size, status;
	pid_t terminated;
	bool incorrect_input = true;
	while(incorrect_input)
	{
		printf("Decide the amount of random Data that will be generated in Mb from 1 to 100: ");
		fflush(stdout);
		scanf("%d", &size);
		if (size <= 0 || size > 100)
		{
			printf("\nERROR!! Not the right input! Please choose an integer number between 1 and 100\n\n");
			incorrect_input = true;
		}
		else
		{
			incorrect_input = false;
		}
	}
	// initialize the array that will contain a MB of random data
	int data_array[MUL_SIZE];
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER; // rand data from 1 to 9
	}
	pid_t pid = fork();

	if (pid < 0) // error in fork()
	{
		error_handler("fork");
	}
		
	// get time before starting to write on the pipe
	gettimeofday(&t_start, NULL);
	if (pid > 0) //I'm in the parent process
	{
		// close pipe for reading 
		close(fd_unnamed[ReadEnd]); 
		for (int i=0; i < size; i++)
		{
			for (int j=0; j < MUL_SIZE; j++)
			{
				write(fd_unnamed[WriteEnd], &data_array[j], sizeof(data_array[j]));
			}
		}
		// child process waits until parent finishes
		terminated = waitpid(pid, &status, 0);
		if (terminated == -1) 
		{
			error_handler("waitpid()");
		}
	}
	
	if(pid == 0)//I'm in the child process
	{
		// close pipe for writing
		close(fd_unnamed[WriteEnd]);
		int rd_data_array[MUL_SIZE];
		for (int i = 0; i < size; i++)
		{
			for (int j=0; j < MUL_SIZE; j++)
			{
				read(fd_unnamed[WriteEnd], &rd_data_array[j], sizeof(rd_data_array[j]));
			}
		}
		// getting time as soon as i end reading from pipe
		gettimeofday(&t_end, NULL);
		close(fd_unnamed[ReadEnd]);
		double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
		printf("\nTime taken for transferring data with unamed pipe = %f sec\n\n", time_taken);
		fflush(stdout);
	
	}
	return 0;
}