#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define BUF_SIZE 8192

int main(int argc, char *argv[])
{
	int fd1,fd2,len;
	char buf[BUF_SIZE];

	fd1=open("test.tar.gz",O_RDONLY);
	fd2=open("cpy.tar.gz",O_WRONLY|O_CREAT|O_TRUNC);

	while((len=read(fd1,buf,sizeof(buf)))>0)
		write(fd2,buf,len);

	close(fd1);
	close(fd2);
	return 0;
}
