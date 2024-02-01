#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
int main() {
	int pid, ret;
	char cmd[128];
	while(1) {
		printf("prompt>");
		scanf("%s", cmd);
		if(strcmp(cmd, "exit") == 0)
			return 0;
		pid = fork(); // A->B
		if(pid == 0) {
			ret = execl(cmd, cmd, NULL);
			if(ret == -1) {
				perror("execution failed");
				exit(errno);
			}
		} else {
			wait(0);
		}
	}
	return 0;
}
