// gcc CircularBufferProducer.c -lrt -pthread -o CircularBufferProducer
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
//#include <lrt.h>
#include <signal.h>

#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define MUL_SIZE 250000

int data_array[MUL_SIZE];
char arrived;

// signal handler for signals from client
// this is done to receive a signal from client as soon as the child
// finishes reading the data
void sig_handler(int signo)
{
    if(signo == SIGINT)
        arrived = 'a';
}

struct shared_data 
{
    int var1;
    int var2;
};

int main(int argc, char *argv[]) 
{
    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
    // signal handler
    signal(SIGINT, sig_handler);
    // defining shared memory and semaphores
    int shared_seg_size = (sizeof(struct shared_data));
    int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, 0666);
    ftruncate(shmfd, shared_seg_size);
    struct shared_data * shared_msg = (struct shared_data *)
    mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    sem_t * sem_id1= sem_open(SEM_PATH_1, O_CREAT | O_RDWR, 0666, 1);
    sem_t * sem_id2= sem_open(SEM_PATH_2, O_CREAT | O_RDWR, 0666, 1);
    sem_init(sem_id1, 1, 1); // initialized to 1
    sem_init(sem_id2, 1, 0); // initialized to 0

    // put data into shmem
    int data_array[MUL_SIZE];
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = rand();
	}

    // getting input from user
    // this is done to define how many MB of data are going to be transferred
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
    // get producer PID
    int producerPid = getpid();
    struct shared_data info_prod = { producerPid, size};
    // wait cli
    sem_wait(sem_id1);
    // send PID and size to client
    memcpy(shared_msg, &info_prod, sizeof(struct shared_data));
    // start cli
    sem_post(sem_id2);

    // getting time before writing
    gettimeofday(&t_start, NULL);
    // write data from producer to client 
    for (int i=0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
            // wait cli
            sem_wait(sem_id1);
			memcpy(shared_msg, &data_array[j], sizeof(data_array[j]));
            // start cli
            sem_post(sem_id2);
		}
	}
    
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
	printf("\nTime taken for transferring data with circular buffer = %f sec\n\n", time_taken);
	fflush(stdout);

    shm_unlink(SHMOBJ_PATH);
    sem_close(sem_id1);
    sem_close(sem_id2);
    sem_unlink(SEM_PATH_1);
    sem_unlink(SEM_PATH_2);
    
    return 0;
}
