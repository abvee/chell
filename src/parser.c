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

// NOTE: we don't handle newlines here. Assume line doesn't have a new line
int tok(char *buf) {
	int bufi = 0; // buffer index
	while (line[li] && (line[li] == ' ' || line[li]  == '\t'))
		li++;

	while ((buf[bufi] = line[li]) != '\t' && buf[bufi] != ' ' && buf[bufi] != '\0')
		bufi++, li++;

	buf[bufi] = '\0'; // account for the whitespace
	return bufi;
}
// TODO: do something else with this tokenizer that does better than whatever
// this thing is. We're just returning words, so `ls>file` doesn't work, it has
// to be `ls > file`

void parser_reset() {
	li = 0; 
}

// debug
void _printline() {
	printf("%s|%d\n", line, li);
}
