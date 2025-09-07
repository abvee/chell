#include <fcntl.h>
#include "constants.h"

static int backup_fds[3];
void pipe_init() {
	backup_fds[stdIn] = dup(stdIn);
	backup_fds[stdOut] = dup(stdOut);
	backup_fds[stdErr] = dup(stdErr);
}

static int stdout_openfd;

// make all fds original again
void pipe_reset() {
	// stdout reset
	if (stdout_openfd) close(stdout_openfd);
	stdout_openfd = 0;
	dup2(backup_fds[stdOut], stdOut);
}

// open files for redirection
void stdout_redir(const char* const file, bool append) {
	if (stdout_openfd) close(stdout_openfd);

	stdout_openfd = open(file, O_WRONLY|O_CREAT|(append?O_APPEND:O_TRUNC), fMode);
	if (stdout_openfd < 0) return;

	dup2(stdout_openfd, stdOut);
}
