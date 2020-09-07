#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}


void pre_order_find(char* target, char* cur_path) {
	char path[256];
	int fd;
	struct stat st;
	struct dirent de;

	strcpy(path, cur_path);

	if ((fd = open(cur_path, 0)) < 0) {
		fprintf(2, "path %s cannot be found\n", cur_path);
		return;
	}
	
	if (fstat(fd, &st) < 0) {
		fprintf(2, "find, cannot stat %s\n", cur_path);
		close(fd);
		return;
	}
	
	switch(st.type) {
		case T_FILE:
			if (!strcmp(fmtname(path), target)) {
				printf("%s\n", path);
			} 
			break;
		case T_DIR:
			while (read(fd, &de, sizeof(de)) == sizeof(de)) {
				if (de.inum == 0) continue;
				if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
					continue;
				}
				int o_len = strlen(path);
				path[o_len] = '/';
				memmove(path+o_len+1, de.name, 256-strlen(path));
				*(path + o_len + 1 + strlen(de.name)) = '\0';
				pre_order_find(target, path);
				*(path + o_len) = '\0';
			}
			break;
	}

	close(fd);
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(2, "find util should be: find [dir] target_file");
		exit(1);
	}
	pre_order_find(argv[2], argv[1]);
	exit(0);
}
