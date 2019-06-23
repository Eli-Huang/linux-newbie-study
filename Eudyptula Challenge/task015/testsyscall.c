#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int err = 0;
	
	//err = syscall(SYS_eudyptula, 31772, 2939113681);
	err = syscall(387, 31772, 2939113681);
	if (err != 0) 
		printf("error is %d\n", errno);
	else
		printf("syscall return true\n");

	//err = syscall(SYS_eudyptula, 31771, 2939113681);
	err = syscall(387, 0x7c1c, 0xaf2f50d1);
	if (err != 0) 
		printf("error is %d\n", errno);
	else
		printf("syscall return true\n");
	return err;
}
