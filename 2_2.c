#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	 if (argc < 3) {
		 printf("Enter the file name and contents to write\n");
		 return 1;
	 }
	 int fd;
	 fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, 0644);
	 if (fd == -1) {
		 perror("Failed to open");
		 return 2;
	 }
	 for (int i = 2; i < argc; i += 1) {
		 if (dprintf(fd, "%s ", argv[i]) < 0) {
			 perror("Failed to write");
			 return 4;
		 }
	 }
	 if (close(fd) != 0) {
		 perror("Failed to close");
		 return 3;
	 }
	 return 0;
}

