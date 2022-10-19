#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "helpers.c"
#include <string.h>

#define _MAX_COL 10
#define _MAX_ROW 3

sig_atomic_t signal_arrived = 0;

void handler(){
    signal_arrived = 1;
}
int main(int argc, char* argv[], char** env){
    
    double average[3] = {0, 0, 0};
	for (int i = 0; i < 10; i++){
		for(int j=0 ; j<3 ; j++){
            average[j] += env[i][j];
        }
    }
    for(int i=0 ; i<3 ; i++){
        average[i] = average[i] / 10;
    }
	
    double result_process[3][3];

	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			result_process[i][j] = 0.0;
			for (int k = 0; k < 10; k++){
				result_process[i][j] += (average[i] - env[k][i]) * (average[j] - env[k][j]);
			
            }
            result_process[i][j] /= 10 ;
        }	
    }
    if(signal_arrived == 1){
        exit(EXIT_SUCCESS);   
    }
    char array[9][20];

    int count = 0;
    for(int i=0 ; i<3 ; i++){
        for(int j=0 ; j<3 ; j++){
            sprintf(array[count], "%.4f", result_process[i][j]);
            count++;  
        }
    }

    
        fprintf(stdout, "R_%d created with (",getpid());
        for(int j = 0; j<10 ; j++){
            for(int k = 0 ; k<3 ; k++){
                if(k!=3){
                    fprintf(stdout, "%.4f,",result_process[j][k]);
                }else{
                    fprintf(stdout, "%.4f,",result_process[j][k]);
                }
            }
        }
        fprintf(stdout, ")\n");
    

    
    int fd;
    struct flock lock;
    int output_index = check_output_file(argv, argc);

    fd = open(argv[output_index],O_WRONLY | O_CREAT , 0644);
        if (fd == -1){
        /* code */
        perror("fd failed");
    }

    if(signal_arrived == 1){
        close(fd);
        exit(EXIT_SUCCESS);   
    }

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;

    fcntl(fd, F_SETLKW, &lock);

    lseek(fd, 0, SEEK_END );
    count = 0;
    for(int i=0 ; i<3 ; i++){
        for(int j=0 ; j<3 ; j++){
            
            if(write(fd, array[count], strlen(array[count])) == -1){
                perror("error");
                return 1;
            }
            if(write(fd, " ", 1) == -1){
                perror("error");
                return 1;
            }
            
            count++;
        }
        if(write(fd, "\n", 1) == -1){
            perror("error");
            return 1;
    
        }
        
    }    
    if(signal_arrived == 1){
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &lock);
        close(fd);
        exit(EXIT_SUCCESS);   
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    
    close(fd);
    
    if(signal_arrived == 1){
        exit(EXIT_SUCCESS);   
    }

    
    return 0;
}