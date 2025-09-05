#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int lineread();
int print(char *s);

enum {stdIn, stdOut, stdErr}; // fds
enum {MLEN = 1024}; // max length of a line

static char line[MLEN];

int main(int argc, char *argv[]) {

	// stdin read loop
	print("> ");
	while (lineread()) {
		printf("%s\n", line);
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
