#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int parent_to_child_pipe[2];
int child_to_parent_pipe[2];

int main(int argc, char** argv) {
	pipe(child_to_parent_pipe);
	pipe(parent_to_child_pipe);

	if (fork() == 0) {
		//child
		char buf[10];
		close(parent_to_child_pipe[1]);
		int n = read(parent_to_child_pipe[0], buf, 10);
		close(parent_to_child_pipe[0]);

		if (n) {
			printf("%d: received ping\n", getpid());
			close(child_to_parent_pipe[0]);
			write(child_to_parent_pipe[1], buf, 10);
			close(child_to_parent_pipe[1]);
		}
		exit(0);
	} else {
		//parent
		char buf[10];

		close(parent_to_child_pipe[0]);
		write(parent_to_child_pipe[1], "any bytes", 10);
		close(parent_to_child_pipe[1]);

		close(child_to_parent_pipe[1]);
		int n = read(child_to_parent_pipe[0], buf, 10);
		close(child_to_parent_pipe[0]);
		
		if (n) {
			printf("%d: received pong\n", getpid());
		}
	}

	exit(0);
}
