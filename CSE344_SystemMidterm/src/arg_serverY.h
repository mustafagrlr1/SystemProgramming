#ifndef _ARG_SERVER_H
#define _ARG_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


char *pathToServerFifo;
char *pathToLogFile;
int poolSize;
int poolSize2;
int _time;

void argument(int, char*[]);

#endif