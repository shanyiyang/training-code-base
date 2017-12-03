/* Child Process. Send 10 number for read. */

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

#define SHAREDMEMPATH "\\tmp\\shmem.men"
#define SEMAPHOREPATH "/tmp/semaphore.sem"
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
	int* 		value = NULL;
	
	// Shared memory data pointer.
	int*		validflag_ptr = NULL;
	pass_v*		data_ptr = NULL;
	int 		run = 0;
	
	// Create a shared memory.
	if ((shmemid = shm_open(SHAREDMEMPATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
	{
		perror("shm_open");
		exit(EXIT_FAILURE);
	}
	printf("Create a shared memory file descriptor.\n");
	printf("The number of file descriptor is %d.\n", shmemid);
	printf("\n");
	
	// Total needed sharedmemory size.
	mem_size = sizeof(int) + sizeof(pass_v);
	printf("Get the needed shared memory size.\n");
	printf("Total shared memory size id %d.\n", (int) mem_size);
	printf("\n");
	
	// Set the file size.
	if (ftruncate(shmemid, mem_size) < 0)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}
	printf("Set shared memory size.\n");
	printf("The size of memory set %d bytes.\n", (int) mem_size);
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
		perror("mem_ptr");
		exit(EXIT_FAILURE);
	}
	printf("Map the shared memory.\n");
	printf("The start address is %p at end child process memory.\n", mem_ptr);
	printf("\n");

	// Initial a semaphore to protect data.
	if ((locker = sem_open(SEMAPHOREPATH, O_CREAT, 0644, 1)) < 0)
	{
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
	printf("Create a semaphore object.\n");
	printf("\n");
	
	// Display the size of the variable.
	printf("The size of the int variable is %d.\n", (int) sizeof(int));
	printf("The size of the pass_value variable is %d.\n", (int) sizeof(pass_v));
	printf("\n");
	
	// Set address of the data which is in the shared memory. 
	validflag_ptr = (int *) mem_ptr;
	data_ptr = (pass_v*) (mem_ptr + sizeof(int));
	printf("The address of the validflag pointer is %p.\n", validflag_ptr);
	printf("The address of the data_ptr pointer is %p.\n", data_ptr);
	printf("\n");
	
	// Initial shared memory.
	*validflag_ptr = 1;		// 0 : valid, 1 : invalid
	
	// Transer data into the shared memory.
	while (run < 10)
	{
		// Semaphore lock.
		if (sem_wait(locker) < 0)
		{
			perror("semaphore_wait");
			exit(EXIT_FAILURE);
		}
		printf("Critiacl section lock.\n");
		
		// Process data.
		if (*validflag_ptr == TRUE)
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
			// Wrap data into the cariable.
			var.num = run;
			sprintf(var.data, "%s", "Send");

			// Copy to shared memory.
			*data_ptr = var;

			// Display data.
			printf("%s %d\n", var.data, var.num);

			// Set valid flag is true.
			*validflag_ptr = TRUE;

			run ++;
			
			// Semaphore unlock.
			if (sem_post(locker) < 0)
			{
				perror("semaphore_post");
				exit(EXIT_FAILURE);
			}
			printf("Critiacl section unlock.\n");
		}
	}
	
	// Wait for read data.
	while (1)
	{
		if (*validflag_ptr == FALSE)
			break;
	}
	
	// Destroy the semaphor object.
	if (sem_destroy(locker) < 0)
	{
		perror("sem_destroy");
		exit(EXIT_FAILURE);
	}
	printf("Destroy the semaphore.\n");
	printf("\n");
	
	// Unmap the memory.
	if (munmap(mem_ptr, mem_size) < 0)
	{
		perror("mumap");
		exit(EXIT_FAILURE);
	}
	printf("Unmap the shared memory.\n");
	printf("\n");

	// Unlink the file.
	if (shm_unlink(SHAREDMEMPATH) < 0)
	{
		perror("shm_unlink");
		exit(EXIT_FAILURE);
	}
	printf("Unlink the file.\n");
	printf("\n");

	// Close shared memory file descriptor.
	if (close(shmemid) < 0)
	{
		perror("close");
		exit(EXIT_FAILURE);
	}
	printf("Close the shared memory file descriptor.\n");
	printf("\n");
	
	return EXIT_SUCCESS;
}