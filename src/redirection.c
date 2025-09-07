#include <fcntl.h>
#include <stdlib.h>
#include "constants.h"

static int backup_fds[3];
void pipe_init() {
	backup_fds[stdIn] = dup(stdIn);
	backup_fds[stdOut] = dup(stdOut);
	backup_fds[stdErr] = dup(stdErr);
}

static int stdout_openfd;
static int stdin_openfd;

// make all fds original again
void pipe_reset() {
	int err;
	// stdout reset
	if (stdout_openfd) {
		close(stdout_openfd);
		stdout_openfd = 0;
		err = dup2(backup_fds[stdOut], stdOut);
		if (err < 0) {
			fprintf(stderr, "stdout: something went catastrophically wrong\n");
			exit(1);
		}
	}

	// stdin reset
	if (stdin_openfd) {
		close(stdin_openfd);
		stdin_openfd = 0;
		err = dup2(backup_fds[stdIn], stdIn);
		if (err < 0) {
			fprintf(stderr, "stdin: something went catastrophically wrong\n");
			exit(1);
		}
	}

}

// open files for redirection
void stdout_redir(const char* const file, bool append) {
	if (stdout_openfd) close(stdout_openfd);

	stdout_openfd = open(file, O_WRONLY|O_CREAT|(append?O_APPEND:O_TRUNC), fMode);
	if (stdout_openfd < 0) return;

	dup2(stdout_openfd, stdOut);
}

void stdin_redir(const char* const file) {
	if (stdin_openfd) close(stdin_openfd);

	stdin_openfd = open(file, O_RDONLY);

	if (stdin_openfd < 0) {
		stdin_openfd = 0;
		fprintf(stderr, "file not found\n");
		return;
	}

	dup2(stdin_openfd, stdIn);
}
