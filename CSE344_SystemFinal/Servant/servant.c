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

#include "servantLib.h"
#include "lib.h"





int main(int argv, char* argc[]){

    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

    int _d_ = 0, _c_ = 0, _r_ = 0, _p_ = 0;
    int c;

    ARGUMENT argument;


    while((c = getopt(argv, argc, "d:c:r:p:")) != -1){
      switch(c)
      {
        case 'd':
            argument.directoryPath = optarg;
            _d_ = 1;
            break;
        case 'c':
            argument.city_range = optarg;
            _c_ = 1;
            break;
        case 'r':
            argument.IP = optarg;
            _r_ = 1;
            break;
        case 'p':
            argument.PORT = optarg;
            _p_ = 1;
            break;
        default:
            break;
      }
    }
    if((!_d_) || (!_c_) || (!_r_)|| (!_p_)){
      perror("No input File");
      exit(EXIT_FAILURE);
   }
    
    read_directory_path(argument);

    return 0;
}