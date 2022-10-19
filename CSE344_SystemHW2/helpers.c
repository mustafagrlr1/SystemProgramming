#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


int check_input_file(char* argv[], int argc){
        
    for(int i=0 ; i<argc-1 ; i++){
        if(strcmp(argv[i], "-i") == 0){
            return i+1;
        }
    }
    return -1;
}

int check_output_file(char* argv[], int argc){
    for(int i=0 ; i<argc-1 ; i++){
            if(strcmp(argv[i], "-o") == 0){
                return i+1;
            }
        }
        return -1;
}






double frobeniusNorm(double* matrix)
{
    double sum = 0.0;
    for (int i = 0; i < 9; i++) {
        sum += matrix[i] * matrix[i];
    }
 
    double result = sqrt(sum);
    return result;
}