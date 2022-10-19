#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <signal.h>
#include <string.h>

#include "struct_file.c"

volatile sig_atomic_t sigIntInterrupt = 0;
   

void sigint_handler(int signo) {
    if (signo == SIGINT)
        sigIntInterrupt = 1;
}

static char clientFifo[CLIENT_FIFO_NAME_LEN];/*client fifo name that connects server*/


 /* remove fifo after finishing the job*/
static void removeFifo(void)                              
{
    unlink(clientFifo);
}

/*get square of the number*/
int power(int number, int powr){                            
    int num = 1;
    for(int i=0 ; i<powr;i++){
        num *= number;
    }
    return num;
}

/*get the next item in the buffer*/
int calculate(unsigned char buf[], int start, int end){     
    
    int len = end - start - 1;
    int number = 0;
    for(int i=0 ; i< end-start ; i++){;
        number += ((int)buf[start+i]-48) * power(10,len);
        len--;
    }
    return number;
}    

int square_reverse(int number){
    for(int i=1 ; i<number ; i++){
        if(i*i == number){
            return i;
        }
    }
    return 0;
}


int main(int argc, char *argv[]){

    
    /*time to start client*/
    time_t start_time = time(NULL);                             


    int _s_ = 0;
    int _o_ = 0;
    int c;
    char *pathToServerFifo;
    char *pathToDataFile;

     /*simple get opt function to get all argument*/
    while((c = getopt(argc, argv, "s:o:")) != -1){             
        switch (c)
        {
        case 's':
            pathToServerFifo = optarg;
            _s_ = 1;
            
            break;
        
        case 'o':
            pathToDataFile = optarg;
            _o_ = 1;
            break;

        default:
            break;
        }
    }

    if((!_s_) || (!_o_) ){
        fprintf(stderr, "invalid command argument!! %s", *argv);
    }    

    struct sigaction sa_SIGINT;
    memset(&sa_SIGINT, 0, sizeof(sa_SIGINT));
    sa_SIGINT.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa_SIGINT, NULL);
    
    struct request req;
    struct response res;


    /*read csv from the file and add all the number to buffer*/
    unsigned char buf[1000000] = {""};
    
    /*Read csv file*/
    size_t bytes_read;
    int readFd = open(pathToDataFile, O_RDONLY);
    int count=0;
    int start=0;
    int length = 0;
    do{
        bytes_read = read(readFd, buf, sizeof(buf));

        for(int i=0 ; i<bytes_read ; i++){
            
            if(buf[i] == ',' || buf[i] == '\r' || buf[i] == '\n' || buf[i] == -1){
                req.message[length] = calculate(buf, start, count);
                start = count+1;
                length++;
                if(sigIntInterrupt){
                    exit(EXIT_SUCCESS);
                }
            }
            count++;
        }        
    }while(bytes_read == sizeof(buf));
    req.message[length] = calculate(buf,start,count);
    length++;
    
    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }
    if(square_reverse(length) == 0){
        if(sigIntInterrupt){
            exit(EXIT_SUCCESS);
        }
        perror("Not a square matrix");
        exit(EXIT_FAILURE);
    }

    /*end of reading csv*/
    req.seqLen = length;
    req.pid = getpid();
    
    int serverFd; /*open in write mode*/
    int clientFd;

    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }
    /*make fifo with client fifo named to connect server*/
    umask(0);
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN,
                        CLIENT_FIFO_TEMPLATE, (long)getpid());
    
    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }
    if(mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 
                && errno != EEXIST){
            fprintf(stderr, "Unable to open fifo\n");
            exit(EXIT_FAILURE);
    }
    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }

    /*run remove after terminating*/       
    if(atexit(removeFifo) != 0){
        fprintf(stderr, "Unable to atexit\n");
        exit(EXIT_FAILURE);
    }    

    /*open server fifo and write request*/
    serverFd = open(pathToServerFifo, O_WRONLY);
    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }
    if(serverFd == -1){
        fprintf(stderr, "Unable to open serverFd\n");
        exit(EXIT_FAILURE);
    }
    if(write(serverFd, &req, sizeof(struct request)) != 
                    sizeof(struct request)){
        if(sigIntInterrupt){
            exit(EXIT_SUCCESS);
        }
        fprintf(stderr, "Unable to open write\n");
        exit(EXIT_FAILURE);
    }


    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }

    /*client fd gets the response from the server*/
    clientFd = open(clientFifo, O_RDONLY);
    if(clientFd == -1){
        fprintf(stderr, "unable to open clientfd\n");
    }

    if(read(clientFd, &res, sizeof(struct response))
                != sizeof(struct response))
        fprintf(stderr, "unalbe to open read\n");

    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }
    /*calculate the time*/
    time_t curr_time = time(NULL) - start_time;
    fprintf(stdout, "Client output      %d\n", res.resValue);

    if(res.resValue == 1){
        fprintf(stdout, "Client PID#%d: the matrix is invertible, total time %ld seconds, goodbye.\n", getpid(), curr_time);
    }else{
        fprintf(stdout, "Client PID#%d: the matrix is not invertible, total time %ld seconds, goodbye.\n", getpid(), curr_time);
    }    

    if(sigIntInterrupt){
        exit(EXIT_SUCCESS);
    }
    
    
    exit(EXIT_SUCCESS);
}
