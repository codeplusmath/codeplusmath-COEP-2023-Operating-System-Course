#include <stdio.h>
#include <unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

int main(int argc, char *argv[]) {
	int fd, x;
	char buf[1024];
	fd = open(argv[1], O_RDONLY);
	printf("%d\n", fd);
	scanf("%d", &x);
	read(fd, buf, 100);	
	scanf("%d", &x);
	close(fd);
	scanf("%d", &x);
	return 0;
}
