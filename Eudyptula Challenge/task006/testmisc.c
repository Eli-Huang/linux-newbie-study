#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>

void main(){
	int readnum = 0;
	int writenum = 0;
	char* buf = (char* )malloc(sizeof(char)*20);
	memset(buf,'\0',sizeof(char)*20);
	int fd = open("/dev/eudyptula",O_RDWR);
	printf("fd = %d\n",fd);
	if(fd < 0){
		printf("cannot open the dev\n");
		return;
	}
	readnum = read(fd,buf,20);
	if(readnum <= 0){
		printf("read the dev error\n");
		close(fd);
		return;
	}
	printf("the dev content is %s\n",buf);
	writenum = write(fd,buf,strlen(buf));
	if(writenum < 0){
		printf("write the dev error %d\n",writenum);
	}else{
		printf("write the dev successfully\n");
	}
	close(fd);
	printf("close the dev\n");
	return;
	
	
	
		
}
