#include <string.h>
#include "constants.h"

struct {
	char *str;
	int len;
	enum builtin symbol;
} builtin_table[BNUM] = {
	{"cd", 2, CD},
	{"exit", 4, EXIT},
};

enum builtin builtin_check(char tok[], int len) {
	for (int i = 0; i < BNUM; i++)
		if (len == builtin_table[i].len && !strcmp(tok, builtin_table[i].str))
			return builtin_table[i].symbol;
	return -1; // this maxes out the enum ?
}
