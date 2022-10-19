#ifndef _HELPER_H_
#define _HELPER_H_
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


typedef struct RealEstateData{
    char transaction_id[100];
    char real_estate[100];
    int day_start;
    int month_start;
    int year_start;
    int day_end;
    int month_end;
    int year_end;
    char cityname[100];
    int socket_fd;
    int pid;
    int choice;
    int begin;
    int end;

    int port;
    char ip[100];
    char dataset[100];
    char servant_pid;
}RealEstateData;






#endif