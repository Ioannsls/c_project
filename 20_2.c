#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define BUF_SIZE 512
#define SHM_NAME "/clock"
#define TIMESTR_SIZE 64
volatile int g_terminate = 0;
 
struct shmbuf {
	sem_t  sem1;            /* POSIX unnamed semaphore */
	size_t cnt;             /* Number of bytes used in 'buf' */
	char   buf[BUF_SIZE];   /* Data being transferred */
};
 
void handler(int sig) {
    (void)sig;
    g_terminate = 1;
}
int main() {
    struct sigaction term_action = {};
    term_action.sa_flags = SA_RESTART;
    term_action.sa_handler = handler;
    if ((sigaction(SIGTERM, &term_action, NULL)) 
    || (sigaction(SIGINT, &term_action, NULL))) {
        perror("sigaction");
        return 1;
    }
 
    int shm_des = shm_open(SHM_NAME, O_RDWR, 0644); // File mode is ignored
    if (shm_des < 0) {
        if (errno == ENOENT) {
            printf("Could not find time reference file\n");
            return 0;
        } else {
            perror("Could not open shared memory file");
            return 1;
        }
    }
 
    struct shmbuf *shmp = mmap(
        NULL,
        sysconf(_SC_PAGE_SIZE),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_des,
        0);
    if (shmp == MAP_FAILED) {
        perror("Failed to map shared memory");
        close(shm_des);
        unlink(SHM_NAME);
        return 3;
    }
    time_t timer;
    struct tm* tm_info;
    close(shm_des);
    printf("Allocated page at [%p]\n", shmp);
    while(!g_terminate) {
	if (sem_wait(&shmp->sem1) == -1) {
		perror("sem init");
		sem_destroy(&shmp->sem1);
		return 4;
	}
	timer = time(NULL);
	tm_info = localtime(&timer);
	strftime(shmp->buf, sizeof(shmp->buf), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("[%.*s]\n", TIMESTR_SIZE, shmp->buf);
	if (sem_post(&shmp->sem1) == -1){
		perror("sem post");
		sem_destroy(&shmp->sem1);
		return 7;
	}
        sleep(1);
    }
    printf("\nStopping client...\n");
    return 0;
}
