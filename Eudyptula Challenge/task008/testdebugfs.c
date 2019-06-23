#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <pthread.h>

#define TEST_ID_FILE "/sys/kernel/debug/eudyptula/id"
#define TEST_JIFFIES_FILE "/sys/kernel/debug/eudyptula/jiffies"
#define TEST_FOO_FILE "/sys/kernel/debug/eudyptula/foo"
static char *test_file_array[] = {TEST_ID_FILE, TEST_JIFFIES_FILE, TEST_FOO_FILE};
static const char my_assigned_id[] = "7c1caf2f50d1";
static const char *test_foo_strings[] = {"This", "is", "a", "write", "test", "for", "file", "foo"};
void test_debugfs_file_write(int file_id, char *write_string)
{
	int readnum = 0;
	int writenum = 0;
	char *tmp = write_string;
	char *buf = (char *) malloc(sizeof(char) * 20);
	int fd = open(test_file_array[file_id], O_RDWR);
	
	memset(buf, '\0', sizeof(char) * 20);
	printf("fd = %d\n", fd);
	if (fd < 0) {
		printf("cannot open the file %s\n", test_file_array[file_id]);
		goto out;
	}
	printf("open the file %s successfully\n", test_file_array[file_id]);
	writenum = write(fd, tmp, strlen(tmp) + 1);
	if (writenum < 0) {
		printf("write the file %s error %d\n", test_file_array[file_id], writenum);
		goto out;
	} else {
		printf("write the file successfully\n");
	}
	readnum = read(fd, buf, 20);
	if (readnum <= 0) {
		printf("read the file error\n");
		goto out;
	}
	printf("the file content after writen is %s\n", buf);
out:
	free(buf);
	close(fd);
	printf("close the file\n");
}
void test_debugfs_file_read(int file_id)
{
	int readnum = 0;
	int writenum = 0;
	char *buf = (char *) malloc(sizeof(char) * 20);
	int fd = open(test_file_array[file_id], O_RDONLY);
	
	memset(buf, '\0', sizeof(char) * 20);
	printf("fd = %d\n", fd);
	if (fd < 0) {
		printf("cannot open the file %s\n", test_file_array[file_id]);
		return;
	}
	printf("open the file %s successfully\n", test_file_array[file_id]);
	readnum = read(fd, buf, 20);
	if (readnum <= 0) {
		printf("read the file error\n");
		close(fd);
		return;
	}
	if (file_id == 1)
		printf("the file content is %ld\n", *(unsigned long *)buf);
	else
		printf("the file content is %s\n", buf);
	printf("close the file\n");
}
void *thread_func_to_write()
{
	int i = 0;
	
	while (i < 8) {
		test_debugfs_file_write(2, test_foo_strings[i++]);
		sleep(1);
	}
}
void *thread_func_to_read()
{
	int i = 10;
	
	while (i-- > 0) {
		test_debugfs_file_read(2);
		sleep(1);
	}
}
int main(int argc, void *args[])
{	
	int test_id = 0;
	int test_case = 0;
	pthread_t test_write_thread_id;
	pthread_t test_read_thread_id;
	int result = 0;
	pthread_attr_t foo_read;
	pthread_attr_t foo_write;
	
	if (argc == 3) {
		test_id = atoi(args[1]);
		test_case = atoi(args[2]);
	} else {
		printf("please input the test id\n");
		return 0;
	}
	switch (test_id) {
	case 3:
		result = pthread_create(&test_write_thread_id, NULL, thread_func_to_write, NULL);
		result = pthread_create(&test_read_thread_id, NULL, thread_func_to_read, NULL);
		pthread_join(test_write_thread_id, NULL);
		pthread_join(test_read_thread_id, NULL);
		break;
	default:
		if (!test_case)
			test_debugfs_file_read(test_id);
		else
			test_debugfs_file_write(test_id, my_assigned_id);
		break;
	}
	return 0;
}
