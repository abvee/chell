#include "constants.h"
#include <unistd.h>

static char line[MLEN];
static int li; // line index

// read into line, return number of characters read
int lineread() {
	int n = read(stdIn, line, MLEN);
	// bounds checking
	if (n >= MLEN)
		return 0;

	line[n-1] = '\0'; // strip newline
	printf("line: %s\n", line);
	return n-1; // return last index, not length
}
// return length of buffer. Do the checks for line and buffer length before you
// enter this function

// return true if it's a breaker
// does not handle whitespace
static inline bool
breaker(char c) {
	switch (c) {
		case '|':
		case '>':
		case '<':
		case '\0':
			return true;
	}
	return false;
}


// NOTE: we don't handle newlines here. Assume line doesn't have a new line
int tok(char *buf) {

	// rid of whitespace
	while (line[li] && line[li] == ' ' || line[li] == '\t')
		li++;

	// copy token
	int bufi = 0;
	while (line[li] && line[li] != ' ' && line[li] != '\t') {
		buf[bufi++] = line[li++];
	}
	buf[bufi] = '\0';
	return bufi;
}

void parser_reset() {
	li = 0; 
}

// debug
void _printline() {
	printf("%s|%d\n", line, li);
}
