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
			return true;
	}
	return false;
}


// NOTE: we don't handle newlines here. Assume line doesn't have a new line
int tok(char *buf) {
	static char holder[2];

	// if we have a holder for a break token, copy it to buffer and return it
	if (holder[0]) {
		buf[0] = holder[0];
		holder[0] = '\0';

		if (holder[1]) {
			buf[1] = holder[1];
			holder[1] = '\0';
			buf[2] = '\0';
			return 2;
		}
		buf[1] = '\0';
		return 1;
	}

	// rid of whitespace
	while (line[li] && line[li] == ' ' || line[li] == '\t')
		li++;

	// copy token
	int bufi = 0;
	while (line[li] && line[li] != ' ' && line[li] != '\t' && !breaker(line[li]))
		buf[bufi++] = line[li++];
	buf[bufi] = '\0';

	// if we have a breaker token, and we don't have anything in buffer yet,
	// copy into buffer
	// otherwise copy into temp holder buffer, which is handled above
	if (breaker(line[li])) {
		if (bufi) {
			holder[0] = line[li++];
			if (holder[0] == '>' && line[li] == '>') {
				holder[1] = '>';
				li++;
			}
		}
		else {
			buf[0] = line[li++];
			if (buf[0] == '>' && line[li] == '>') {
				buf[1] = '>';
				buf[2] = '\0';
				li++;
				return 2;
			}
			buf[1] = '\0';
			return 1;
		}
	}
	return bufi;
}

void parser_reset() {
	li = 0; 
}

// debug
void _printline() {
	printf("%s|%d\n", line, li);
}
