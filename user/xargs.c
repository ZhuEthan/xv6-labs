#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


void strtok(char** iargv, char* buf, char c, int n) {
	char* end = buf+n;
	char* cur = buf;
	char* prev = cur;
	int len = 0;
	int i = 1;

	while (iargv[i] != 0) {
		i += 1;
	}

	while (cur < end) {
		//cur = strchr(prev, c);
		while (cur < end) {
			if (*cur == ' ' || *cur == '\n') {
				break;
			}
			cur += 1;
		}
		len = cur-prev;
		iargv[i] = (char*) malloc(sizeof(char) * (len+1));
		strcpy(iargv[i], prev);
		iargv[i][len] = '\0';
		prev = cur+1;
		cur = prev;
		i++;
	}
	iargv[i] = 0;
}

char** read_line(char** iargv, int fd) {
	char buf[512];
	char** res = 0;
	int n = 0;
	while ((n = read(fd, buf, 128)) > 0) {
		strtok(iargv, buf, ' ', n);
	}

	return res;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(2, "xargs command ...\n");
	}
	
	char* iargv[MAXARG]; 

	for (int i = 1; argv[i] != 0; i++) {
		//printf("parsing: %s\n", iargv[i]);
		iargv[i-1] = (char*) malloc(strlen(argv[i])+1);
		strcpy(iargv[i-1], argv[i]);
		iargv[i-1][strlen(argv[i])] = '\0';
	}

	read_line(iargv, 0);

	if (fork() == 0) {
		//printf("iargv[0]: %s, length is %d\n", iargv[0], strlen(iargv[0]));
		//printf("iargv[1]: %s, length is %d\n", iargv[1], strlen(iargv[1]));
		//printf("argv[2]: %s\n", iargv[2]);
		if (exec(iargv[0], iargv) != -1) {
			printf("something wrong\n");
		}
	} 

	wait(0);
	exit(0);
}
