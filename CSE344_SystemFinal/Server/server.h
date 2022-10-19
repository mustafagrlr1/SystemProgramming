#ifndef _SERVER_H_
#define _SERVER_H_
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "../Helper/helper.h"

volatile sig_atomic_t sigtermInterrupt = 0;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		sigtermInterrupt = 1;
}

typedef struct ARGUMENT{
    char* PORT;
    int Number_of_Threads;
}ARGUMENT;

typedef struct PORTSERVANT{
    int begin;
    int end;
    int port_number;
    char dataset[100];
    char IP[100];
}PORTSERVANT;



#endif