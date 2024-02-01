#include<stdio.h>
#include<sys/mman.h>
#include <fcntl.h>
int main(int argc, char *argv[]) {
	int fd, i;
	char *data;
	fd = open(argv[1], O_RDWR);
	getchar();
	data = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(data == NULL)
		return 10;
	for(i = 0; i < 4096; i++)
		putchar(data[i]);
	getchar();
	for(i = 0; i < 4096; i++)
		data[i] = 'x';
	munmap(data, 1024);
	getchar();
	close(fd);
	return 0;
}
