#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigint_handler(int sig);
int user_input(char msg[]);
void child_process();
void parent_process();

volatile sig_atomic_t usr1Happened = 0; // false

int main(void) {
	pid_t pid = 1;

	// initialize signal action handler
	struct sigaction sigAction;
	sigAction.sa_handler = sigint_handler;
	sigAction.sa_flags = 0;
	sigemptyset(&sigAction.sa_mask);

	// register signal - exit if an error occurs
	if (sigaction(SIGUSR1, &sigAction, NULL) == -1) {
		perror("signal action failure");
		exit(EXIT_FAILURE);
	}

	// ask and store the amount of children
	int numOfChildren = user_input("Enter the number of children:\n");

	printf("PID = %d: Parent running... \n", getpid());

	// create the number of children with fork
	int i = 0;
	while (pid != 0 && i < numOfChildren) {
		pid = fork();
		i++;
	}

	switch (pid) {
	case -1:
		// -1 indicates an error
		perror("fork error");

		exit(EXIT_FAILURE);
		break;
	case 0:
		// child process receives 0
		child_process();
		break;
	default:
		// parent process is other than -1 or 0
		parent_process();
		break;
	}

	printf("PID = %d: Children finished, parent exiting. \n", getpid());

	exit(EXIT_SUCCESS);
}

void sigint_handler(int sig) {
	usr1Happened = 1; // true
}

int user_input(char msg[]) {
	char *end, buffer[100];
	long numInput;

	do {
		// prompt for user input
		printf("%s", msg);
		if (!fgets(buffer, sizeof buffer, stdin)) {
			break;
		}

		// remove the last character of new line
		buffer[strcspn(buffer, "\n")] = '\0';

		// convert string to integer
		numInput = strtol(buffer, &end, 10);

		// confirm the entire line was used in integer conversion
	} while (end != buffer + strlen(buffer));

	// return user input as integer
	return numInput;
}

void child_process() {
	// wait for kill command
	printf("PID = %d: Child running... \n", getpid());
	while (usr1Happened == 0) {
		sleep(1);
	}

	// set variable for parent process
	printf("PID = %d: Child received USR1. \n", getpid());

	printf("PID = %d: Child exiting. \n", getpid());
	exit(EXIT_SUCCESS);
}

void parent_process() {
	pid_t pid;
	int status = 0;

	// ignore kill signal
	signal(SIGUSR1, SIG_IGN);
	// wait for each child to finish
	while ((pid = wait(&status)) > 0) {
		sleep(1);
	}
}
