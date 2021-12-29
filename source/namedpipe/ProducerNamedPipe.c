#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h> 
#include <signal.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define MUL_SIZE 250000
char arrived;
#define LOWER 0
#define UPPER 9

//Open a pointer file for writing in log file
FILE *logfile;

// initialize variables for time informations for log file
time_t rawtime;
struct tm * timeinfo;

/* 
 * Function used for the handling signals from client
 * producer receives a signal from client as soon as the child
 * finishes reading the data
 * when signal arrives, get the time
 */
void sig_handler(int signo)
{
    if(signo == SIGINT)
        arrived = 'a';
}

/*
 *Function that handles possible errors 
 */
void error_handler(char *msg)
{
    perror(msg);
    fprintf(logfile, "%s", msg);
    fflush(logfile);
    exit(0);
}

/*
 * Function to spawn processes
 */
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
    	perror("execvp failed\n");
        return 1;
    }
}


int main()
{
    logfile = fopen("./../logfile/NamedPipe/logfileProducerNamedPipe.txt", "w");
    if(logfile == NULL)
    {
        printf("Error in opening logfile\n");
        exit(1);
    }
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "%sPRODUCER PROGRAM STARTS:\n", asctime(timeinfo));
    fflush(logfile);
    // when signal arrives, sig_handler function is called
    signal(SIGINT, sig_handler);
    // getting PID of producer for signal handling
    int pidProducer = getpid();
    char buf_pidProducer[20];
    sprintf(buf_pidProducer, "%d", pidProducer);
    //fprintf(logfile,"Size: %d\n", size);
    fflush(logfile);
    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
    // initialize client PID 
    int pidClient;

    // create the array that will contain one MB of random data
	int data_array[MUL_SIZE];
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
	}
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sdata_array for writing filled\n", asctime(timeinfo));
    fflush(logfile);
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
        unlink(f_client);
        error_handler("f_client");
    }

    // spawning client process
    // passing to client the pid of the producer and the size given as input from user
    char * arg_list_client [] = { "./ClientNamedPipe", buf_pidProducer, buf_size, (char*)NULL };
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sProducer Pid and chosen size are sent:\n", asctime(timeinfo));
    fprintf(logfile,"PID: [%d]\n", pidProducer);
    fprintf(logfile,"Size: [%d]\n", size);
    fflush(logfile);
    pidClient = spawn("./ClientNamedPipe", arg_list_client);
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime ); 
    fprintf(logfile,"\n%sProducer forked Consumer\n", asctime(timeinfo));
    fflush(logfile);
    // open the pipe
    int fd_client;	
    fd_client = open(f_client, O_WRONLY);
	if (fd_client < 0) 
	{
        error_handler("fd_client");
    }
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sFile descriptor for writing opened\n", asctime(timeinfo));
    fflush(logfile);
    // getting time before writing
    gettimeofday(&t_start, NULL);
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sGet time before starting to write on the pipe\n", asctime(timeinfo));
    fflush(logfile);
    // write data from producer to client 
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sWriting:", asctime(timeinfo));
    fflush(logfile);
    for (int i=0; i < size; i++)
	{
        time ( &rawtime );
  	    timeinfo = localtime ( &rawtime );
        fprintf(logfile,"\n\n%sMB number %d: \n", asctime(timeinfo), i+1);
        fflush(logfile);
		for (int j=0; j < MUL_SIZE; j++)
		{
			write(fd_client, &data_array[j], sizeof(data_array[j]));
            if (j % 5000 == 0)
            {
                fprintf(logfile,"[%d]-%d; ", j, data_array[j]);
                fflush(logfile);
            }
		}
	}
    close(fd_client);
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n\n%sEnd of writing\n", asctime(timeinfo));
    fflush(logfile);
    // until the signal is not received means that client is still reading
    int counter = 0;
    bool not_received = true;
    while(not_received)
    {
        if (arrived == 'a') // when signal arrives enter the loop
        {
            gettimeofday(&t_end, NULL); // get time when client finishes reading
            time ( &rawtime );
  	        timeinfo = localtime ( &rawtime );
            fprintf(logfile,"\n%sGet time at the end of transmission\n", asctime(timeinfo));
            fflush(logfile);
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
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sTime taken for transferring data with named pipe = %f sec\n\n", asctime(timeinfo), time_taken);
    fflush(logfile);
    unlink(f_client);
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sProducer program exiting...\n", asctime(timeinfo));
    fflush(logfile);
    fclose(logfile);
}
