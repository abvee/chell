#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// TODO: something something header files, something something external linkage
#include "constants.h"
#include "parser.c"
#include "redirection.c"
#include "builtins.c"
#include "debug.h"

int prompt(int exit_value);

char root_command[MLEN]; // buffer for root command
char args_buf[MARGS][MLEN]; // buffer arguments for command

int main(int argc, char *argv[]) {
	if (argc > 1) debugf = true; // any arg passed will turn on debug

	debug("Debug mode enabled\n");

	int exit_val = 0; // return value of the previous command
	char *args[MARGS] = {root_command, NULL};
	// set every pointer in args to the args_buf.
	for (int i = 1; i < MARGS; i++)
		args[i] = (char *)(args_buf + i);
	// NOTE: this is basically a long way around doing malloc

	redir_init();
	bool next_pipe = false, prev_pipe = false;
	// next pipe tells us if there's a new pipe
	// prev_pipe tells us if we're already in a pipe

	// TODO: don't output this when you're running a shell script
	prompt(0);

	char token[MLEN] = {0}; // general purpose token
	// stdin read loop
	while (true) {

		// if we're in a pipe, don't read a new line
		// set the read end
		if (prev_pipe) {
			debug("Set stdin to pipe\n");
			pipe_read();
		}
		else {
			while (!lineread()) // hitting enter just resets here
				prompt(0);
			debug("Read new line\n");
			next_pipe = prev_pipe = false;
		}


		// first token has to be a command
		int root_len = tok(root_command);
		debug("Found root command: %s\n", root_command);

		// NOTE: For builtin checking right now we tokenize first then recompare
		// the length Complexity is thus O(2n). If the parser can identify the
		// type of the argument beforehand (builtin, command, redirection, etc),
		// we can reduce this to O(n).

		// TODO: Implement ^ in the parser ?
		// This risks polluting the tokenizer with project specific semantics
		// instead of being somewhat portable.

		// builtin check
		switch (builtin_check(root_command, root_len)) {
			case -1: break; // do nothing
			case CD:
				debug("Found cd command\n");
				tok(token);
				chdir(token);
				// Note: we should check if there are multiple arguments to cd
				parser_reset();
				// this is not good. It doesn't allow for pipes or anything like
				// that. One could say that we don't need that anyways.... which is
				// a fair and valid argument, because you can't cd with a pipe. But
				// what about &&. Surely you would need that
				prompt(exit_val);
				continue;
			case EXIT: return 0;
		}


		int toki = 1; // token counter.
		next_pipe = false; // new command, new pipe

		// copy arguments for execv
		for (; !next_pipe && toki < MARGS && tok(args[toki]); toki++) {
			// check if the token is redirection
			switch (args[toki][0]) {
				case '>':
					tok(token); // next token is file name

					if (args[toki][1] == '>') stdout_redir(token, true);
					else stdout_redir(token, false);

					toki--; // don't include redirection
					break;
				case '<':
					tok(token);
					stdin_redir(token);
					toki--;
					break;
				case '|':
					toki--;
					next_pipe = true;
					pipe_create(); // create a pipe, set stdout
					break;
			}
		}
		args[toki] = NULL;

		switch (fork()) {
			case -1:
				fprintf(stderr, "fork failed, abort shell\n");
				return 1;

			case 0: // child
				int err = execvp(root_command, args);

				// it's an error if we get here
				fprintf(stderr, "Command not found\n");
				// return to stop forking
				return -err;

			default:
				wait(&exit_val);
				// TODO: close the write end of the pipe, if there is one

				// we now reset the null value in arguments
				args[toki] = (char *)args_buf[toki];
		}

		// what should this function do ?
		// It needs to close the write end of the current pipe
		// If there is a previous pipe, we close the read end of it as well no?
		redir_reset(prev_pipe, next_pipe);
		prev_pipe = next_pipe;

		if (!next_pipe) {
			parser_reset();
			prompt(exit_val);
		}
	}
	return 0;
}

// unbuffered print
inline int prompt(int exit_val) {
	char buf[MLEN] = {0};
	int i = 0;

	// TODO: hardcode the initial prompt
	// copy the before prompt
	for (int p = 0; buf[i] = prompt_str_before[p]; p++)
		i++;

	// exit value of 0 is success, so don't parse 0
	if (exit_val) {

		char num[4] = {0}; // max number can be 3 digits
		int numi = 0;

		for (; exit_val % 10 != 0; numi++) {
			num[numi] = exit_val % 10 + '0';
			exit_val /= 10;
		}

		int tmp;
		// reverse the array
		for (int k = 0; k < numi / 2; k++) {
			tmp = num[k];
			num[k] = num[numi - k - 1];
			num[numi - k - 1] = tmp;
		}

		// copy return value
		for (int p = 0; buf[i] = num[p]; p++)
			i++;
	}

	// copy the after prompt
	for (int p = 0; buf[i] = prompt_str_after[p]; p++)
		i++;

	return write(stdOut, buf, i);
}
