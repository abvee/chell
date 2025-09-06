#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// TODO: something something header files, something something external linkage
#include "constants.h"
#include "parser.c"

int print(char *s);

char root_command[MLEN]; // buffer for root command
char args_buf[MARGS][MLEN]; // buffer arguments for command

int main(int argc, char *argv[]) {

	int exit_val = 0; // return value of the previous command

	char *args[MARGS] = {root_command, NULL};
	// set every pointer in args to the args_buf. 
	for (int i = 1; i < MARGS; i++)
		args[i] = (char *)(args_buf + i);
	// NOTE: this is basically a long way around doing malloc

	// TODO: don't output this when you're running a shell script
	print("> ");

	// stdin read loop
	while (lineread()) {
		// first token is the root token
		// TODO: This should reset when we find a pipe
		tok(root_command);

		switch (fork()) {
			case 0: // child
				// the rest of the arguments are pointers to args
				// build the arguments

				execlp(root_command, root_command, NULL); // TODO: replace with execv

				// it's an error if we get here
				fprintf(stderr, "Command not found\n");

			default:
				wait(&exit_val);
		}
		// reset the parser back to initial state, so new lines are not reading
		// from the end of old lines
		reset();

		// TODO: make the prompt a bit more dynamic, so it shows exit values
		// print("> ", ret_val);
		print("> ");
	}
	return 0;
}

// unbuffered print
int print(char *s) {
	return write(stdOut, s, strlen(s));
}
