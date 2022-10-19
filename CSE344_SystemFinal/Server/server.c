

#include "server.h"

#include "queue.h"

pthread_mutex_t mutex_barrier= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_barrier = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_queue = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_available = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_available = PTHREAD_COND_INITIALIZER;

int size_queue = 0;

int arrived=0;
int available=0;

int count_responses = 0;
pthread_mutex_t mutex_count = PTHREAD_MUTEX_INITIALIZER;


struct PORTSERVANT portservant[1000];
int portservant_size = 0;

Queue* queue = NULL;

ARGUMENT argument;

char** servant_server_port;

int flag_dataset = 1;

char** city_names;

int city_size = 0;


void printMessage(char *message){
    while(((write(STDERR_FILENO, message, strlen(message))) == -1) && (errno == EINTR));
    
}



int connect_to(char* ip, int port, char* new_message){
  int fd;
  int result = 0;
  fd = socket(AF_INET, SOCK_STREAM, 0);
  
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  // int length_struct = 0;
  // fprintf(stdout, "%s\n", ip);
  // fflush(stdout);
  if(inet_aton(ip, &addr.sin_addr) == -1){
    fprintf(stderr, "ipv4 address is invalid!!\n");
    exit(EXIT_FAILURE);
  }
  
  // perror("client requeste girdim");

  
  if((connect(fd, (struct sockaddr*)&addr, sizeof(addr))) == -1){
    close(fd);
    fprintf(stderr, "connection failed!!\n");
    exit(EXIT_FAILURE);
  }
  
  int size_of_struct = 400;
  // fprintf(stdout, "%s\n", new_message);
  if(write(fd, new_message, size_of_struct) == -1){
    perror("error occures reading from client to server!!");
    exit(EXIT_FAILURE);
  }

  bzero(new_message, 400);
  read(fd, &result, sizeof(int));

  
  close(fd);
  
  return result;
}

void* thread_pool(void* arg){
  

  
  
  //int my_id = *((int*)arg);
  pthread_mutex_lock(&mutex_barrier);
  
  arrived++;
  available++;
  if(arrived < argument.Number_of_Threads){
		
    pthread_cond_wait(&cond_barrier,&mutex_barrier);
       
  }else{
    pthread_cond_broadcast(&cond_barrier);
  }
	
  pthread_mutex_unlock(&mutex_barrier);

  pthread_mutex_lock(&mutex_barrier);
 
  while(arrived == argument.Number_of_Threads){
    
  }
  pthread_mutex_unlock(&mutex_barrier);
   


  
  int fd_server = 0;
  
  char message[400];
  


  int size_of_struct = 0;

  int servar_or_servant = 0;
  int begin = -1;
  int end = -1;
  char transaction[100];
  char real[100];
  char day[100];
  char day2[100];
  char* city = calloc(30, sizeof(char));
  time_t t;
  
  int pid = 0;
  char new_message[1024];
  char message_print[1024];
  while(1){

    pthread_mutex_lock(&mutex_queue);

    while(size_queue == 0){
      pthread_cond_wait(&cond_queue, &mutex_queue);
    }

    fd_server = poll_int(&queue);
    --size_queue;

    pthread_mutex_unlock(&mutex_queue);

    bzero(message, 400);

    struct RealEstateData* realEstate = calloc(1, sizeof(realEstate));
    read(fd_server, realEstate, sizeof(realEstate));
    // printf("%d\n", servar_or_servant);
              if(realEstate->choice == 1){

                
                  
                
              
                time(&t);
                sprintf(message_print + strlen(message_print),"[%.19s] Request arrived %s.\nContacting all servers\n",ctime(&t),message);
                printMessage(message_print);
                bzero(message_print, 1024);

              
              
                pthread_mutex_lock(&mutex_count);
                count_responses++;
                pthread_mutex_unlock(&mutex_count);
                
                int port;
                char ip[10];
                int result = 0;
                if(city == NULL){
                  for(int j = 0 ; j < portservant_size ; j++){
                    port = portservant[j].port_number;
                    strcpy(ip,portservant[j].IP);
                    //fprintf(stdout, "%s\n", new_message); 
                    result += connect_to(ip, port, new_message);

                    
                    
                  } 
                  write(fd_server, &result, sizeof(int));
                  
                  time(&t);
                  sprintf(message_print + strlen(message_print),"[%.19s] Response received: %d, forwarded to client\n",ctime(&t),result);
                  printMessage(message_print);
                  bzero(message_print, 1024);
                  
                } 
                else{
                  // perror("elsedeyim");

                  time(&t);
                  sprintf(message_print + strlen(message_print),"[%.19s] Request arrived %s.\nContacting servant %d\n",ctime(&t),message, pid);
                  printMessage(message_print);
                  bzero(message_print, 1024);
                  for(int i=0 ; i < city_size ; i++){
                    
                    
                    if(strcmp(city_names[city_size-i-1],city) == 0){
                      for(int j = 0 ; j < portservant_size ; j++){
                        if(portservant[j].begin <= i && portservant[j].end > i){
                          port = portservant[j].port_number;
                          strcpy(ip ,portservant[j].IP);
                          break;
                        } 

                      }
                    }
                  }
                  
                  result = connect_to(ip, port, new_message);
                  
                  write(fd_server, &result, sizeof(int));

                  time(&t);
                  sprintf(message_print + strlen(message_print),"[%.19s] Response received: %d, forwarded to client\n",ctime(&t),result);
                  printMessage(message_print);
                  bzero(message_print, 1024);

              }
                }      else if(realEstate->choice == 2){
                        

                        int y = 0;
                        while(realEstate->dataset[y] != '\0'){
                          portservant[portservant_size].dataset[y] = realEstate->dataset[y];  
                          y++;
                        }



                        if(read(fd_server, message, 400) == -1){
                          perror("error occures reading from client to server!!");
                          exit(EXIT_FAILURE);
                        }
                        
                        read(fd_server, &pid ,sizeof(int));

                        pid = realEstate->servant_pid;
                        begin = realEstate->begin;
                        end = realEstate->end;
                        y = 0;
                        
                        while(realEstate->ip[y] != '\0'){
                          portservant[portservant_size].IP[y] = realEstate->ip[y];  
                          y++;
                        }
                        //printf("%s\n",portservant[portservant_size].IP);
                        
                        int port = atoi(argument.PORT) + begin + end;

                        portservant[portservant_size].begin = begin;
                        portservant[portservant_size].end = end;
                        portservant[portservant_size].port_number = port;
                        


                        if(flag_dataset == 1){
                            struct dirent **entry;
                            DIR *dp;

                            dp = opendir(portservant->dataset);
                            if (dp == NULL) 
                            {
                              perror("opendir");
                              exit(EXIT_FAILURE);
                            }
                            int n;
                            city_names = malloc(10*sizeof(char**));
                            n = scandir(portservant->dataset  , &entry, NULL, alphasort);

                            int big = 1;
                            int count =0;
                            while(n--){
                              if(strcmp(entry[n]->d_name, ".") != 0 && strcmp(entry[n]->d_name, "..") != 0){
                                
                                city_names[count] = malloc(1+strlen(entry[n]->d_name)*sizeof(char));
                                strcpy(city_names[count] ,entry[n]->d_name);
                                count++;
                                if(count >= 10*big*0.90){
                                    big++;
                                    
                                    city_names = realloc(city_names,10*big*sizeof(char*) );
                                    
                                }

                                free(entry[n]);      
                                
                                
                              }else{
                                free(entry[n]);
                              }
                              
                            }
                            city_size = count;
                            closedir(dp);
                            flag_dataset = 0;

                          }
                      }

                      
                      time(&t);

                      char message_print[1024];
                      sprintf(message_print + strlen(message_print),"[%.19s] Servant %d present at port %d handling cities %s-%s.\n",ctime(&t),pid,portservant[portservant_size].port_number,city_names[city_size-begin-1], city_names[city_size-end-1]);
                      printMessage(message_print);
                      bzero(message_print, 1024);

                      portservant_size++; 

                      pthread_mutex_lock(&mutex_available);
                      available++;

                      pthread_cond_signal(&cond_available);
                      pthread_mutex_unlock(&mutex_available);
    
  }



}





int main(int argv, char* argc[]){

    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

    int _p_ = 0, _t_ = 0;
    int c;

    while((c = getopt(argv, argc, "p:t:")) != -1){
      switch(c)
      {
        case 'p':
            argument.PORT = optarg;
            _p_ = 1;
            break;
        case 't':
            argument.Number_of_Threads = atoi(optarg);
            _t_ = 1;
            break;
        default:
            break;
      }
    }
    if((!_p_) || (!_t_)){
      perror("No input File");
      exit(EXIT_FAILURE);
   }


    

    pthread_t thread[argument.Number_of_Threads];
    int thread_no[argument.Number_of_Threads];
      
    for(int i=0; i < argument.Number_of_Threads ; i++){
      thread_no[i] = i+1;
      
      if((pthread_create(&thread[i],NULL, thread_pool, (void*) &thread_no[i])) !=0 ){
        perror("creating thread error!");
        exit(EXIT_FAILURE);
      }
      
    }

    int fd ;
    if((fd= socket(AF_INET, SOCK_STREAM, 0)) == -1){
      fprintf(stderr,"socket failed!!");
      exit(EXIT_FAILURE);
    }

    int option = 1;

    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option , sizeof(option)) == -1){
      close(fd);
      fprintf(stderr, "setsocokopt error!!\n");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argument.PORT));
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(fd, (struct sockaddr*)(&addr), sizeof(addr)) == -1){
      close(fd);
      fprintf(stderr, "bind error!!\n");
      exit(EXIT_FAILURE);
    }

    if(listen(fd, 200) < 0){
      close(fd);
      fprintf(stderr," listen error!!!");
      exit(EXIT_FAILURE);
    }

    int addr_length = sizeof(addr);
  
    
    int fd_accept;
    
    while(1){
      //perror("bekliyorum accepti");
      fd_accept = accept(fd, (struct sockaddr*)(&addr), (socklen_t*)(&addr_length));
      //perror("accept geldi");
      if(fd_accept == -1){
        fprintf(stderr, "accept error");
        break;
      }
      //printf("%d\n", fd_accept);
        
      
      pthread_mutex_lock(&mutex_available);
      while(available == 0){
        perror("available lock icindeyim\n");
        pthread_cond_wait(&cond_available,&mutex_available);
      } 
      available--;
      
      pthread_mutex_unlock(&mutex_available);

      pthread_mutex_lock(&mutex_queue);

      offer_int(&queue, fd_accept);
      size_queue++;
      pthread_cond_broadcast(&cond_queue);

      pthread_mutex_unlock(&mutex_queue);

      //perror("mustafa");
      
      
    }


    
    // for(int i=0 ; i < argument.Number_of_Threads ; i++){
      
    //   if((pthread_join(thread[i+1], NULL)) != 0){
    //     perror("joining thread error");
    //     exit(EXIT_FAILURE);
    //   }
    // }



    
    
    return 0;
}