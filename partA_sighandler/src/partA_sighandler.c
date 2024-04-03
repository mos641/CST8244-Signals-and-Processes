#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void sigint_handler(int sig);

volatile sig_atomic_t usr1Happened = 0; // false

int main(void) {
	// initialize signal action handlerh
	struct sigaction sigAction;
	sigAction.sa_handler = sigint_handler;
	sigAction.sa_flags = 0;
	sigemptyset(&sigAction.sa_mask);

	// register signal - exit if an error occurs
	if (sigaction(SIGUSR1, &sigAction, NULL) == -1) {
		perror("signal action failure");
		exit(EXIT_FAILURE);
	}

	// wait for kill command
	printf("PID = %d : Running... \n", getpid());
	while (usr1Happened == 0) {
		sleep(1);
	}

	printf("PID = %d : Received USR1. \n", getpid());

	printf("PID = %d : Exiting. \n", getpid());
	exit(EXIT_SUCCESS);
}

void sigint_handler(int sig) {
	usr1Happened = 1; // true
}
