#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
	struct stat sb;

	if (argc != 2) 	
	{
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		return 1;
	}

	if (stat(argv[1], &sb) == -1) 
	{
		perror("stat");
		return 1;
	}

	printf("File type:                ");

	switch (sb.st_mode & S_IFMT)                    //파일의 종류검사 S_IFMT=0170000
	{
		case S_IFBLK:  printf("block device\n");            break;   //S_IFBLK=0060000
		case S_IFCHR:  printf("character device\n");        break;   //S_IFCHR=0020000
		case S_IFDIR:  printf("directory\n");               break;   //S_IFDIR=0040000
		case S_IFIFO:  printf("FIFO/pipe\n");               break;   //S_IFIFO=0010000
		case S_IFLNK:  printf("symlink\n");                 break;   //S_IFLNK=0120000
		case S_IFREG:  printf("regular file\n");            break;   //S_IFREG=0100000
		case S_IFSOCK: printf("socket\n");                  break;   //S_IFSOCK=0140000
		default:       printf("unknown?\n");                break;
	}

	   printf("I-node number:            %ld\n", (long) sb.st_ino);
		printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);
		printf("Link count:               %ld\n", (long) sb.st_nlink);
		printf("Ownership:                UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
		printf("Preferred I/O block size: %ld bytes\n",         (long) sb.st_blksize);
		printf("File size:                %lld bytes\n",        (long long) sb.st_size);
		printf("Blocks allocated:         %lld\n",              (long long) sb.st_blocks);
		printf("Last status change:       %s", ctime(&sb.st_ctime));
		printf("Last file access:         %s", ctime(&sb.st_atime));
		printf("Last file modification:   %s", ctime(&sb.st_mtime));

		return 0;
}
