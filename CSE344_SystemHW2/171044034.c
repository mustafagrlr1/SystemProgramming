#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include "helpers.c"

sig_atomic_t signal_arrived = 0;

void handler(){
    signal_arrived = 1;
}

int check_output_file(char* argv[], int argc);
int check_input_file(char* argv[], int argc);

int main(int argc, char* argv[]){

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    if(argc != 5){
		perror("invalid argument number");
		return 1;
	}
    unsigned char buffer[30];
    size_t bytes_read;

    char*** dimensional_coordineates = (char***) malloc(1*sizeof(dimensional_coordineates));

    int input_index = check_input_file(argv, argc);

    int fd = open(argv[input_index], O_RDONLY);
    fprintf(stdout, "Process P reading %s\n", argv[input_index]);
    
    int r_ith = 0;
    do{ 
        bytes_read = read(fd, buffer, sizeof(buffer));
        if(bytes_read != 30){
            break;
        }
        dimensional_coordineates = (char***)realloc(dimensional_coordineates, ((r_ith+1)*sizeof(dimensional_coordineates)));
        dimensional_coordineates[r_ith] = (char**)malloc(10*(sizeof(dimensional_coordineates[r_ith])));
       
        for(int i=0 ; i<10 ; i++){
            dimensional_coordineates[r_ith][i] = (char*)malloc(3*(sizeof(dimensional_coordineates[r_ith][i])));
        }

        int buffer_it = 0;
        for(int i=0 ; i<10 ; i++){
            for(int j=0 ; j < 3 ; j++){
                dimensional_coordineates[r_ith][i][j] = (char)buffer[buffer_it];
                buffer_it++;
            }
        }
        
        r_ith++;
    }while(bytes_read == sizeof(buffer));

    

    close(fd);
    pid_t handler_signal[r_ith] ;
    char *programName = "./childProcess";
    int status;
    pid_t childPid;
    for(int i=0 ; i<r_ith ; i++){
        childPid = fork();

        if(childPid > 0){
            handler_signal[i] = childPid;
        }
        if(childPid == -1){
            perror("fork");
            exit(1);
        }else if(childPid == 0){
            
            execve(programName, argv, dimensional_coordineates[i]);
            exit(1);
        
        }
        
        
    }
    
    while ((childPid = wait(&status)) > 0){
        
        if(signal_arrived == 1){
            for(int i=0 ; i<r_ith ; i++){
                kill(handler_signal[i] , SIGINT);
            }

            exit(1);
            
        }

        if(childPid == -1){
            if(errno == ECHILD){
                break;
            }else{
                exit(EXIT_FAILURE);
            }
        }
    }
    

    for(int i=0 ; i<r_ith; i++){
        for(int j=0 ; j<10 ; j++){
            free(dimensional_coordineates[i][j]);
        }
        free(dimensional_coordineates[i]);
    }
    free(dimensional_coordineates);

    char* norms = (char*) malloc(1*sizeof(char));
    int offset = 0;
    char buffer_array[8];
    int output_index = check_output_file(argv, argc);
    fprintf(stdout, "Reached EOF, collecting outputs from  %s\n", argv[output_index]);
    fd = open(argv[output_index], O_RDONLY);

    do{
        bytes_read = read(fd, buffer_array, sizeof(buffer_array));
        
        norms = (char*)realloc(norms, (offset+bytes_read)*sizeof(char));
        for(int i=0 ; i<bytes_read ; i++){
            norms[offset] = buffer_array[i];
            offset++;
        }
        
    }while(bytes_read == sizeof(buffer_array));
    close(fd);
    
    
    double** norm_double = (double**)malloc(1*sizeof(norm_double));

    int it = 0;
    int r_ith_it = 0;
    int r_ith_index = 0;
    char* p;
    for( p = strtok(norms, " \n") ; p != NULL ; p = strtok(NULL, " \n") ){
        if(it%9 == 0){
            if(r_ith == r_ith_it){
                break;
            }
            r_ith_it++;
            r_ith_index = 0;
            if(it != 0){
                norm_double = (double**)realloc(norm_double, (r_ith_it*sizeof(norm_double)));
            }
            norm_double[r_ith_it-1] = (double*)malloc(9*sizeof(double));
            
        }
        
        norm_double[r_ith_it-1][r_ith_index] = atof(p);  
        r_ith_index++;
        it++;
        
    }
    
    
    free(norms);
    
    double normss[r_ith];
    for(int i=0 ; i<r_ith-1 ; i++){
        normss[i] = frobeniusNorm(norm_double[i]);
    }
    
    
    double min = 100000000;
    int first = -1;
    int second = -1;
    for(int i=0 ; i < r_ith ; i++){
        for(int j=0 ; j < r_ith ; j++){
            if(fabs(normss[i]- normss[j]) <= min ){
                if(i != j){
                    min = fabs(normss[i] - normss[j]);
                    first = i;
                    second = j;
                    
                }
                
            }
        }
    }

 
    
    fprintf(stdout,"The closest 2 matrices are ");
    for (int i = 0; i < 9; ++i){
        fprintf(stdout,"%.4f ",norm_double[first][i]);
    }
    fprintf(stdout,"\n");
    fprintf(stdout,"and ");
                    
    for (int i = 0; i < 9; ++i){
        fprintf(stdout,"%f ",norm_double[second][i]);
    }
    fprintf(stdout," and their distance is: %.4f\n",min);
    
    int ofd = open(argv[output_index], O_RDONLY | O_TRUNC);
    if (ofd == -1){
        /* code */
        perror("fd failed");
    }
    
    close(ofd);

    for(int i=0 ; i<r_ith ; i++){
        free(norm_double[i]);
    }
    free(norm_double);

    // remove(argv[output_index]);
    
    return 0;
}