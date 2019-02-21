#include "userapp.h"

void register_proc(int pid) {
	char buffer[4096];
	sprintf(buffer, "echo %d >/proc/mp1/status", pid);
	system(buffer);
}

void read_proc()
{
	FILE * fp;
	fp = fopen("/proc/mp1/status", "w+");
	fprintf(fp, "%d", pid);
    fclose(fp);
}

int factorial(int n){
	if (n < 2) {
		return 1;
	}
	return n*factorial(n-1);
}

int main(int argc, char* argv[])
{
	// Write the pid into proc file
	int pid = getpid();
	int n = 10;
	register_proc(pid);
	find_factorials(n);
	// Read the proc file
	read_proc();
	return 0;
}
