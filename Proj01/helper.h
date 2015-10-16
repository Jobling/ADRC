#ifndef __HELPER_H
#define __HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MACROS
#define BITSIZE 8
#define BUFFSIZE 32

#define ADD 0
#define DEL 1
#define PRINT 2
#define TWOTREE 3
#define LOOKUP 4
#define HELP 5
#define EXIT 6

void PrintInterface();
int getCMD(char * command);
int isBinary(char * prefix);

#endif

