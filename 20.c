#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
int main()
{
	void *p = mmap(
			NULL,
			sysconf(_SC_PAGE_SIZE),
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			-1,
			0
		      );
	if (p == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	printf("allocated page at %p\n", p);
	struct {
		sem_t s;
		unsigned long long i;
		double x, y;
		bool run;
	} *buf = p;

	sem_init(&buf->s, 1, 1);
	buf->i = 0;
	buf->x = 0;
	buf->y = 0;
	buf->run = true;
	printf("1. %llu %lf %lf\n", buf->i, buf->x, buf->y);

	pid_t child_id = fork();
	switch (child_id) {
		case -1:
			perror("fork");
			return 1;
		case 0:
			while (buf->run) {
				sem_wait(&buf->s);
				buf->i += 1;
				buf->x += 1;
				buf->y -= 1;
				sem_post(&buf->s);
			}
			return 0;
		default:
			for (unsigned i = 0; i < 5; i += 1) {
				sem_wait(&buf->s);
				printf("2. %llu %lf %lf\n", buf->i, buf->x, buf->y);
				sem_post(&buf->s);
				sleep(1);
			}
			printf("3. %llu %lf %lf\n", buf->i, buf->x, buf->y);
			return 0;
	}
	return 0;
}
