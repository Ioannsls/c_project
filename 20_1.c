#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHMSIZE 512
#define BUF_SIZE 512
#define MODE S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH

struct shmbuf {
	sem_t  sem1;            /* POSIX unnamed semaphore */ 
	size_t cnt;             /* Number of bytes used in 'buf' */
	char   buf[BUF_SIZE];   /* Data being transferred */
};
int done = 0;
void term(int signum) {
	done = 1;
}
int main (){
	char *shm_name = "SharedMemory";
	int fd;
	/* Open an Shared Memory Object for Read-/Write-Access */
	if((fd = shm_open(shm_name, O_RDWR | O_CREAT, MODE)) < 0) {
		perror("\nshm_open() in Caretaker failed");
	}
	/* Truncate Shared Memory Object to specific size */
	if((ftruncate(fd, SHMSIZE) < 0)) {
		perror("\nftruncate() in Caretaker failed");
	}

	struct shmbuf *shmp = mmap(
			NULL,
			sizeof(*shmp),
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			fd,
			0
			);
	if (shmp == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	time_t timer;
	struct tm* tm_info;
	timer = time(NULL);
	if (sem_init(&shmp->sem1, 1, 0) == -1){
		perror("sem init");
		return 2;
	}
	action.sa_handler = term;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	while(!done){
		Sleep(1);
		tm_info = localtime(&timer);
		if (sem_wait(&shmp->sem1) == -1){
			perror("sem init");
			return 3;
		}
		strftime(shmp->buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
		if (sem_post(&shmp->sem1) == -1){
			perror("sem post");
			return 4;
		}
	}
	shm_unlink(shm_name);
	return 0;
}
