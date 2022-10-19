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


volatile sig_atomic_t sigtermInterrupt = 0;

int sem_id;

char* inputFilePath;

int N;
int C;



int sem1;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		sigtermInterrupt = 1;
}

//Returns length of string
size_t strlen(const char* string){
	size_t n = 0;

	while(string[n]) ++n;

	return n;
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


void* consumer(void* arg){

	time_t ltime; 
	

	struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

	struct sembuf sem[2];
	int my_id = *((int*) arg);

	int value_sem1 = 0;
	int value_sem2 = 0;

	sem[0].sem_num = 0;
	sem[0].sem_op = -1;
	sem[0].sem_flg = 0;

	sem[1].sem_num = 1;
	sem[1].sem_op = -1;
	sem[1].sem_flg = 0;

	char* message;
	message = (char*)malloc(256);
	for(int i=0 ; i<N ; i++){
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		if((value_sem1 = semctl(sem_id, 0, GETVAL)) == -1){
			perror("Value sem1 error!!");
			free(message);
			exit(EXIT_FAILURE);
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		if((value_sem2 = semctl(sem_id, 1, GETVAL)) == -1){
			perror("Value sem1 error!!");
			free(message);
			exit(EXIT_FAILURE);
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		ltime=time(NULL);
		sprintf(message, "%.24s Consumer-%d at iteration %d (waiting).Post-consumption amounts: %d x ‘1’, %d x ‘2’.\n",asctime(localtime(&ltime)),my_id,i,value_sem1,value_sem2);
		write(1,message, strlen(message));
		strcpy(message, "");
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		if(semop(sem_id, sem, 2) == -1){
			perror("semop supplier error!!");
			free(message);
			exit(EXIT_FAILURE);
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}

		if((value_sem1 = semctl(sem_id, 0, GETVAL)) == -1){
			perror("Value sem1 error after semop!!");
			free(message);
			exit(EXIT_FAILURE);
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		if((value_sem2 = semctl(sem_id, 1, GETVAL)) == -1){
			perror("Value sem1 error after semop!!");
			free(message);
			exit(EXIT_FAILURE);
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		ltime=time(NULL);
		sprintf(message, "%.24s Consumer-%d at iteration %d (consumed).Post-consumption amounts: %d x ‘1’, %d x ‘2’.\n",asctime(localtime(&ltime)),my_id,i,value_sem1,value_sem2);
		write(1,message, strlen(message));
		strcpy(message, "");
		
	}

	sprintf(message, "%.24s Consumer-%d has left.\n",asctime(localtime(&ltime)),my_id);
	write(1,message, strlen(message));
	strcpy(message, "");
	
	free(message);

	return NULL;
}

void* supplier(void* arg){
	//
	//
	time_t ltime; 
	unsigned char buf[1] = {""};

	struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

	/*Read csv file*/
	size_t bytes_read;
	int value_sem1 = 0;
	int value_sem2 = 0;
	int readFd = open(inputFilePath, O_RDONLY);

	struct sembuf sem;
	sem.sem_num = 0;
	sem.sem_op = 1;
	sem.sem_flg = 0;

	char* message;
	message = (char*)malloc(256);
	do{
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		bytes_read = read(readFd, buf, sizeof(buf));
		if(bytes_read != sizeof(buf)){
			break;
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			kill(getpid(), SIGINT);
			semctl(sem_id, 1, IPC_RMID);
			free(message);
			exit(EXIT_FAILURE);
		}
		if(buf[0] == 10 || buf[0] == 13 || buf[0] == ' '){

		}else if(buf[0] == '1'){
			// 1 numarayi threade post et
			sem.sem_num = 0;
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem1 = semctl(sem_id, 0, GETVAL)) == -1){
				perror("Value sem1 error!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem2 = semctl(sem_id, 1, GETVAL)) == -1){
				perror("Value sem1 error!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			ltime=time(NULL);
			sprintf(message, "%.24s Supplier: read from input a ‘%c’. Current amounts: %d x ‘1’, %d x ‘2’.\n",asctime(localtime(&ltime)),'1',value_sem1,value_sem2);
			write(1,message, strlen(message));
			strcpy(message, "");
			
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if(semop(sem_id, &sem, 1) == -1){
				perror("semop supplier error!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem1 = semctl(sem_id, 0, GETVAL)) == -1){
				perror("Value sem1 error after semop!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem2 = semctl(sem_id, 1, GETVAL)) == -1){
				perror("Value sem1 error after semop!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			ltime=time(NULL);
			sprintf(message, "%.24s Supplier: delivered a ‘%c’. Post-delivery amounts: %d x ‘1’, %d x ‘2’.\n",asctime(localtime(&ltime)),'1',value_sem1,value_sem2);
			write(1,message, strlen(message));
			strcpy(message, "");

		}else if(buf[0] == '2'){
			// 2 numarayi threade post et
			sem.sem_num = 1;
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem1 = semctl(sem_id, 0, GETVAL)) == -1){
				perror("Value sem1 error!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem2 = semctl(sem_id, 1, GETVAL)) == -1){
				perror("Value sem1 error!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			ltime=time(NULL);
			sprintf(message, "%.24s Supplier: read from input a ‘%c’. Current amounts: %d x ‘1’, %d x ‘2’.\n",asctime(localtime(&ltime)),'2',value_sem1,value_sem2);
			write(1,message, strlen(message));
			strcpy(message, "");

			if(semop(sem_id, &sem, 1) == -1){
				perror("semop supplier error!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			
			if((value_sem1 = semctl(sem_id, 0, GETVAL)) == -1){
				perror("Value sem1 error after semop!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			if((value_sem2 = semctl(sem_id, 1, GETVAL)) == -1){
				perror("Value sem1 error after semop!!");
				free(message);
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				kill(getpid(), SIGINT);
				semctl(sem_id, 1, IPC_RMID);
				free(message);
				exit(EXIT_FAILURE);
			}
			ltime=time(NULL);
			sprintf(message, "%.24s Supplier: delivered a ‘%c’. Post-delivery amounts: %d x ‘1’, %d x ‘2’.\n",asctime(localtime(&ltime)),'2',value_sem1,value_sem2);
			write(1,message, strlen(message));
			strcpy(message, "");
		}

	}while(bytes_read == sizeof(buf));
	ltime=time(NULL);
	sprintf(message, "%.24s The Supplier has left..\n",asctime(localtime(&ltime)));
	write(1,message, strlen(message));
	strcpy(message, "");
	free(message);
	return NULL;
}


int main(int argv, char* argc[]){

	setbuf(stdout, NULL);

    int _c_ = 0;
    int _n_ = 0;
    int _f_ = 0;
    int c;

    int cValue;
    int nValue;

    while((c = getopt(argv, argc, "N:C:F:")) != -1){
      switch(c)
      {
        case 'F':
          inputFilePath = optarg;
          _f_ = 1;

          break;
        case 'N':
          if(isNumber(optarg)){
              nValue = atoi(optarg);
							if(nValue <= 1){
								perror("-N value is less than 2");
								exit(EXIT_FAILURE);
							}
          }else{
            perror("-N is not a value");
            exit(EXIT_FAILURE);
          }
          _n_ = 1;

          break;
        case 'C':
          if(isNumber(optarg)){
            cValue = atoi(optarg);
						if(cValue <= 4){
							perror("-C value is less than 5!!");
							exit(EXIT_FAILURE);
						}
          }else{
            perror("-C is not a value");
            exit(EXIT_FAILURE);
          }
          _c_ = 1;
        default:
          break;
      }
    }
    if((!_f_) || (!_c_) || (!_n_)){
      perror("No input File");
      exit(EXIT_FAILURE);
    }

		N = nValue;
		C = cValue;

    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

	/*semaphore*/
	union semun{
		int val;
		struct semid_ds *buf;
		ushort array[1];
	};

	if(sigtermInterrupt){
		write(1, "KILLED",strlen("KILLED"));
		exit(EXIT_SUCCESS);
    }
	if((sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("Creating semaphore error");
		exit(EXIT_FAILURE);
	}
	if(sigtermInterrupt){
		write(1, "KILLED",strlen("KILLED"));
		semctl(sem_id, 1, IPC_RMID);
		exit(EXIT_SUCCESS);
    }

		/*read csv from the file and add all the number to buffer*/
    unsigned char buf[1] = {""};

    /*Read csv file*/
    size_t bytes_read;
    if(sigtermInterrupt){
		write(1, "KILLED",strlen("KILLED"));
		exit(EXIT_SUCCESS);
    }
	int readFd = open(inputFilePath, O_RDONLY);
	int count = 0;
	if(sigtermInterrupt){
		write(1, "KILLED",strlen("KILLED"));
		semctl(sem_id, 1, IPC_RMID);
		exit(EXIT_SUCCESS);
    }
	// bir kere file in icinde kac tane sayi olduguna bakmak icin bos okuyorum.
	do{

		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}
		bytes_read = read(readFd, buf, sizeof(buf));
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}
		if(bytes_read != sizeof(buf)){
			break;
		}
		if(buf[0] == 10 || buf[0] == 13 || buf[0] == ' '){

		}else if(buf[0] == 49 || buf[0] == 50){
			count++;
		}else{
			perror("file does cover other values!!");
			exit(EXIT_FAILURE);
		}
    }while(bytes_read == sizeof(buf));
		
		if(N*C*2 != count){
			perror("N*C is not equal to file values");
			exit(EXIT_FAILURE);
		}




		pthread_t tid_thread [C], tid_supplier;
		pthread_attr_t attr;
		int s;
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}
		s = pthread_attr_init(&attr);
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}
		if(s != 0){
			perror("pthread_attr_init");
			exit(EXIT_FAILURE);
		}
		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}

		s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if(s != 0){
			perror("pthread_attr_setdetachstate");
			exit(EXIT_FAILURE);
		}
		int thread_no[C];
		int r;

		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}

		//Create supplier
		if((r = pthread_create(&tid_supplier, &attr ,supplier, NULL))  != 0){
			write(1, "Creating supplier thread failed",
								sizeof("Creating supplier thread failed"));
			exit(EXIT_FAILURE);
		}

		if(sigtermInterrupt){
			write(1, "KILLED",strlen("KILLED"));
			semctl(sem_id, 1, IPC_RMID);
			exit(EXIT_SUCCESS);
		}
		for(int i=0; i < C ; i++){
			thread_no[i] = i;
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				semctl(sem_id, 1, IPC_RMID);
				exit(EXIT_SUCCESS);
			}
			if((r = pthread_create(&tid_thread[i],NULL, consumer, (void*) &thread_no[i])) !=0 ){
				write(1, "Creating consumer thread failed",
									sizeof("Creating consumer thread failed"));
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				semctl(sem_id, 1, IPC_RMID);
				exit(EXIT_SUCCESS);
			}
		}

		for(int i=0 ; i < C ; i++){
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				semctl(sem_id, 1, IPC_RMID);
				exit(EXIT_SUCCESS);
			}
			if((r = pthread_join(tid_thread[i], NULL)) != 0){
				write(1, "Joining thread failed",
									sizeof("Joining thread failed"));
				exit(EXIT_FAILURE);
			}
			if(sigtermInterrupt){
				write(1, "KILLED",strlen("KILLED"));
				semctl(sem_id, 1, IPC_RMID);
				exit(EXIT_SUCCESS);
			}
		}

		if(semctl(sem_id, 1, IPC_RMID) == -1){
			perror("semctl error!");
			exit(EXIT_FAILURE);
		}
		
		s = pthread_attr_destroy(&attr);
		if(s != 0){
			perror("pthread_attr_destroy");
			exit(EXIT_FAILURE);
		}




}
