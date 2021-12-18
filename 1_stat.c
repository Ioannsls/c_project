#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <grp.h>
#include <pwd.h>


// Функция для вывода даты и времени с миллисекундами	
//
// static void print_time(const char *type, const struct timespec *ts) {
//
//         time_t tv_sec = (time_t) ts->tv_sec;
//                 char buffer[sizeof("YYYY-mm-dd HH:MM:SS")];
//                         struct tm *tm = localtime(&tv_sec);
//                                 strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
//                                         printf ("%s %s.%09lu ", type, buffer, ts->tv_nsec);
//                                                 strftime(buffer, sizeof(buffer), "%z", tm);
//                                                         printf("%s\n", buffer);
//                                                         }
//
//
//                                                         //Тип файла
//                                                         const char * filetype (unsigned int mode)
//                                                         {
//                                                         	switch (mode & S_IFMT) {
//                                                         	                case S_IFBLK: return("block device");
//                                                         	                                case S_IFCHR: return("character device");
//                                                         	                                                case S_IFDIR: return("directory");
//                                                         	                                                                case S_IFIFO: return("FIFO/pipe");
//                                                         	                                                                                case S_IFLNK: return("symlink");
//                                                         	                                                                                                case S_IFREG: return("regular file");
//                                                         	                                                                                                                case S_IFSOCK: return("socket");
//                                                         	                                                                                                                                default: return("unknown?");
//                                                         	                                                                                                                                	}
//                                                         	                                                                                                                                	}
//
//                                                         	                                                                                                                                	//Права
//                                                         	                                                                                                                                	char * rights (mode_t mode)
//                                                         	                                                                                                                                	{
//                                                         	                                                                                                                                		char * mode_rights = malloc (sizeof("rwxrwxrwx"));
//                                                         	                                                                                                                                			int i = 8;
//                                                         	                                                                                                                                				for (i = 8; i >= 0; i--)
//                                                         	                                                                                                                                					       mode_rights[8-i] = (mode & (1 << i) ? "xwr"[i%3]: '-');
//
//                                                         	                                                                                                                                					       	if (S_ISUID & mode)
//                                                         	                                                                                                                                					       			mode_rights[2] = 's';
//                                                         	                                                                                                                                					       				if (S_ISGID & mode)
//                                                         	                                                                                                                                					       						mode_rights[5] = 's';
//                                                         	                                                                                                                                					       							if (S_ISVTX & mode)
//                                                         	                                                                                                                                					       									mode_rights[8] = 't';
//                                                         	                                                                                                                                					       										mode_rights[9] = '\0';
//                                                         	                                                                                                                                					       											return mode_rights;
//                                                         	                                                                                                                                					       											}
//
//
//                                                         	                                                                                                                                					       											// Имя пользователя
//                                                         	                                                                                                                                					       											const char * user_name(uid_t uid) {
//                                                         	                                                                                                                                					       												struct passwd *info = getpwuid(uid);
//                                                         	                                                                                                                                					       													return (info == NULL) ? NULL : info -> pw_name;
//                                                         	                                                                                                                                					       													}
//                                                         	                                                                                                                                					       													//Имя группы
//                                                         	                                                                                                                                					       													const char * group_name(uid_t uid) { 
//                                                         	                                                                                                                                					       														struct group *info = getgrgid(uid);
//                                                         	                                                                                                                                					       															return (info == NULL) ? NULL : info -> gr_name;
//                                                         	                                                                                                                                					       															}
//
//
//                                                         	                                                                                                                                					       															int main(int argc, char *argv[])
//                                                         	                                                                                                                                					       															{
//                                                         	                                                                                                                                					       																struct stat sb; 
//                                                         	                                                                                                                                					       																	
//                                                         	                                                                                                                                					       																		if (argc != 2) {
//                                                         	                                                                                                                                					       																				fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
//                                                         	                                                                                                                                					       																						exit(EXIT_FAILURE);
//                                                         	                                                                                                                                					       																							}
//                                                         	                                                                                                                                					       																								if (lstat(argv[1], &sb) == -1) {
//                                                         	                                                                                                                                					       																										perror("lstat");
//                                                         	                                                                                                                                					       																												exit(EXIT_FAILURE);
//                                                         	                                                                                                                                					       																													}
//                                                         	                                                                                                                                					       																														
//                                                         	                                                                                                                                					       																															printf("File name %s\n", argv[1]);
//                                                         	                                                                                                                                					       																																printf("ID of containing device: [%lxh, %ldd]\n", (long) (sb.st_dev), (long) (sb.st_dev));
//                                                         	                                                                                                                                					       																																	printf("File type: %s\n", filetype(sb.st_mode));
//                                                         	                                                                                                                                					       																																		printf("I-node number: %ld\n", (uintmax_t) sb.st_ino);
//                                                         	                                                                                                                                					       																																			char *permissions_str = rights(sb.st_mode);
//                                                         	                                                                                                                                					       																																				printf("Mode_rights: (%04o/%s)\n", sb.st_mode & ALLPERMS, permissions_str);
//                                                         	                                                                                                                                					       																																					free(permissions_str);
//                                                         	                                                                                                                                					       																																						printf("Link count: %ld\n", (long) sb.st_nlink);
//                                                         	                                                                                                                                					       																																							printf("Ownership UID=%ld/%s   GID=%ld/%s\n", (long) sb.st_uid, user_name(sb.st_uid), (long) sb.st_gid, group_name(sb.st_gid));
//                                                         	                                                                                                                                					       																																								printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
//                                                         	                                                                                                                                					       																																									printf("File size: %lld bytes\n", (long long) sb.st_size);
//                                                         	                                                                                                                                					       																																										printf("Blocks allocated: %lld\n", (long long) sb.st_blocks);
//
//                                                         	                                                                                                                                					       																																											print_time("Last status change:	  ", &sb.st_ctim);
//                                                         	                                                                                                                                					       																																											        print_time("Last file access:         ", &sb.st_atim);
//                                                         	                                                                                                                                					       																																											                print_time("Last file modification:   ", &sb.st_mtim);
//                                                         	                                                                                                                                					       																																											                   
//                                                         	                                                                                                                                					       																																											                   	exit(EXIT_SUCCESS);
//                                                         	                                                                                                                                					       																																											                   	/                                                         	                                                                                                                                					       																																											                   	       }
