#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
int main(int argc, char *argv[]) {
	int fd; 
	char arr[128], *d;
	fd = open(argv[1], O_RDWR);
	assert(fd != -1);
	d = mmap(NULL, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	assert(d);
	putchar(d[10]);
	read(fd, arr, 128);
	arr[10] = 'x';
	lseek(fd, SEEK_SET, 0);
	write(fd, arr, 128);
	putchar(d[10]);
}

