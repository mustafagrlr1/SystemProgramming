#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <math.h>
#include <sys/time.h>
#include <complex.h>

#define PI 3.142857

volatile sig_atomic_t sigtermInterrupt = 0;

char* FilePath1;
char* FilePath2;
char* outputFile;

unsigned int byte;
unsigned int** file1;
unsigned int** file2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

float** realNumber;
float** imagNumber;

float complex** number;

int n;
int m;

int arrived=0;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		sigtermInterrupt = 1;
}

int isNumber(char* value){
  for(int i=0 ; i<strlen(value) ; i++){
    if(value[0] == 48){
      perror("Number should not start with zero");
      exit(EXIT_FAILURE);
    }
    if(value[i]< 48 || value[i] > 57){
      return 0;
    }
  }
  return 1;
}


void* function(void* arg){

  time_t ltime; 

  float resultReal[byte][byte];
  float resultImag[byte][byte];

  

  double time_spent = 0.0;

  clock_t start = clock();
  
  int my_id = *((int*)arg);

  long long int C[byte][byte];
  for(int i=0;i<byte;i++)    
  {    
    for(int j=0;j<byte;j++){        
      for(int k=0;k<byte;k++)    
      {    
        C[i][j]+=file1[i][k]*file2[k][j]; 
      }    
    }    
  }  



  long long int C_low[byte][byte/m];

  int r = 0,k;
  for(int i=0 ; i<byte ; i++){
    
    k=0;
    for(int j=(my_id-1)*(byte/m) ; j<((my_id-1)*(byte/m))+byte/m ;j++){
      C_low[r][k] = C[i][j];
      k++;
    }
    r++;
  }
  char message[1024];
  
  clock_t end = clock();
  time_spent += (double)(end - start) / CLOCKS_PER_SEC;
  ltime=time(NULL);
  sprintf(message, "%.24s Thread %d has reached the rendezvous point in %f seconds.\n",asctime(localtime(&ltime)),my_id, time_spent);
  if(write(1, message, strlen(message)) == -1){
    perror("writing message error!");
    exit(EXIT_FAILURE);
  }
  if(pthread_mutex_lock(&mutex) != 0){
    perror("error locking mutex");
    exit(EXIT_FAILURE);
  }
  ++arrived;

  if(arrived < m){
		if(pthread_cond_wait(&cond,&mutex) != 0){
      perror("error condition wait");
      exit(EXIT_FAILURE);
    }  
  }
	else{
		if(pthread_cond_broadcast(&cond) != 0){
      perror("error condition broadcast");
      exit(EXIT_FAILURE);
    } 
  }
  
	if(pthread_mutex_unlock(&mutex) != 0){
    perror("error mutex unlocking");
    exit(EXIT_FAILURE);
  }

  if(my_id == 0){
    free(file1);
    free(file2);
  }

  ltime=time(NULL);
  sprintf(message, "%.24s Thread %d is advancing to the second part.\n",asctime(localtime(&ltime)),my_id);
  if(write(1, message, strlen(message)) == -1){
    perror("writing message error!");
    exit(EXIT_FAILURE);
  }

  time_spent = 0.0;

  start = clock();
  

  float ak=0; 
  float bk=0;

  //float complex number[byte][byte];
  
  number = (float complex**)malloc(byte*sizeof(float complex));
  for(int i=0 ; i<byte ; i++){
    number[i] = (float complex*)malloc(byte*sizeof(float complex));
  }
  for(int i=0; i<byte ;i++)
  {
    for(int j=(my_id-1)*(byte/m);j<((my_id-1)*(byte/m))+byte/m;j++)
    {

      ak = 0;  
      bk = 0;
        for(int ii=0;ii<byte;ii++)
        {
          for(int jj=0;jj<byte/m;jj++)
            {

            float x=-2.0*PI*i*ii/(float)byte;
            float y=-2.0*PI*j*jj/(float)byte/m;
            ak+=C_low[ii][jj]*cos(x+y);
            bk+=C_low[ii][jj]*1.0*sin(x+y);
          }
        }
    
      resultImag[i][j]=bk;
      resultReal[i][j]=ak;
    }
}

for(int i=0 ; i<byte ; i++){
  for(int j=(my_id-1)*(byte/m);j<((my_id-1)*(byte/m))+byte/m;j++)
  {
    number[i][j] = resultReal[i][j] + resultImag[i][j]*I;
  }
}



  end = clock();
  time_spent += (double)(end - start) / CLOCKS_PER_SEC;

  ltime=time(NULL);
  sprintf(message, "%.24s Thread %d has has finished the second part in %f seconds.\n",asctime(localtime(&ltime)),my_id, time_spent);
  if(write(1, message, strlen(message)) == -1){
    perror("writing message error!");
    exit(EXIT_FAILURE);
  }


  return NULL;
}


int main(int argv, char* argc[]){

    time_t ltime;

    
    double time_spent = 0.0;

    clock_t start = clock();

    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

    int _i_ = 0, _j_ = 0,_o_ = 0, _n_ = 0, _m_ = 0;
    int c;


    while((c = getopt(argv, argc, "i:j:o:n:m:")) != -1){
      switch(c)
      {
        case 'i':
          FilePath1 = optarg;
          _i_ = 1;

          break;
        case 'j':
          FilePath2 = optarg;
          _j_ = 1;

          break;
        case 'o':
            outputFile = optarg;
            _o_ = 1;
            break;
        case 'n':
          if(isNumber(optarg)){
            
            
            n = atoi(optarg);
            if(n <= 2){
                perror("-n value is less than 2");
                exit(EXIT_FAILURE);
            }

          }else{
            perror("-n is not a value");
            exit(EXIT_FAILURE);
          }
          _n_ = 1;
          break;
        case 'm':
          if(isNumber(optarg)){
            m = atoi(optarg);
            if(m <= 1){
                perror("-m value is less than 2");
                exit(EXIT_FAILURE);
            }

          }else{
            perror("-m is not a value");
            exit(EXIT_FAILURE);
          }
          _m_ = 1;
          break;
        default:
          break;
      }
    }
    if((!_i_) || (!_j_) || (!_o_)|| (!_m_)|| (!_n_)){
      perror("No input File");
      exit(EXIT_FAILURE);
   }

  byte = pow(2,n);

  
  
  unsigned char bufferFile1[byte][byte];
  unsigned char bufferFile2[byte][byte];
  
  /*Read csv file*/
  size_t bytes_read;
  int readFd = open(FilePath1, O_RDONLY);
  if(readFd == -1){
    perror("opening filepath1 in error");
    exit(EXIT_FAILURE);
  }
  int readFd2 = open(FilePath2, O_RDONLY);
  if(readFd2 == -1){
    perror("opening filepath2 in error");
    close(readFd);
    exit(EXIT_FAILURE);
  }
  int count = 0;
  do{
    if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
      close(readFd2);
      close(readFd);
			exit(EXIT_SUCCESS);
		}
		bytes_read = read(readFd, bufferFile1[count], sizeof(bufferFile1[count]));
    
    if(bytes_read == -1){
      perror("reading file");
      close(readFd2);
      close(readFd);
      exit(EXIT_FAILURE);
    }

    if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
      close(readFd2);
      close(readFd);
			exit(EXIT_SUCCESS);
		}
    if(bytes_read < byte){
      perror("Fatal error");
      close(readFd2);
      close(readFd);
      exit(EXIT_FAILURE);
    }
    count++;
    if(count == byte-1){
      break;
    }
	}while(bytes_read == sizeof(bufferFile1[count]));
  count = 0;
  do{
    if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
      close(readFd2);
      close(readFd);
			exit(EXIT_SUCCESS);
		}
		bytes_read = read(readFd2, bufferFile2[count], sizeof(bufferFile2[count]));
    if(bytes_read == -1){
      perror("reading file");
      close(readFd2);
      close(readFd);
      exit(EXIT_FAILURE);
    }
    if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
      close(readFd2);
      close(readFd);
			exit(EXIT_FAILURE);
    }
    count++;
    if(bytes_read < byte){
      perror("Fatal error");
      exit(EXIT_FAILURE);
    }
    
    if(count == byte-1){
      break;
    }
	}while(bytes_read == sizeof(bufferFile2[count]));

  file1 = (unsigned int**)malloc(byte*sizeof(unsigned int));
  file2 = (unsigned int**)malloc(byte*sizeof(unsigned int));
  
  for(int i=0 ; i<byte;i++){
    file1[i] = (unsigned int*)malloc(byte*sizeof(unsigned int));
    file2[i] = (unsigned int*)malloc(byte*sizeof(unsigned int));
    for(int j=0 ; j<byte;j++){
      file1[i][j] = (unsigned int)bufferFile1[i][j];
      file2[i][j] = (unsigned int)bufferFile2[i][j];
      
    }
  }

  close(readFd);
  close(readFd2);

  char message[1024];
  ltime=time(NULL);
  sprintf(message, "%.24s Two matrices of size %dx%d have been read. The number of threads is %d\n",asctime(localtime(&ltime)),byte, byte, m);
  if(write(1, message, strlen(message)) == -1){
    perror("writing message error!");
    exit(EXIT_FAILURE);
  }

  
  if(sigtermInterrupt){
    write(1, "KILLED",strlen("KILLED"));
    exit(EXIT_FAILURE);
  }
  pthread_t thread [m];
  int thread_no[m];

  
  for(int i=0; i < m ; i++){
    thread_no[i] = i+1;
    if(sigtermInterrupt){
      write(1, "KILLED",strlen("KILLED"));
      exit(EXIT_FAILURE);
    }
    if((pthread_create(&thread[i],NULL, function, (void*) &thread_no[i])) !=0 ){
      perror("creating thread error!");
      exit(EXIT_FAILURE);
    }
    if(sigtermInterrupt){
      write(1, "KILLED",strlen("KILLED"));
      exit(EXIT_FAILURE);
    }
	}

  for(int i=0 ; i < m ; i++){
    if(sigtermInterrupt){
      write(1, "KILLED",strlen("KILLED"));
      exit(EXIT_FAILURE);
    }
    if((pthread_join(thread[i], NULL)) != 0){
      perror("joining thread error");
      exit(EXIT_FAILURE);
    }
  }
  
  int readFd3 = open(outputFile, O_WRONLY | O_CREAT);

  if(readFd3 == -1){
    perror("opening csv file error!!");
    exit(EXIT_FAILURE);
  }
  for(int i=0 ; i<byte ; i++){
    for(int j=0 ; j<byte ; j++){
      if(sigtermInterrupt){
        write(1, "KILLED",strlen("KILLED"));
        close(readFd3);
        exit(EXIT_FAILURE);
      }
      sprintf(message, "%f + j(%f)\n",creal(number[i][j]),cimag(number[i][j]));
      if(write(readFd3, message, strlen(message)) == -1){
          perror("error writing to output file");
          return 1;
      }
    }
  }

  close(readFd3);
  free(number);
  

  clock_t end = clock();
  time_spent += (double)(end - start) / CLOCKS_PER_SEC;
  ltime=time(NULL);
  sprintf(message, "%.24s The process has written the output file. The total time spent is %f seconds.\n",asctime(localtime(&ltime)),time_spent);
  if(write(1, message, strlen(message)) == -1){
    perror("writing message error!");
    exit(EXIT_FAILURE);
  }

  return 0;



}