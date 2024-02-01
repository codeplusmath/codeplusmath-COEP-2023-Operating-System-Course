#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
//A
int main() {
	int pid1;
	printf("hi\n");
	pid1 = fork(); // A->B
	if(pid1 == 0) {
		printf("in child pid1= %d\n", pid1);
	} else {
		printf("in parent pid1= %d\n", pid1);
	}
	printf("bye\n");
	sleep(10);
	fork(); // A->C, B->D
	printf("goodbye\n");
	sleep(10);
	return 0;
}
