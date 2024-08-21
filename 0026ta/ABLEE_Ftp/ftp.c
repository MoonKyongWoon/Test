
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

pid_t pid;

int main(int argc, char *argv[])
{
	int status;
	char *s;

//	setpgrp(0, getpid());
	setpgid(0, getpid());

	if ((pid = fork()) < 0) {
		return -1;
	}
	else if (pid == 0) {
		if ((s = strstr(argv[0], ".cc"))) {
			s[1] = 's';
			s[2] = 'h';
		}

		execvp(argv[0], &argv[0]);
	}
	else {
		int ret;
		int st = time(0), et;

		do {
			if ((ret = waitpid(pid, &status, WNOHANG)) < 0)
				return -1;
			else if (ret == 0)
				usleep(500 * 1000);
			else
				break;

			et = time(0);

			if (et - st > 30) {
				kill(0, 9);
				break;
			}
		} while (1);
	}

	return 0;
}
