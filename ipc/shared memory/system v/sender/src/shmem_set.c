/* Child Process. Send 10 number for read. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
	key_t		key;
	pass_v		var;
	int			shmemid;
	void*		mem_ptr = NULL;
	size_t		mem_size = 0;
	sem_t*		locker;
	
	// Shared memory data pointer.
	int*		validflag_ptr = NULL;
	pass_v*		data_ptr = NULL;
	int 		run = 0;
	
	// Total needed sharedmemory size.
	mem_size = sizeof(int) + sizeof(pass_v);
	printf("Get the needed shared memory size.\n");
	printf("Total shared memory size id %d.\n", (int) mem_size);
	printf("\n");
	
	// Create a ipc key
	key = ftok(SHAREDMEMPATH, 'S');
	
	// Create a shared memory.
	if ((shmemid = shmget(key, mem_size, S_IRUSR | S_IWUSR | IPC_CREAT)) < 0)
	{
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	printf("Create a shared memory file descriptor.\n");
	printf("The size of memory set %d bytes.\n", (int) mem_size);
	printf("The number of file descriptor is %d.\n", shmemid);
	printf("\n");

	// Map file to memory.
	if ((mem_ptr = shmat(shmemid, (void *)0x87000000, 0)) < 0)
	{
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	printf("Map the shared memory.\n");
	printf("The start address is %p at end child process memory.\n", mem_ptr);
	printf("\n");
	
	// Initial a semaphore to protect data.
	if ((locker = sem_open(SEMAPHOREPATH, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) < 0)
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
	
	// Initial validflag.
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
		}
	}
	
	// Wait for read data.
	while (1)
	{
		if (*validflag_ptr == FALSE)
			break;
	}
	printf("\n");
	
	// Unlink semaphore file.
	if (sem_unlink(SEMAPHOREPATH) < 0)
	{
		perror("sem_unlink");
		exit(EXIT_FAILURE);
	}
	printf("Unlink the file name ofthe semaphore file.\n");
	printf("\n");
	
	// Destroy the semaphor object.
	if (sem_destroy(locker) < 0)
	{
		perror("sem_destroy");
		exit(EXIT_FAILURE);
	}
	printf("Destroy the semaphore.\n");
	printf("\n");
	
	// Detach the shared memory.
	if (shmdt(mem_ptr) < 0)
	{
		perror("shmdt");
		exit(EXIT_FAILURE);
	}
	printf("Detach the shared memory.\n");
	printf("\n");

	// Destroy the shared memory object.
	if (shmctl(shmemid, IPC_RMID, 0) < 0)
	{
		perror("shmctl");
		exit(EXIT_FAILURE);
	}
	printf("Close the shared memory file descriptor.\n");
	printf("\n");
	
	return EXIT_SUCCESS;
}