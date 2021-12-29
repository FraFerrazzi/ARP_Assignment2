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

#define SHMOBJ_PATH_1 "/shm_AOS_1"
#define SHMOBJ_PATH_2 "/shm_AOS_2"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"
#define MUL_SIZE 250000
#define CBUFF_SIZE 200
#define LOWER 0
#define UPPER 9

int data_array[MUL_SIZE];
char arrived;
// initialize circular buffer
int * addr;
int circular_buffer[CBUFF_SIZE];
//initialize file for logfile
FILE *logfile;
// initialize variables for time informations for log file
time_t rawtime;
struct tm * timeinfo;
/*
 * Defining struct for shared data 
 */
struct shared_data 
{
    int var1;
    int var2;
};

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
    // when signal arrives, sig_handler function is called
    signal(SIGINT, sig_handler);
    // file for getting the size from user
    FILE *stream;
    int offset = 0;
    int size;
    logfile = fopen("./../logfile/CircularBuffer/logfileCircularBufferProducer.txt", "w");
    if(logfile == NULL)
    {
        printf("Error in opening logfile\n");
        exit(1);
    }
    // open the file
    //stream = fopen("./circularbufferdata.txt","r");
    if (stream == NULL)
    {
        error_handler("fopen");
    }

    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "%sPRODUCER PROGRAM STARTS:\n", asctime(timeinfo));
    fflush(logfile);

    // read size from file
    fscanf(stream, "%d", &size);
    fclose(stream);
    // initialize time struct to get the time of data transfer
	struct timeval t_start, t_end;
    // defining shared memory and semaphores
    int shared_seg_size = (sizeof(struct shared_data));
    int cbuff_seg_size = (sizeof(circular_buffer));
    int shmfd_1 = shm_open(SHMOBJ_PATH_1, O_CREAT | O_RDWR, 0666);
    int shmfd_2 = shm_open(SHMOBJ_PATH_2, O_CREAT | O_RDWR, 0666);
    ftruncate(shmfd_1, shared_seg_size);
    ftruncate(shmfd_2, cbuff_seg_size);
    struct shared_data * shared_msg = (struct shared_data *)
    // mapping shared memory
    mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_1, offset);
    addr = mmap(NULL, cbuff_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_2, offset);
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sShared memory addressed\n", asctime(timeinfo));
    fflush(logfile);
    // opening semaphores
    sem_t * sem_id1= sem_open(SEM_PATH_1, O_CREAT | O_RDWR, 0666, 1);
    sem_t * sem_id2= sem_open(SEM_PATH_2, O_CREAT | O_RDWR, 0666, 1);
    sem_t * sem_id3= sem_open(SEM_PATH_3, O_CREAT | O_RDWR, 0666, 1);
    // initialize semaphores 
    sem_init(sem_id1, 1, 1); // initialized to 1
    sem_init(sem_id2, 1, cbuff_seg_size); // initialized to dimension of circular buffer 
    sem_init(sem_id3, 1, 0); // initialized to zero 
    // create 1MB random data
    int data_array[MUL_SIZE];
	for (int i=0; i < MUL_SIZE; i++)
	{
		data_array[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
	}
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
	fprintf(logfile,"\n%sdata_array for writing filled\n", asctime(timeinfo));
    fflush(logfile);

    // get producer PID
    int producerPid = getpid();
    struct shared_data info_prod = { producerPid, size};
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sProducer Pid and chosen size are sent:\n", asctime(timeinfo));
    fprintf(logfile,"PID: [%d]\n", producerPid);
    fprintf(logfile,"Size: [%d]\n", size);
    fflush(logfile);
    // wait cli
    sem_wait(sem_id1);
    // send PID and size to client
    memcpy(shared_msg, &info_prod, sizeof(struct shared_data));
    // start cli
    sem_post(sem_id3);

    int in = 0;
    // getting time before writing
    gettimeofday(&t_start, NULL);
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sGet time before starting to write on the shared memory\n", asctime(timeinfo));
    fflush(logfile);
    // write data from producer to client 
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sWriting:\n", asctime(timeinfo));
    fflush(logfile);
    for (int i=0; i < size; i++)
	{
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        fprintf(logfile, "\n\n%sMB number: %d\n", asctime(timeinfo), i+1);
        fflush(logfile);
        for (int j=0; j < MUL_SIZE; j++)
		{
            //wait until array is full
            sem_wait(sem_id2);
            // wait cli
            sem_wait(sem_id1);
            addr[in] = data_array[j];
            if (j % 5000 == 0)
            {
                fprintf(logfile, "[%d]-%d; ", j, addr[in]);
                fflush(logfile);
            }
            in = (in + 1) % cbuff_seg_size;
            // start cli
            sem_post(sem_id1);
            //increment until array is full
            sem_post(sem_id3);
		}
	}
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
            not_received = false;
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            fprintf(logfile,"\n%sGet time at the end of transmission\n", asctime(timeinfo));
            fflush(logfile);
        }
        else
        {
            counter++;
        }
    }
    // calculate the time it took to write and read data
    double time_taken = ((double)t_end.tv_sec + (double)t_end.tv_usec/1000000) - ((double)t_start.tv_sec + (double)t_start.tv_usec/1000000);
    // open the file
    stream = fopen("./circularbufferdata.txt","w");
    if (stream == NULL)
    {
        error_handler("fopen");
    }
	fprintf(stream, "\n\nTime taken for transferring data with circular buffer = %f sec\n\n", time_taken);
	fflush(stream);
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sTime taken for transferring data with circular buffer = %f sec\n\n",asctime(timeinfo), time_taken);
    fflush(logfile);
    

    sleep(1);
    shm_unlink(SHMOBJ_PATH_1);
    shm_unlink(SHMOBJ_PATH_2);
    sem_close(sem_id1);
    sem_close(sem_id2);
    sem_unlink(SEM_PATH_1);
    sem_unlink(SEM_PATH_2);
    sem_unlink(SEM_PATH_3);
    munmap(addr, cbuff_seg_size);
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sProducer program exiting...\n", asctime(timeinfo));
    fflush(logfile);
    fclose(logfile);
    return 0;
}