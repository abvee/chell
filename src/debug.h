#ifndef DEBUG

bool debugf = false; // gobal boolean flag for debug

#define debug(expr, ...) if (debugf)\
	fprintf(stderr, "\033[1;31mDEBUG:\033[0m " expr, ##__VA_ARGS__)

#define DEBUG
#endif
