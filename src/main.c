#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int lineread();
int print(char *s);
int tok(char const *line, char *buf);

enum {stdIn, stdOut, stdErr}; // fds
enum {MLEN = 1024}; // max length of a line

static char line[MLEN];

int main(int argc, char *argv[]) {
	// TODO: don't output this when you're running a shell script
	print("> ");

	char root_command[1024] = {};
	int exit_val = 0; // return value of the previous command

	// stdin read loop
	while (lineread()) {
		// first token is the root token
		// TODO: This should reset when we find a pipe
		tok(line, root_command);

		switch (fork()) {
			case 0: // child
				execlp(root_command, root_command, NULL); // TODO: replace with execv
			default:
				wait(&exit_val);
		}
		// TODO: make the prompt a bit more dynamic, so it shows exit values
		// print("> ", ret_val);
		print("> ");
	}
	return 0;
}

// read into line, return number of characters read
int lineread() {
	int n = read(stdIn, line, MLEN);
	// bounds checking
	if (n >= MLEN)
		return 0;
	line[n-1] = '\0'; // strip newline
	return n-1; // return last index, not length
}

// unbuffered print
int print(char *s) {
	return write(stdOut, s, strlen(s));
}

// return length of buffer. Do the checks for line and buffer length before you
// enter this function

// NOTE: we don't handle newlines here. Assume line doesn't have a new line
int tok(char const *line, char *buf) {
	int li = 0, bufi = 0; // indexes
	while (line[li] == ' ' || line[li]  == '\t' && line[li])
		li++;

	while ((buf[bufi] = line[li]) != '\t' && buf[bufi] != ' ' && buf[bufi] != '\0')
		bufi++, li++;

	buf[bufi] = '\0'; // account for the whitespace
	return bufi;
}
