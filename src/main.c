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
		int toki = 1; // token counter

		switch (fork()) {
			case -1: // this should never happen
				fprintf(stderr, "fork failed, abort shell\n");
				return 1;

			case 0: // child
				// the rest of the arguments are pointers to args
				// build the arguments

				// root_command already pointed in args[0]
				for (; toki < MARGS && tok(args[toki]); toki++);
				// explicitly make NULL
				args[toki] = NULL;
				
				int err = execvp(root_command, args);

				// it's an error if we get here
				fprintf(stderr, "Command not found\n");
				fflush(stderr);
				// return to stop forking
				return err;

			default: // parent
				wait(&exit_val);
				// we now reset the null value in arguments
				args[toki] = (char *)args_buf[toki];
		}
		// reset the parser back to initial state, so new lines are not reading
		// from the end of old lines
		reset();

		// TODO: make the prompt a bit more dynamic, so it shows exit values
		print("> ");
	}
	return 0;
}

// unbuffered print
int print(char *s) {
	return write(stdOut, s, strlen(s));
}
