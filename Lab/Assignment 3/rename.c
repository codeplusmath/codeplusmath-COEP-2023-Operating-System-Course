#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char * argv[]){
	int rvalue;
	if(argc<=1){
		return 1;
	}
	else{
		rvalue = rename(argv[1], argv[2]);
		return rvalue;			
			
	}
}

