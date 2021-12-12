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
		else{
			correct_input = false;
		}
	}
	int array_size = MUL_SIZE * size;
	pid_t pid = fork();

	if (pid < 0) // error in fork()
	{
		report_and_exit("fork");
	}	

	if (pid > 0) //I'm in the parent process
	{
		// initialize the array that will contain a MB of random data
		int data_array[MUL_SIZE];
		for (int i=0; i < MUL_SIZE; i++)
		{
			data_array[i] = rand();
		}
	
		printf("Culo");
		fflush(stdout);
		close(fd_unnamed[ReadEnd]); 
		gettimeofday(&t_start, NULL);
		for (int i=0; i < size; i++)
		{
			for (int j=0; j < MUL_SIZE; j++)
			{
				write(fd_unnamed[WriteEnd], &data_array[i], sizeof(data_array[i]));
			}
		}
		printf("Culo1");
		fflush(stdout);

		terminated = waitpid(pid, &status, 0);
		if (terminated == -1) 
		{
		report_and_exit("waitpid()");
		}
	}
	
	if(pid == 0)//I'm in the child process
	{
		printf("Culo3");
		fflush(stdout);
		close(fd_unnamed[WriteEnd]);
		int rd_data_array[array_size];
		
		for (int i = 0; i < array_size; i++){
			read(fd_unnamed[ReadEnd], &rd_data_array[i], sizeof(rd_data_array[i]));
			
		}
		printf("Culo2");
		fflush(stdout);
		gettimeofday(&t_end, NULL);
		close(fd_unnamed[ReadEnd]);
		double time_taken = (t_end.tv_sec * 1000000 + t_end.tv_usec) - (t_start.tv_sec * 1000000 + t_start.tv_usec);
		printf("\nTime taken for transferring data = %f sec\n", time_taken);
		fflush(stdout);
	
	}
	return 0;
}
