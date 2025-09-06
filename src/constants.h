#ifndef CONSTANTS
enum {stdIn, stdOut, stdErr}; // fds

// Line length, max argument length
enum {MLEN = 1024, MARGS = 64};

const char *prompt_str_before = "\x1b[38;2;235;107;141m<";
const char *prompt_str_after = ">\x1b[0m ";
#define CONSTANTS 
#endif
