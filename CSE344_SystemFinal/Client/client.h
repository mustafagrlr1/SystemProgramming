#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <dirent.h>
#include <fcntl.h>

#include <signal.h>
#include "../Helper/helper.h"


volatile sig_atomic_t sigtermInterrupt = 0;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		sigtermInterrupt = 1;
}

typedef struct ARGUMENT{
    char* requestFile;
    char* PORT;
    char* IP;
}ARGUMENT;

#endif