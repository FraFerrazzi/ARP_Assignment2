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
#include <signal.h>

#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"
#define MUL_SIZE 500
#define RAND_DATA_SIZE 250000
#define CBUFF_SIZE 500
#define LOWER 0
#define UPPER 9

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

/*struct circular_buffer
{
    int cir_buffer[CBUFF_SIZE];
};*/

int main(int argc, char *argv[]) 
{
    void * addr;
    int offset = 0;
    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
    // signal handler
    signal(SIGINT, sig_handler);
    // defining shared memory and semaphores
    int shared_seg_size = (sizeof(struct shared_data));
    //int cbuff_seg_size = (sizeof(struct circular_buffer));
    int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, 0666);
    ftruncate(shmfd, shared_seg_size);
    //ftruncate(shmfd, cbuff_seg_size);
    struct shared_data * shared_msg = (struct shared_data *)
    //struct circular_buffer * circular_msg = (struct circular_buffer *)
    /*addr =*/ mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, offset);
    sem_t * sem_id1= sem_open(SEM_PATH_1, O_CREAT | O_RDWR, 0666, 1);
    sem_t * sem_id2= sem_open(SEM_PATH_2, O_CREAT | O_RDWR, 0666, 1);
    sem_t * sem_id3= sem_open(SEM_PATH_3, O_CREAT | O_RDWR, 0666, 1);
    sem_init(sem_id1, 1, 1); // initialized to 1
    sem_init(sem_id2, 1, CBUFF_SIZE); // initialized to dimension of circular buffer 
    sem_init(sem_id3, 1, 0); // initialized to zero 

    // create 1MB random data
    int data_array[RAND_DATA_SIZE];
	for (int i=0; i < RAND_DATA_SIZE; i++)
	{
		data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
        printf("%d", data_array[i]);
        fflush(stdout);
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
    sem_post(sem_id3);

    // getting time before writing
    gettimeofday(&t_start, NULL);
    //initializing a buffer msg
    int circular_buffer[CBUFF_SIZE];
    // write data from producer to client 
    for (int i=0; i < size; i++)
	{
		for (int j=0; j < MUL_SIZE; j++)
		{
            for(int k = 0; k < CBUFF_SIZE; k++)
            {
                //wait until array is full
                sem_wait(sem_id2);
                // wait cli
                sem_wait(sem_id1);
                memcpy(&circular_buffer[k], &data_array[j], sizeof(data_array[j]));
                // start cli
                sem_post(sem_id1);
                //increment until array is full
                sem_post(sem_id3);
            }
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
    munmap(addr, shared_seg_size);
    
    return 0;
}