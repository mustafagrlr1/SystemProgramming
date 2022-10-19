#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <error.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "become_daemon.h"
#include "arg_serverY.h"
#include "struct_file.c"
#include "tlpi_hdr.h"
#include <pthread.h>
#include <math.h>

#define SHARED_MEMORY "shared_memory.%ld"

#define SHARED_MEMORYZ "shared_memory_z.%ld"

#define SHARED_MEMORY_LEN (sizeof(SHARED_MEMORY)+20)

char clientFifo[CLIENT_FIFO_NAME_LEN];

/*producer consumer problem*/
#define N 1

int m,n; 


/*calculates the determinant of matrix*/
int determinant(int matrix[m][n]) {
    int row_size = m;
    int column_size = n;

    if (row_size != column_size) {
        exit(1);
    }

    else if (row_size == 1)
        return (matrix[0][0]);

    else if (row_size == 2)
        return (matrix[0][0]*matrix[1][1] - matrix[1][0]*matrix[0][1]);

    else {
        int minor[row_size-1][column_size-1];
        int row_minor, column_minor;
        int firstrow_columnindex;
        int sum = 0;

        register int row,column;
        for(firstrow_columnindex = 0; firstrow_columnindex < row_size;
                firstrow_columnindex++) {

            row_minor = 0;

            for(row = 1; row < row_size; row++) {

                column_minor = 0;

                for(column = 0; column < column_size; column++) {
                    if (column == firstrow_columnindex)
                        continue;
                    else
                        minor[row_minor][column_minor] = matrix[row][column];

                    column_minor++;
                }

                row_minor++;
            }

            m = row_minor;
            n = column_minor;

            if (firstrow_columnindex % 2 == 0)
                sum += matrix[0][firstrow_columnindex] * determinant(minor);
            else
                sum -= matrix[0][firstrow_columnindex] * determinant(minor);

        }

        return sum;

    }
}     

/*shared memory to keep flag servery and workers*/
typedef struct Process {
    pid_t id[1000];
    unsigned int flag[1000];
    int handled;
    int unhandled;
    int forwarded;
}Process;

/*serverz shared memory buffer*/
typedef struct Semap{
    int count;
    struct request req[100];
    int length_pool;
    pid_t busy_n[1000];
    int count_pid;
    int handled;
    int unhandled;
}Semap;

/*get the square root */
int square_reverse(int number){
    for(int i=1 ; i<number ; i++){
        if(i*i == number){
            return i;
        }
    }
    return 0;
}
                                                                                                                          
int main(int argc, char *argv[]){

    int shared_memory_fd;
    
    /*shared memory for servery to workesr*/
    char shared_memory_name[SHARED_MEMORY_LEN];
    
    char shared_memory_name_z[SHARED_MEMORY_LEN];/*shared memory name for serverZ TO workers*/
    char response[10000];
    
    
    /*prevent double instantiation*/
    sem_t* sem = sem_open("PDA2166", (O_CREAT | O_RDWR | O_EXCL), 0666, 0);
    if (sem == SEM_FAILED) {
        perror("Error: Duble Instantiation");
        sprintf(response, "Error: Duble Instantiation");
        write(logFd, response, strlen(response));
        exit(EXIT_FAILURE);
    }

    
    //become_daemon(); /*become daemon*/
    argument(argc, argv); /*gets arguments from command line*/
    
    int fds[poolSize+1][2];/*pipe for serverz and workers from serverY*/
    pid_t pid;
    pid_t poolProcess[poolSize];
    pid_t poolProcessZ[poolSize2];
    
    size_t size;
    
    
    logFd = open(pathToLogFile, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(logFd == -1){
        fprintf(stderr, "Unable to open file %s", pathToLogFile);
        exit(EXIT_FAILURE);
    }
    

    sprintf(response, "Server Y (%s, p=%d, t=%d) started\n", pathToLogFile, poolSize, _time);
    if(write(logFd,response ,strlen(response))==-1){
        perror("Error writing log ");
        sprintf(response, "Error writing log ");
        write(logFd, response, strlen(response));
        exit(EXIT_FAILURE);
    }

    sprintf(response, "Instantiated server Z\n");
    if(write(logFd,response ,strlen(response))==-1){
        perror("Error writing log ");
        sprintf(response, "Error writing log ");
        write(logFd, response, strlen(response));
        exit(EXIT_FAILURE);
    }

    /*first fork from serverY TO worker*/
    for(int i=0 ; i<poolSize+1 ; i++){
        pipe(fds[i]);
        
        
        pid = fork();
        
        
        if(pid == 0 && i!= poolSize){
                    struct response res;
                    struct request req;

                    if(close(fds[i][1]) == -1){
                        fprintf(stderr, "close hatasi");
                        sprintf(response, "close hatasi");
                        write(logFd, response, strlen(response));
                        exit(EXIT_FAILURE);
                    } 

                    Process* process;
                    
                    /*shared memory for flag */
                    snprintf(shared_memory_name, SHARED_MEMORY_LEN,
                                SHARED_MEMORY, (long)1);

                    shared_memory_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666);

                    if(shared_memory_fd == -1){
                        perror("fd error");
                        sprintf(response, "fd error");
                        write(logFd, response, strlen(response));
                        exit(EXIT_FAILURE);
                    } 
                    size = sizeof(Process);

                    process = (struct Process*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);

                    /*end of shared memory open*/

                    
                    int busy_n;

                    int invertible;
                    
                    int length;
                    int square;

                    
                    struct flock lock;
                    memset(&lock, 0, sizeof(lock));
                    int clientFd;
                    char response[10000];

                    /*for loop inside worker never ends*/
                    for(;;){


                        if(sigIntInterrupt){
                            int invertible = process->handled;
                            int notInvertible = process->unhandled;
                            int forwarded = process->forwarded;
                            int total = process->handled+process->unhandled;
                            lock.l_type = F_WRLCK;
                            if(fcntl(logFd, F_SETLKW, &lock)){
                                perror("error");
                                sprintf(response, "Error locking");
                                write(logFd, response, strlen(response));
                                return 1;
                            }
                            sprintf(response,"SIGINT received, terminating Z and exiting server Y. Total requests handled: %d, %d invertible, %d not. %d requests were forwarded.",total,invertible,notInvertible, forwarded);
                            
                            write(logFd, response, sizeof(response));
                            lock.l_type = F_UNLCK;

                            if(fcntl(logFd, F_SETLKW, &lock) == -1){
                                perror("error");
                                sprintf(response, "Error unlocking");
                                write(logFd, response, strlen(response));
                                return 1;
                            }
                            close(logFd);
                            kill(getpid(), SIGINT);
                            exit(EXIT_SUCCESS);
                        }

                        if(read(fds[i][0], &req, sizeof(struct request)) == -1){
                            perror("unable to read file");
                            sprintf(response, "unable to read file");
                            write(logFd, response, strlen(response));
                            exit(EXIT_FAILURE);
                        }
                        busy_n = req.busy_n;
                        
                        //mat req aldim ya response cevircem
                        length = square_reverse(req.seqLen);
                        int temp_matrix[length][length];
                        
                        for(int i=0 ; i<length ; i++){
                            for(int j=0 ; j<length ; j++){
                                temp_matrix[i][j] = req.message[(i*length)+j];
                            }
                        }
                        m = length;
                        n = length;
                        invertible = determinant(temp_matrix);
                        if(invertible == 0){
                            res.resValue = 0;
                        }else{
                            res.resValue = 1;
                        }
                        
                        /*hesaplama bitimi*/

                        square = square_reverse(req.seqLen);
                        lock.l_type = F_WRLCK;
                        if(fcntl(logFd, F_SETLKW, &lock)){
                            perror("error");
                            sprintf(response, "error");
                            write(logFd, response, strlen(response));
                            return 1;
                        }
                        sprintf(response,"Worker PID#%d is handling client PID#%d, matrix size %dx%d, pool busy %d/%d\n",getpid(),req.pid,square,square,busy_n,poolSize);


                        if(write(logFd,response ,strlen(response))==-1){
                            perror("Error writing log ");
                            sprintf(response, "Error writing log ");
                            write(logFd, response, strlen(response));
                            exit(EXIT_FAILURE);
                        }
                        lock.l_type = F_UNLCK;

                        if(fcntl(logFd, F_SETLKW, &lock) == -1){
                            perror("error");
                            sprintf(response, "Error locking");
                            write(logFd, response, strlen(response));
                            return 1;
                        }
                        
                        //sleep(_time);

                        sleep(_time);
                        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, 
                                    CLIENT_FIFO_TEMPLATE, (long) req.pid);
                        clientFd = open(clientFifo, O_WRONLY);
                        
                        if(clientFd == -1){
                            fprintf(stderr, "Error opening client file descriptor\n");
                            fflush(stderr);
                            sprintf(response, "Error opening client file descriptor");
                            write(logFd, response, strlen(response));
                            continue;
                        }
                        
                        if(write(clientFd, &res, sizeof(struct response)) != 
                                        sizeof(struct response)){
                                fprintf(stderr, "Error writing to FIFO %d\n", clientFd);
                                sprintf(response, "Error writing to FIFO %d\n", clientFd);
                                 write(logFd, response, strlen(response));
                                fflush(stderr);
                        }

                        
                        for(int i=0 ; i<poolSize ;i++){
                            if(process->id[i] == getpid()){
                                process->flag[i] = 0;
                            }
                        }  

                        lock.l_type = F_WRLCK;
                        if(fcntl(logFd, F_SETLKW, &lock)){
                            perror("error");
                            sprintf(response, "Error");
                            write(logFd, response, strlen(response));
                            return 1;
                        }
                        if(res.resValue == 1){
                            square = square_reverse(req.seqLen);
                            process->unhandled = process->unhandled + 1;
                            sprintf(response,"Worker PID#%d is responding to client PID#%d, the matrix is not invertibale\n",getpid(),req.pid);

                            if(write(logFd,response ,strlen(response))==-1){
                                perror("Error writing log ");
                                sprintf(response, "Error writing to llog ");
                                 write(logFd, response, strlen(response));
                                exit(EXIT_FAILURE);
                            }
                        }else{
                            square = square_reverse(req.seqLen);
                            process->handled = process->handled + 1;
                            sprintf(response,"Worker PID#%d is responding to client PID#%d, the matrix is invertibale\n",getpid(),req.pid);

                            if(write(logFd,response ,strlen(response))==-1){
                                perror("Error writing log ");
                                sprintf(response, "Error writing to llog ");
                                 write(logFd, response, strlen(response));
                                exit(EXIT_FAILURE);
                            }
                        }
                        lock.l_type = F_UNLCK;

                        if(fcntl(logFd, F_SETLKW, &lock) == -1){
                            perror("error");
                            sprintf(response, "Error");
                                 write(logFd, response, strlen(response));
                            return 1;
                        }
                    }

        }
        else if(pid == 0 && i == poolSize ){
            
            Semap* semap;
            snprintf(shared_memory_name_z, SHARED_MEMORY_LEN,
                SHARED_MEMORYZ, (long)1);
                
            
            shared_memory_fd = shm_open(shared_memory_name_z, O_CREAT | O_RDWR, 0666);

            if(shared_memory_fd == -1){
                perror("fd error");
                sprintf(response, "FD error");
                write(logFd, response, strlen(response));
                exit(EXIT_FAILURE);
            } 

            size = sizeof(struct Semap);

            if(ftruncate(shared_memory_fd, size) == -1){
                perror("truncate error");
                sprintf(response, "Error Truncate");
                write(logFd, response, strlen(response));
                exit(EXIT_FAILURE);
            }   


            semap = (struct Semap *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, shared_memory_fd, 0);
            
            
            
            sem_t* empty = mmap(NULL, sizeof(*empty), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            if (sem_init(empty, 1, 0) < 0)
            {
                perror("sem_init");
                sprintf(response, "Error sem init");
                write(logFd, response, strlen(response));
                exit(EXIT_FAILURE);
            }
            sem_t* mutex = mmap(NULL, sizeof(*mutex), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            if (sem_init(mutex, 1, 2) < 0)
            {
                perror("sem_init");
                sprintf(response, "Error sem_init");
                write(logFd, response, strlen(response));
                
                exit(EXIT_FAILURE);
            }
            
            

            /*fork from serverY to workers*/
            for(int j=0 ; j<poolSize2 ; j++){
                pid = fork();

                if(pid == 0){
                    
                    
                    struct response res;
                    struct request req;
                
                    
                    struct flock lock;
                    memset(&lock, 0, sizeof(lock));

                    char response[1000];
                    int length;
                    int busy_n;
                    
                    int clientFd;
                    
                    int invertible;            

                    semap->busy_n[semap->count_pid] = getpid();
                    semap->count_pid = semap->count_pid + 1;
                    /*reads from serverZ and response to client*/
                    for(;;){
                        
                        if(sigIntInterrupt){
                            int invertible = semap->handled;
                            int notInvertible = semap->unhandled;
                            int total = semap->handled+semap->unhandled;
                            lock.l_type = F_WRLCK;
                            if(fcntl(logFd, F_SETLKW, &lock)){
                                perror("error");
                                sprintf(response, "Error locking");
                                write(logFd, response, strlen(response));
                                return 1;
                            }
                            sprintf(response,"Z:SIGINT received, exiting server Z. Total requests handled %d, %d invertible, %d not",total,invertible,notInvertible);
                            
                            write(logFd, response, sizeof(response));
                            lock.l_type = F_UNLCK;

                            if(fcntl(logFd, F_SETLKW, &lock) == -1){
                                perror("error");
                                sprintf(response, "Error unlocking");
                                write(logFd, response, strlen(response));
                                return 1;
                            }
                            close(logFd);
                            sem_close(empty);
                            //sem_close(full);
                            sem_post(mutex);
                            sem_close(mutex);
                            exit(EXIT_SUCCESS);
                        }

                        sem_wait(empty);

                        req = semap->req[semap->count-1];
                        semap->count = semap->count - 1;
                        
                        
                        
                        
                        //mat req aldim ya response cevircem
                        length = square_reverse(req.seqLen);
                        int temp_matrix[length][length];
                        
                        for(int i=0 ; i<length ; i++){
                            for(int j=0 ; j<length ; j++){
                                temp_matrix[i][j] = req.message[(i*length)+j];
                            }
                        }
                        m = length;
                        n = length;

                        invertible = determinant(temp_matrix);
                        if(invertible == 0){
                            res.resValue = 0;
                        }else{
                            res.resValue = 1;
                        }
                        
                        /*hesaplama bitimi*/

                        lock.l_type = F_WRLCK;
                        if(fcntl(logFd, F_SETLKW, &lock)){
                            perror("error");
                            sprintf(response, "Error locking");
                            write(logFd, response, strlen(response));
                            return 1;
                        }
                        
                        for(int i=0 ; i<poolSize2 ; i++){
                            if(getpid() == semap->busy_n[i]){
                                busy_n = i+1;
                                
                            }
                        }
                        sprintf(response,"Z:Worker PID#%d is handling client PID#%d, matrix size %dx%d, pool busy %d/%d\n",getpid(),req.pid,length,length,busy_n,poolSize2);
                    
                        if(write(logFd,response ,strlen(response))==-1){
                            perror("Error writing log ");
                            sprintf(response, "Error writing to llog ");
                            write(logFd, response, strlen(response));
                            exit(EXIT_FAILURE);
                        }
                        lock.l_type = F_UNLCK;

                        if(fcntl(logFd, F_SETLKW, &lock) == -1){
                            perror("error");
                            sprintf(response, "Error unlocking");
                            write(logFd, response, strlen(response));
                            return 1;
                        }
                        
                        //sleep(_time);

                        /*writes to client with client pid*/
                        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, 
                                    CLIENT_FIFO_TEMPLATE, (long) req.pid);
                        clientFd = open(clientFifo, O_WRONLY);
                        
                        if(clientFd == -1){
                            perror("Error writing response");
                            sprintf(response, "Error writing to llog ");
                            write(logFd, response, strlen(response));
                            continue;
                        }

                        res.pid = getpid();

                        sleep(_time);
                        if(write(clientFd, &res, sizeof(struct response)) != 
                                        sizeof(struct response)){
                                fprintf(stderr, "Error writing to FIFO %d\n", clientFd);
                                fflush(stderr);
                                sprintf(response, "Error writing to FIFO %d\n", clientFd);
                                write(logFd, response, strlen(response));   
                                exit(EXIT_FAILURE);
                        }


                        lock.l_type = F_WRLCK;
                        if(fcntl(logFd, F_SETLKW, &lock)){
                            perror("error");
                            sprintf(response, "Error locking");
                            write(logFd, response, strlen(response)); 
                            return 1;
                        }
                        if(res.resValue == 1){
                            semap->unhandled = semap->unhandled+1;
                            sprintf(response,"Z:Worker PID#%d is responding to client PID#%d, the matrix is not invertible\n",getpid(),req.pid);

                            if(write(logFd,response ,strlen(response))==-1){
                                perror("Error writing log ");
                                sprintf(response, "Error writing log");
                                write(logFd, response, strlen(response));   
                                exit(EXIT_FAILURE);
                            }
                        }else{

                            semap->handled = semap->handled + 1;
                            sprintf(response,"Z:Worker PID#%d is responding to client PID#%d, the matrix is invertible\n",getpid(),req.pid);

                            if(write(logFd,response ,strlen(response))==-1){
                                perror("Error writing log ");
                                sprintf(response, "Error writing log");
                                write(logFd, response, strlen(response)); 
                                exit(EXIT_FAILURE);
                            }
                        }
                        lock.l_type = F_UNLCK;

                        if(fcntl(logFd, F_SETLKW, &lock) == -1){
                            perror("error");
                            sprintf(response, "Error locking");
                            write(logFd, response, strlen(response)); 
                            return 1;
                        }


                        

                        if(close(clientFd) == -1){
                            perror("Error close file");
                            sprintf(response, "Error close file");
                            write(logFd, response, strlen(response)); 
                            exit(EXIT_FAILURE);
                        }

                        sem_post(mutex);

                        
                    }
                }
                else{
                    
                }
            }   

            semap->handled=0;
            semap->unhandled=0;
            semap->length_pool = poolSize2;
            
            if(close(fds[i][1]) == -1){
                perror("Error close file descriptor serverZ");
                sprintf(response, "Error close file descriptor serverZ");
                write(logFd, response, strlen(response)); 
                exit(EXIT_FAILURE);
            } 
            
            struct sigaction sa_SIGINT;

            struct request req;

            
            

            memset(&sa_SIGINT, 0, sizeof(sa_SIGINT));
            sa_SIGINT.sa_handler = sigint_handler;
            sigaction(SIGINT, &sa_SIGINT, NULL);
            
            
            
            for(;;){

                if(sigIntInterrupt){
                    for(int i=0 ; i<poolSize+1 ;i++){
                        if(close(fds[i][0]) == -1){
                            perror("Error close file descriptor serverZ");
                            sprintf(response, "Error close file descriptor serverZ");
                            write(logFd, response, strlen(response)); 
                            exit(EXIT_FAILURE);
                        }
                    }
                        
                    sem_close(empty);
                    //sem_close(full);
                    sem_post(mutex);
                    sem_close(mutex);
                    for(int i=0 ; i<poolSize2; i++){
                        kill(poolProcessZ[i], SIGINT);
                    }
                    exit(EXIT_SUCCESS);
                }
                

                sem_wait(mutex);
                

                
                if(read(fds[poolSize][0], &req, sizeof(struct request)) != sizeof(struct request)){   
                        fprintf(stderr, "Error reading request; discardin\n");
                        fflush(stderr);
                        sprintf(response, "Error reading request; discardin\n");
                        write(logFd, response, strlen(response)); 
                        exit(EXIT_FAILURE);
                }

                semap->req[semap->count] = req;
                semap->count = semap->count + 1;

                sem_post(empty);
                

            }
                
        }else{
            poolProcess[i] = pid;  /*remember every pid of children*/ 
        }
    }
    
    /*pipe for serverY to serverZ*/
    

        if(close(fds[poolSize][0]) == -1){
            perror("Error close file descriptor serverZ");
            sprintf(response, "Error close file descriptor serverZ");
            write(logFd, response, strlen(response));
            exit(EXIT_FAILURE);
        } 

        
        Process* process ;
        struct sigaction sa_SIGINT;

        int serverFd, dummyFd;
        struct request req;

        memset(&sa_SIGINT, 0, sizeof(sa_SIGINT));
        sa_SIGINT.sa_handler = sigint_handler;
        sigaction(SIGINT, &sa_SIGINT, NULL);

        snprintf(shared_memory_name, SHARED_MEMORY_LEN,
            SHARED_MEMORY, (long)1);
        

        shared_memory_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666);

        if(shared_memory_fd == -1){
            perror("fd error");
            sprintf(response, "Fd Error");
            write(logFd, response, strlen(response));
            exit(EXIT_FAILURE);
        } 

        size = sizeof(Process);

        if(ftruncate(shared_memory_fd, size) == -1){
            perror("truncate error");
            sprintf(response, "Error Truncate");
            write(logFd, response, strlen(response));
            exit(EXIT_FAILURE);
        }

        process = (struct Process *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
        for(int i=0 ; i<poolSize ; i++){
            process->id[i] = poolProcess[i];
            process->flag[i] = 0;

        }

        process->handled=0;
        process->unhandled=0;
        process->forwarded=0;


        umask(0);
        if(mkfifo(pathToServerFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
                        && errno != EEXIST){
            fprintf(stderr, "Error mkfifo");
            sprintf(response, "Error mkfifo");
            write(logFd, response, strlen(response));
            exit(EXIT_FAILURE); 
        }
        serverFd = open(pathToServerFifo, O_RDONLY);

        if(serverFd == -1){
            fprintf(stderr, "unable open serverfd");
            sprintf(response, "Error unable open serverfd");
            write(logFd, response, strlen(response));
            exit(EXIT_FAILURE);
        }

        dummyFd = open(pathToServerFifo, O_WRONLY);
        if(dummyFd == -1){
            fprintf(stderr, "unable to open dummy file");
            sprintf(response, "Error unable to open dummy file");
            write(logFd, response, strlen(response));
            exit(EXIT_FAILURE);
        }
        for(;;){

            if(sigIntInterrupt){
                close(serverFd);
                close(dummyFd);

                for(int i=0 ; i<poolSize+1; i++){
                    kill(poolProcess[i], SIGINT);
                }

                exit(EXIT_SUCCESS);
            }
            if(read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)){   
                    fprintf(stderr, "Error reading request; discarding");
                    sprintf(response, "Error reading request; discarding");
                    write(logFd, response, strlen(response));
                    exit(EXIT_FAILURE);
            }
        
            for(int i=0 ; i<poolSize ;i++){
                if(process->flag[i] == 0){
                    process->flag[i] = 1;
                    req.busy_n = i+1;
                    
                    if(write(fds[i][1], &req, sizeof(struct request)) == -1){
                        perror("Error writing serverZ ");
                        sprintf(response, "Error writing serverZ ");
                        write(logFd, response, strlen(response));
                        exit(EXIT_FAILURE);
                    
                    }
                    
                    break;    
                }
                if(i == poolSize-1){
                    //zye girsin
                 
                    if(write(fds[poolSize][1], &req, sizeof(struct request)) == -1){
                        process->forwarded = process->forwarded + 1;
                        perror("z de yazma hatasi");
                        sprintf(response, "Error z yazma hatasi");
                        write(logFd, response, strlen(response));
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
            }
            
        }
    
    
    
    
    
    sem_close(sem);
    sem_unlink("PDA2166");
    return EXIT_SUCCESS;


}    

