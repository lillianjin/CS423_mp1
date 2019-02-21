#include "userapp.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void read_proc()
{
	char *buf = malloc(4096);
    memset(buf, 0, 4096);
	int fp = open("/proc/mp1/status", O_RDONLY);
	read(fp, buf, 4096);
	printf("%s", buf);
    close(fp);
}

void write_proc(int pid)
{
	char *buf = malloc(4096);
    memset(buf, 0, 4096);
	int fp = open("/proc/mp1/status", O_WRONLY);
	int count = sprintf(buf, "%d", pid);
	write(fp, buf, count + 1);
    close(fp);
}


int main(int argc, char* argv[])
{
	// Write the pid into proc file
	int pid = getpid();
	// Write from user
	write_proc(pid);
	for(int i=0; i<1000000000; i++){
		// if(i%1000==0){
		// 	printf("Loaded 1000 numbers!\n");
		// }
		long long res = 1;
		for(int j=20; j > 0; j--){
			res = res * j;
		}
	}
	// Read the proc file
	read_proc();
	return 0;
}
