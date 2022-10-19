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

volatile sig_atomic_t sigtermInterrupt = 0;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		sigtermInterrupt = 1;
}


typedef struct isTrueFlag{
	int isWalnuts;
	int isSugar;
	int isMilk;
	int isFlour;
	pid_t pids[10];
	char arr[3];
}isTrueFlag;

sem_t* sem_agent;

sem_t* sem_chef1;
sem_t* sem_chef2;
sem_t* sem_chef3;
sem_t* sem_chef4;
sem_t* sem_chef5;
sem_t* sem_chef0;

sem_t* walnut;
sem_t* sugar;
sem_t* milk;
sem_t* flour;

sem_t* is_done;

sem_t* sem_mux;



void init_semaphores(){
	sem_agent = mmap(NULL, sizeof(*sem_agent), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_agent, 1, 1) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sem_chef0 = mmap(NULL, sizeof(*sem_chef0), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_chef0, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sem_chef1 = mmap(NULL, sizeof(*sem_chef1), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_chef1, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sem_chef2 = mmap(NULL, sizeof(*sem_chef2), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_chef2, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sem_chef3 = mmap(NULL, sizeof(*sem_chef3), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_chef3, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sem_chef4 = mmap(NULL, sizeof(*sem_chef4), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_chef4, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sem_chef5 = mmap(NULL, sizeof(*sem_chef5), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_chef5, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	walnut = mmap(NULL, sizeof(*walnut), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(walnut, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	milk = mmap(NULL, sizeof(*milk), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(milk, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	flour = mmap(NULL, sizeof(*flour), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(flour, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	sugar = mmap(NULL, sizeof(*sugar), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sugar, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	sem_mux = mmap(NULL, sizeof(*sem_mux), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(sem_mux, 1, 1) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
	is_done = mmap(NULL, sizeof(*is_done), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(is_done, 1, 0) < 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
}

void postIng(unsigned char ingredient){
	
	if(ingredient == 'M'){
		sem_post(milk);
	}else if(ingredient == 'F'){
		sem_post(flour);
	}else if(ingredient == 'W'){
		sem_post(walnut);
	}else if(ingredient == 'S'){
		sem_post(sugar);
	}else{
		fprintf(stderr, "Wrong Ingredient!!");
		exit(EXIT_FAILURE);
	}
}


int main(int argv, char* argc[]){


	int _i_ = 0;
	int c;
	char* inputFilePath;

	while((c = getopt(argv, argc, "i:")) != -1){
		switch(c)
		{
			case 'i':
				inputFilePath = optarg;
				_i_ = 1;

				break;
			default:
				break;
		}
	}
	if((!_i_)){
		perror("No input File");
		exit(EXIT_FAILURE);
	}

	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
		perror("sigaction");
	}

	isTrueFlag* istrueflag;
	char shared_memory_name[25] = "shared_memory";

	int shared_memory_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666);

	if(shared_memory_fd == -1){
		perror("fd error");
		exit(EXIT_FAILURE);
	} 

	int size = sizeof(istrueflag);

	if(ftruncate(shared_memory_fd, size) == -1){
		perror("truncate error");
		exit(EXIT_FAILURE);
	}   


	istrueflag = (struct isTrueFlag*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);

	/*purpose is initializing the semaphores*/
	init_semaphores(); 

	/*first 6 for chefs 	*/
	/*second 4 for pushers	*/
	pid_t children_pid[10];	 					

	for(int i=0 ; i<10 ; i++){

		children_pid[i] = fork();

		if(i == 0 && children_pid[i] == 0){
			int count=0;
			while(1){
				
				fprintf(stdout, "chef%d (pid %d) is waiting for %c and %c\n", i,getpid(), istrueflag->arr[0], istrueflag->arr[1]);
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_chef0);
				if(sigtermInterrupt == 1){
					break;
				}
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[0]);
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[1]);
				fprintf(stdout,"chef%d (pid %d) is preparing the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_agent);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(is_done);
				
				count++;
				fprintf(stdout,"chef%d (pid %d) has delivered the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
			}
			fprintf(stdout,"chefi (pid %d) is exiting\n", getpid());
			return count;
			
		}else if(i == 1 && children_pid[i] == 0){
			int count=0;
			while(1){
				
				fprintf(stdout, "chef%d (pid %d) is waiting for %c and %c\n", i,getpid(), istrueflag->arr[0], istrueflag->arr[1]);
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_chef1);
				if(sigtermInterrupt == 1){
					break;
				}
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[0]);
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[1]);
				fprintf(stdout,"chef%d (pid %d) is preparing the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_agent);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(is_done);
				count++;
				fprintf(stdout,"chef%d (pid %d) has delivered the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}

			}
			fprintf(stdout,"chefi (pid %d) is exiting\n", getpid());
			return count;
			
		}else if(i == 2 && children_pid[i] == 0){
			int count = 0;
			while(1){
				fprintf(stdout, "chef%d (pid %d) is waiting for %c and %c\n", i,getpid(), istrueflag->arr[0], istrueflag->arr[1]);
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_chef2);
				if(sigtermInterrupt == 1){
					break;
				}
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[0]);
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[1]);
				fprintf(stdout,"chef%d (pid %d) is preparing the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_agent);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(is_done);
				if(sigtermInterrupt == 1){
					break;
				}
				count++;
				fprintf(stdout,"chef%d (pid %d) has delivered the dessert\n", i, getpid());
				fflush(stdout);
			}
			fprintf(stdout,"chefi (pid %d) is exiting\n", getpid());
			return count;
			
		}else if(i == 3 && children_pid[i] == 0){
			int count=0;
			while(1){
				fprintf(stdout, "chef%d (pid %d) is waiting for %c and %c\n", i,getpid(), istrueflag->arr[0], istrueflag->arr[1]);
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_chef3);
				if(sigtermInterrupt == 1){
					break;
				}
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[0]);
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[1]);
				fprintf(stdout,"chef%d (pid %d) is preparing the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_agent);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(is_done);
				if(sigtermInterrupt == 1){
					break;
				}
				count++;
				fprintf(stdout,"chef%d (pid %d) has delivered the dessert\n", i, getpid());
				fflush(stdout);
			}
			fprintf(stdout,"chefi (pid %d) is exiting\n", getpid());
			return count;
			
		}else if(i == 4 && children_pid[i] == 0){
			int count=0;
			while(1){
				fprintf(stdout, "chef%d (pid %d) is waiting for %c and %c\n", i,getpid(), istrueflag->arr[0], istrueflag->arr[1]);
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_chef4);
				if(sigtermInterrupt == 1){
					break;
				}
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[0]);
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[1]);
				fprintf(stdout,"chef%d (pid %d) is preparing the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_agent);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(is_done);
				if(sigtermInterrupt == 1){
					break;
				}
				count++;
				fprintf(stdout,"chef%d (pid %d) has delivered the dessert\n", i, getpid());
				fflush(stdout);
			}
			fprintf(stdout,"chefi (pid %d) is exiting\n", getpid());
			return count;
			
		}else if(i == 5 && children_pid[i] == 0){
			int count = 0;
			while(1){
				fprintf(stdout, "chef%d (pid %d) is waiting for %c and %c\n", i,getpid(), istrueflag->arr[0], istrueflag->arr[1]);
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_chef5);
				if(sigtermInterrupt == 1){
					break;
				}
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[0]);
				fprintf(stdout,"chef%d (pid %d) has taken the %c\n", i, getpid(), istrueflag->arr[1]);
				fprintf(stdout,"chef%d (pid %d) is preparing the dessert\n", i, getpid());
				fflush(stdout);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_agent);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(is_done);
				if(sigtermInterrupt == 1){
					break;
				}
				count++;
				fprintf(stdout,"chef%d (pid %d) has delivered the dessert\n", i, getpid());
				fflush(stdout);
			}
			fprintf(stdout,"chefi (pid %d) is exiting\n", getpid());
			return count;
			
		}else if(i == 6 && children_pid[i] == 0){
			while(1){
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(walnut);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
				if(istrueflag->isFlour == 1){
					istrueflag->isFlour = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef1);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isMilk == 1){
					istrueflag->isMilk = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef4);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isSugar == 1){
					istrueflag->isSugar = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef0);
					if(sigtermInterrupt == 1){
						break;
					}
				}
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
			}
			exit(EXIT_SUCCESS);
		}else if(i == 7 && children_pid[i] == 0){
			while(1){
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sugar);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
				
				if(istrueflag->isFlour == 1){
					istrueflag->isFlour = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef2);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isMilk == 1){
					istrueflag->isMilk = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef5);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isWalnuts == 1){
					istrueflag->isWalnuts = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef0);
					if(sigtermInterrupt == 1){
						break;
					}
				}
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
			}
			exit(EXIT_SUCCESS);
			
		}else if(i == 8 && children_pid[i] == 0){
			while(1){
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(milk);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
				
				if(istrueflag->isFlour == 1){
					istrueflag->isFlour = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef3);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isSugar == 1){
					istrueflag->isSugar = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef5);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isWalnuts == 1){
					istrueflag->isWalnuts = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef4);
					if(sigtermInterrupt == 1){
						break;
					}
				}
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
			}
			exit(EXIT_SUCCESS);
		}else if(i == 9 && children_pid[i] == 0){
			while(1){
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(flour);
				if(sigtermInterrupt == 1){
					break;
				}
				sem_wait(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}

				if(istrueflag->isMilk == 1){
					istrueflag->isMilk = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef3);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isSugar == 1){
					istrueflag->isSugar = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef2);
					if(sigtermInterrupt == 1){
						break;
					}
				}else if(istrueflag->isWalnuts == 1){
					istrueflag->isWalnuts = 0;
					if(sigtermInterrupt == 1){
						break;
					}
					sem_post(sem_chef1);
					if(sigtermInterrupt == 1){
						break;
					}
				}
				if(sigtermInterrupt == 1){
					break;
				}
				sem_post(sem_mux);
				if(sigtermInterrupt == 1){
					break;
				}
			}
			exit(EXIT_SUCCESS);
		}
		

	}

	/*read csv from the file and add all the number to buffer*/
    unsigned char buf[3] = {""};
    
    /*Read csv file*/
    size_t bytes_read;
    int readFd = open(inputFilePath, O_RDONLY);
	
	do{
		sem_wait(sem_agent);

        bytes_read = read(readFd, buf, sizeof(buf));
		istrueflag->arr[0] = buf[0];
		istrueflag->arr[1] = buf[1];
		if(buf[1] == 'M'){
			istrueflag->isMilk = 1;
		}else if(buf[1] == 'F'){
			istrueflag->isFlour = 1;
		}else if(buf[1] == 'W'){
			istrueflag->isWalnuts = 1;
		}else if(buf[1] == 'S'){
			istrueflag->isSugar = 1;
		}else{
			fprintf(stderr, "Wrong Ingredient!!!\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout,"the wholesaler (pid %d) delivers %c and %c\n", getpid(), buf[0], buf[1]);
		postIng(buf[0]);
		fprintf(stdout, "the wholesaler (pid %d) is waiting for the dessert\n", getpid());
		sem_wait(is_done);
		fprintf(stdout,"the wholesaler (pid %d) has obtained the dessert and left\n", getpid());

    }while(bytes_read == sizeof(buf));
	
	int ret;
	int wstatus[10];
	int returned[10];
	for(int i=0 ; i<10 ; i++){
		ret = kill(children_pid[i], SIGINT);
		if (ret == -1) {
			perror("kill");
			exit(EXIT_FAILURE);
		}

	}
	int total_desert = 0;
	
	for(int i=0 ; i<10 ; i++){
		if (waitpid(children_pid[i], &wstatus[i], WUNTRACED | WCONTINUED) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

		if(WIFEXITED(wstatus[i])){
			returned[i] = WEXITSTATUS(wstatus[i]);
			
		}
	}	

	for(int i=0 ; i<6 ; i++){
		total_desert += returned[i];
	}

	sem_destroy(sem_agent);
	sem_destroy(sem_chef0);
	sem_destroy(sem_chef1);
	sem_destroy(sem_chef2);
	sem_destroy(sem_chef3);
	sem_destroy(sem_chef4);
	sem_destroy(sem_chef5);
	sem_destroy(walnut);
	sem_destroy(sugar);
	sem_destroy(milk);
	sem_destroy(flour);
	sem_destroy(is_done);
	sem_destroy(sem_mux);

	fprintf(stdout,"the wholesaler (pid %d) is done (total desserts: %d)\n", getpid(), total_desert);

	fflush(stdout);

}
