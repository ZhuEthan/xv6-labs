#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void swap_prev(int*prev_pipe, int* next_pipe) {
	prev_pipe[0] = next_pipe[0];
	prev_pipe[1] = next_pipe[1];
}

void sieve(int* prev_pipe, int* next_pipe) {
	int target;
	int n = read(prev_pipe[0], &target, 4);
	if (n) {
		printf("prime %d\n", target);
	} else {
		if (close(prev_pipe[0]) == -1) {
			printf("close failed on line %d\n", __LINE__);
		}
		if (close(next_pipe[1]) == -1) {
			printf("close failed on line %d\n", __LINE__);
		}
		if (close(next_pipe[0]) == -1) {
			printf("close failed on line %d\n", __LINE__);
		}
		exit(0);
	}

	int next;
	while ((n = read(prev_pipe[0], &next, 4)) != 0) {
		if (next % target != 0) {
			write(next_pipe[1], &next, 4);
		}
	}
	if (close(prev_pipe[0]) == -1) {
		printf("close failed on line %d\n", __LINE__);
	}
	if (close(next_pipe[1]) == -1) {
		printf("close failed on line %d\n", __LINE__);
	}

	if (fork() == 0) {
		//child
		swap_prev(prev_pipe, next_pipe);
		pipe(next_pipe);
		sieve(prev_pipe, next_pipe);
	} 
	//parent
	if (close(next_pipe[0]) == -1) {
		printf("close failed on line %d\n", __LINE__);
	}


	int status = 0;
	int pid;
	while ((pid = wait(&status)) > 0);
		//printf("pid %d is reaped\n", pid);
	exit(0);
}


int main(int argc, char** argv) {
	int prev_pipe[2];
	int next_pipe[2];

	pipe(prev_pipe);
	pipe(next_pipe);
	for (int i = 2; i < 35; i++) {
		write(prev_pipe[1], &i, 4);
	}

	if (close(prev_pipe[1]) == -1) {
		printf("close failed on line %d\n", __LINE__);
	}
	sieve(prev_pipe, next_pipe);

	exit(0);
}

