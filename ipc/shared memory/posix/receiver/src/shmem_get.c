/* Parent Process. Read 10 numnber for write. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#define SHAREDMEMPATH "shmem.men"
#define SEMAPHOREPATH "semaphore.sem"
#define TRUE 0
#define FALSE 1

struct pass_value
{
	int 		num;
	char 		data[10];
};
typedef struct pass_value pass_v;

int main()
{
	pass_v		var;
	int			shmemid;
	void*		mem_ptr = NULL;
	size_t		mem_size = 0;
	sem_t*		locker;
	struct stat	status;
	
	// Shared memory data pointer.
	int*		validflag_ptr = NULL;
	pass_v*		data_ptr = NULL;
	int 		run = 0;
	
	// Open a file for shared memory.
	if ((shmemid = shm_open(SHAREDMEMPATH, O_RDWR, 0)) < 0)
	{
		perror("shm_open");
		exit(EXIT_FAILURE);
	}
	printf("Opne a shared memory file descriptor.\n");
	printf("The number of file descriptor is %d.\n", shmemid);
	printf("\n");
	
	// Total needed sharedmemory size.
	mem_size = sizeof(int) + sizeof(pass_v);
	printf("Get the needed shared memory size.\n");
	printf("Total shared memory size id %d.\n", (int) mem_size);
	printf("\n");
	
	// Get shared memory status.
	if (fstat(shmemid, &status) < 0)
	{
		perror("fstat");
		exit(EXIT_FAILURE);
	}
	printf("Get shared memory status.\n");
	printf("The size of shared memory is %lld bytes.\n", (long long) status.st_size);
	printf("\n");

	// Map file to memory.
	if ((mem_ptr = mmap((void *)0x87000000, status.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmemid, 0)) < 0)
	{
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	printf("Map the shared memory.\n");
	printf("The start address is %p at end child process memory.\n", mem_ptr);
	printf("\n");
	
	// Open a semaphore to protect data.
	if ((locker = sem_open(SEMAPHOREPATH, O_CREAT)) < 0)
	{
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
	printf("Open a semaphore object.\n");
	printf("\n");
	
	// Set address of the data which is in the shared memory. 
	validflag_ptr = (int *) mem_ptr;
	data_ptr = (pass_v*) (mem_ptr + sizeof(int));
	printf("The size of the validflag_ptr pointer is %p.\n", validflag_ptr);
	printf("The size of the data_ptr pointer is %p.\n", data_ptr);
	printf("\n");
	
	// Transer data into the shared memory.
	while (run < 10)
	{
		// Semaphore lock.
		if (sem_wait(locker) < 0)
		{
			perror("semaphore_wait");
			exit(EXIT_FAILURE);
		}
	 	
		if (*validflag_ptr == FALSE)
		{
			//
			// Semaphore unlock.
			if (sem_post(locker) < 0)
			{
				perror("semaphore_post");
				exit(EXIT_FAILURE);
			}
			continue;
		}
		else
		{
			// Copy data from shared memory to process memory.
			var = *((pass_v *) data_ptr);

			// Display data.
			printf("%s %d\n", var.data, var.num);

			// Set valid flag is true.
			*validflag_ptr = FALSE;

			run++;
			
			// Semaphore unlock.
			if (sem_post(locker) < 0)
			{
				perror("semaphore_post");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	printf("\n");
	
	// Detach the shared memory.
	if (munmap(mem_ptr, mem_size) < 0)
	{
		perror("mumap ");
		exit(EXIT_FAILURE);
	}
	printf("Detach the shared memory.\n");
	printf("\n");

	close(shmemid);
	printf("Close the shared memory file descriptor.\n");
	printf("\n");
	
	return EXIT_SUCCESS;
}