// sem2 cons - run AFTER prod
// to compile gcc CircularBufferClient.c -lrt -pthread -o CircularBufferClient
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define SHMOBJ_PATH_1 "/shm_AOS_1"
#define SHMOBJ_PATH_2 "/shm_AOS_2"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"
#define MUL_SIZE 250000
#define CBUFF_SIZE 200

int rd_data_array[MUL_SIZE];
// initialize circular buffer
int * addr;
int circular_buffer[CBUFF_SIZE];

//Open a pointer file for writing in log file
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
 *Function that handles possible errors 
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
    // open the file
    logfile = fopen("./../logfile/CircularBuffer/CircularBufferClient.txt","w");
    if (logfile == NULL)
    {
        error_handler("fopen");
    }
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "%sCONSUMER PROGRAM STARTS:\n", asctime(timeinfo));
    fflush(logfile);
    int offset = 0;
     // defining shared memory and semaphores
    int shmfd_1 = shm_open(SHMOBJ_PATH_1, O_RDONLY, 0666);
    int shmfd_2 = shm_open(SHMOBJ_PATH_2, O_RDWR, 0666);
    int shared_seg_size = (sizeof(struct shared_data));
    int cbuff_seg_size = (sizeof(circular_buffer));
    struct shared_data * shared_msg = (struct shared_data *)
    // mapping memory
    mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd_1, offset);
    addr = mmap(NULL, cbuff_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_2, offset);
    struct shared_data info_prod_in;
    // opening semaphores
    sem_t * sem_id1 = sem_open(SEM_PATH_1, 0);
    sem_t * sem_id2 = sem_open(SEM_PATH_2, 0);
    sem_t * sem_id3 = sem_open(SEM_PATH_3, 0);
    // wait prod
    sem_wait(sem_id3);
    // receive PID and size to client
    memcpy(&info_prod_in, shared_msg, sizeof(struct shared_data));
    // start prod
    sem_post(sem_id1);
    // struct shared_data info_prod = { producerPid, size};
    int pidProducer = info_prod_in.var1;
    int size = info_prod_in.var2;
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sProducer Pid and chosen size are received:\n", asctime(timeinfo));
    fprintf(logfile,"PID: [%d]\n", pidProducer);
    fprintf(logfile,"Size: [%d]\n", size);
    
    int out = 0;
    //reading from shm
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n%sReading:\n", asctime(timeinfo));
    fflush(logfile);
    for (int i=0; i < size; i++)
	{
        time ( &rawtime );
  	    timeinfo = localtime ( &rawtime );
        fprintf(logfile,"\n\n%sMB number %d: \n", asctime(timeinfo), i+1);
        fflush(logfile);
	    //read 1MB data by data
		for (int j=0; j < MUL_SIZE; j++)
		{  
            // wait until array is empty 
            sem_wait(sem_id3);
            // wait producer
            sem_wait(sem_id1);
            rd_data_array[j] = addr[out];
			//print one value every 5000 on the logfile
            if (j % 5000 == 0)
            {
                fprintf(logfile, "[%d]-%d ", j, rd_data_array[j]);
                fflush(logfile);
            }
            out = (out + 1) % cbuff_seg_size; 
            // start prod
            sem_post(sem_id1);
            // increment until array is empty 
            sem_post(sem_id2); 
		}
	}
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile,"\n\n%sEnd of reading\n", asctime(timeinfo));
    fflush(logfile);
    // send signal to Producer as soon as client finishes reading
    time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
    fprintf(logfile, "\n%sSending signal of finishing consuming data\n", asctime(timeinfo));
    fprintf(logfile, "\n%sConsumer program exiting...\n", asctime(timeinfo));
    fflush(logfile);
    kill(pidProducer, SIGINT);

    sleep(1);
    fclose(logfile);
    shm_unlink(SHMOBJ_PATH_1);
    shm_unlink(SHMOBJ_PATH_2);
    sem_close(sem_id1);
    sem_close(sem_id2);
    sem_unlink(SEM_PATH_1);
    sem_unlink(SEM_PATH_2);
    sem_unlink(SEM_PATH_3);

    return 0;
}
