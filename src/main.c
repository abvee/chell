#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// TODO: something something header files, something something external linkage
#include "constants.h"
#include "parser.c"

int prompt(int exit_value, const char* const s);
void pipe_init();
void pipe_reset();

char root_command[MLEN]; // buffer for root command
char args_buf[MARGS][MLEN]; // buffer arguments for command

int main(int argc, char *argv[]) {

	int exit_val = 0; // return value of the previous command
	char *args[MARGS] = {root_command, NULL};
	// set every pointer in args to the args_buf. 
	for (int i = 1; i < MARGS; i++)
		args[i] = (char *)(args_buf + i);
	// NOTE: this is basically a long way around doing malloc

	// initialise pipe variables
	pipe_init();

	// TODO: don't output this when you're running a shell script
	prompt(0, prompt_str);

	char token[MLEN] = {0}; // general purpose token
	// stdin read loop
	while (true) {

		lineread();
		// first token has to be a command ?
		// TODO: This should reset when we find a pipe
		tok(root_command);
		int toki = 1; // token counter.

		// copy arguments
		for (; toki < MARGS && tok(args[toki]); toki++) {
			// check if the token 
		}
		switch (fork()) {
			case -1:
				fprintf(stderr, "fork failed, abort shell\n");
				return 1;

			case 0: // child

				// root_command already pointed in args[0]
				// explicitly make NULL
				args[toki] = NULL;
				
				int err = execvp(root_command, args);

				// it's an error if we get here
				fprintf(stderr, "Command not found\n");
				// return to stop forking
				return -err;

			default: // parent
				wait(&exit_val);
				// we now reset the null value in arguments
				args[toki] = (char *)args_buf[toki];
		}
		// reset the parser back to initial state, so new lines are not reading
		// from the end of old lines
		reset();

		// TODO: make the prompt a bit more dynamic, so it shows exit values
		prompt(exit_val, prompt_str);
	}
	return 0;
}

// unbuffered print
inline int prompt(int exit_val, const char* const s) {
	char buf[MLEN] = {0};
	int i = 0;

	// exit value of 0 is success, so don't parse 0
	if (exit_val) {

		while (exit_val % 10 != 0) {
			buf[i++] = exit_val % 10 + '0';
			exit_val /= 10;
		}

		int tmp;
		// reverse the array
		for (int k = 0; k < i / 2; k++) {
			tmp = buf[k];
			buf[k] = buf[i - k - 1];
			buf[i - k - 1] = tmp;
		}
		buf[i++] = ' ';
	}

	// copy s
	int j = 0;
	while (buf[i++] = s[j++]);

	return write(stdOut, buf, i);
}

static int backup_fds[3];
static int stdout_pipe[2];
static int stderr_pipe[2];

void pipe_init() {
	// I don't know if the compiler will expand this if it's for loop, so I'm
	// not going to use one

	backup_fds[stdIn] = dup(stdIn);
	backup_fds[stdOut] = dup(stdOut);
	backup_fds[stdErr] = dup(stdErr);

	// also make stdout pipe and stderr pipe
	pipe(stdout_pipe);
	pipe(stderr_pipe);
}

// make all fds original again
void pipe_reset() {
	dup2(backup_fds[stdIn], stdIn);
	dup2(backup_fds[stdOut], stdOut);
	dup2(backup_fds[stdErr], stdErr);
}
