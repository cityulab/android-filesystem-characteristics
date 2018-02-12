#include <sys/klog.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define KLOG_CLOSE         0
#define KLOG_OPEN          1
#define KLOG_READ          2
#define KLOG_READ_ALL      3
#define KLOG_READ_CLEAR    4
#define KLOG_CLEAR         5
#define KLOG_CONSOLE_OFF   6
#define KLOG_CONSOLE_ON    7
#define KLOG_CONSOLE_LEVEL 8
#define KLOG_SIZE_UNREAD   9
#define KLOG_SIZE_BUFFER   10
#define DEFAULT_BUFFER_SIZE 1024 * 64
#define INTERVAL           20

struct loginfo{
	char path[50];
};

void * handler(void * data){
	int bufsize = 0, count;
	char * buffer;
	struct loginfo * info = (struct loginfo *)data;
	FILE * fp = fopen(info->path, "wa");
	if(!fp){
		printf("error in openning a file\n");
		return NULL;
	}
	bufsize = klogctl(KLOG_SIZE_BUFFER, 0 ,0);
	if(bufsize <= 0){
		bufsize = DEFAULT_BUFFER_SIZE;
	}
	buffer = (char *)malloc(bufsize + 1);
	if(!buffer){
		printf("error in allocating memory\n");
		return NULL;
	}
	while(1){
		count = klogctl(KLOG_READ_CLEAR, buffer ,bufsize);
		if(count < 0){
			printf("error in getting log message\n");
			return NULL;
		}
		buffer[count] = '\0';
		fwrite(buffer, count, 1, fp);
		fflush(fp);
		sleep(INTERVAL);
	}
	fclose(fp);
	free(buffer);
}

int main(){
	struct loginfo info = {
			.path = "/sdcard/record.log"
	};
	printf("start threads\n");
	pthread_t tid = 1;
	pthread_create(&tid, NULL, handler, &info);
	pthread_join(tid, NULL);
}