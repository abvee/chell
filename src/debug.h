#ifndef CONSTANTS
bool debugf = false; // gobal boolean flag for debug
#define debug(expr, ...) if (debugf) {\
	printf("\033[1;31mDEBUG:\033[0m " expr, ##__VA_ARGS__);\
	fflush(stdout);\
}
#define CONSTANTS 
#endif
