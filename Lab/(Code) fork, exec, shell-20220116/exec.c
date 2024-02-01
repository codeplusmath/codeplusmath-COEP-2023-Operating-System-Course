#include <stdio.h>
#include <unistd.h>
int main() {
	printf("hi\n");
	execl("/usr/bin/google-chrome", "/usr/bin/google-chrome", NULL);
	printf("bye\n");
}
