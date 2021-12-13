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


/*
Function that handles possible errors 
*/
void report_and_exit(const char* msg) 
{
  perror(msg);
  exit(-1);    /** failure **/
}


int main (int argc, char *argv[]) 
{
	struct timeval t_start, t_end;
	int fd_unnamed[2];
	if (pipe(fd_unnamed) < 0) 
	{
		report_and_exit("fd_unnamed");
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
		else
		{
			correct_input = false;
		}
	}
	// initialize the array that will contain a MB of random data
	int data_array[MUL_SIZE];
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = rand();
	}
	int array_size = MUL_SIZE * size;
	pid_t pid = fork();

	if (pid < 0) // error in fork()
	{
		report_and_exit("fork");
	}	
	gettimeofday(&t_start, NULL);
	if (pid > 0) //I'm in the parent process
	{
		close(fd_unnamed[ReadEnd]); 
		for (int i=0; i < size; i++)
		{
			for (int j=0; j < MUL_SIZE; j++)
			{
				write(fd_unnamed[WriteEnd], &data_array[j], sizeof(data_array[j]));
			}
		}

		terminated = waitpid(pid, &status, 0);
		if (terminated == -1) 
		{
			report_and_exit("waitpid()");
		}
	}
	
	if(pid == 0)//I'm in the child process
	{
		close(fd_unnamed[WriteEnd]);
		int rd_data_array[MUL_SIZE];
		
		for (int i = 0; i < size; i++)
		{
			for (int j=0; j < MUL_SIZE; j++)
			{
				read(fd_unnamed[WriteEnd], &rd_data_array[j], sizeof(rd_data_array[j]));
			}
		}
		gettimeofday(&t_end, NULL);
		close(fd_unnamed[ReadEnd]);
		double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
		printf("\nTime taken for transferring data = %f sec\n\n", time_taken);
		fflush(stdout);
	
	}
	return 0;
}
