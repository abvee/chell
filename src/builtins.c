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
	int i = 0;
	// breaks when builtin matches
	for (;i < BNUM && (len != builtin_table[i].len || strcmp(tok, builtin_table[i].str)); i++)
		;
	return i >= BNUM?-1:builtin_table[i].symbol;
}
