#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h> 
#include <signal.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/time.h>
#include <stdbool.h>

#define MUL_SIZE 250000
char arrived;

// function to spawn processes
int spawn(const char * program, char ** arg_list) 
{
	pid_t child_pid = fork(); // create parent and child processes
	// if we are in parent process, return the PID of the child
	if (child_pid != 0) 
		return child_pid;
	// if we are in child process, call execvp in order to sobstitute caller's image with the
	// executable file program
	else 
	{
    	execvp (program, arg_list);
    	perror("execvp failed");
    	return 1;
    }
}

// signal handler for signals from client
// this is done to receive a signal from client as soon as the child
// finishes reading the data
void sig_handler(int signo)
{
    if(signo == SIGINT)
        arrived = 'a';
}

int main()
{
    // signal handler
    signal(SIGINT, sig_handler);
    // getting PID of producer for signal handling
    int pidProducer = getpid();
    char buf_pidProducer[20];
    sprintf(buf_pidProducer, "%d", pidProducer);

    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
    // initialize client PID 
    int pidClient;

    // create the array that will contain one MB of random data
	int data_array[MUL_SIZE];
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = rand();
	}

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

    // defining a pipe for interprocess communication
    char* f_client = "/tmp/f_client";
    // create a pipe for interprocess communication
    int ret_mk_client = mkfifo(f_client, 0666);
	if (ret_mk_client< 0)
    {
        perror("f_client");
        unlink(f_client);
        return -1;
    }

    // spawning client process
    // passing to client the pid of the producer and the size given as input from user
    char * arg_list_client [] = { "./ClientNamedPipe", buf_pidProducer, buf_size, (char*)NULL };
    pidClient = spawn("./ClientNamedPipe", arg_list_client); 

    // open the pipe
    int fd_client;	
    fd_client = open(f_client, O_WRONLY);
	if (fd_client < 0) 
	{
        perror("fd_client");
        return -1;
    }

    // getting time before writing
    gettimeofday(&t_start, NULL);
    // write data from producer to client 
    for (int i=0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
			write(fd_client, &data_array[j], sizeof(data_array[j]));
		}
	}
    close(fd_client);

    // until the signal is not received means that client is still reading
    int counter = 0;
    bool not_received = true;
    while(not_received)
    {
        if (arrived == 'a') // when signal arrives enter the loop
        {
            gettimeofday(&t_end, NULL); // get time when client finishes reading
            not_received = false;
        }
        else
        {
            counter++;
        }
    }
    // calculate the time it took to write and read data
    double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
	printf("\nTime taken for transferring data with named pipe = %f sec\n\n", time_taken);
	fflush(stdout);

    unlink(f_client);
}
