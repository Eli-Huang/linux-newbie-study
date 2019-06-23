#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>

void main(){
	int readnum = 0;
	int writenum = 0;
	char* buf = (char* )malloc(sizeof(char)*20);
	char *tmp = NULL;
	memset(buf,'\0',sizeof(char)*20);
	int fd = open("/dev/eudyptula",O_RDWR);
	int times = 10;
	
	printf("fd = %d\n",fd);
	if(fd < 0){
		printf("cannot open the dev\n");
		return;
	}
	/*readnum = read(fd,buf,20);
	if(readnum <= 0){
		printf("read the dev error\n");
		close(fd);
		return;
	}
	printf("the dev content is %s\n",buf);
	*/
	buf = "this is a test";
	tmp = buf;
	//tmp = "this is a test";
	//memcpy(buf,tmp,strlen(tmp));
	while((times--) > 0){
		writenum = write(fd,tmp,strlen(tmp));
		if(writenum < 0){
			printf("write the dev error %d\n",writenum);
			
		}else{
			printf("write the dev successfully\n");
		}
		sleep(3);
		tmp = buf;
	}
	close(fd);
	free(buf); //buf 已经被重新赋予常量字符串指针地址，动态分配内存已经变成野地址，释放会报段错误。
	printf("close the dev\n");
	return;
	
	
	
		
}
