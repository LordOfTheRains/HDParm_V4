#include <unistd.h>
#include <stdio.h>

int main() {

	int mypid;
	mypid = getpid();

	fork();
	open();
	read();
	write();
	close();
	dup2();
	_execv();
	waitpid();
	lseek();

	reboot(RB_REBOOT);
	return 0;
}
