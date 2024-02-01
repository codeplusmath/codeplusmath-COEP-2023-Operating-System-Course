#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int i, j;
	printf("%d\n", getpid());
	scanf("%d", &i);
	for(i = 0; i < 3; i++) {
		sleep(1);
		for(j = 0; j < INT_MAX - 1; j++)
			;
	}
	j = fork();
	if(j == 0) {
		sleep(5);
		printf("waiting 10 secs , then I will be zombie\n");
		sleep(10);
		printf("child going to be zombie\n");
		exit(1);
	} else {
		printf("child is %d, waiting 20 secs to allow child to be zombie\n", j);
		sleep(20);
	}
}
