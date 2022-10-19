
#include "client.h"

char** buffer_token;
int count = 0;
int count_line;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int arrived=0;

ARGUMENT argument;

void printMessage(char *message){
    while(((write(STDERR_FILENO, message, strlen(message))) == -1) && (errno == EINTR));
    
}

char** message_array;

struct RealEstateData* addLast_given_message(char* buffer){


    char start_day[100];
    char end_day[100];
  
    struct RealEstateData *newNode = malloc(sizeof(struct RealEstateData));
   
   
    newNode->day_end = 0;
    newNode->day_start = 0;
    newNode->month_end = 0;
    newNode->month_start = 0;
   
    newNode->year_end = 0;
    newNode->year_start = 0;

    
    char* context;
    char* token = strtok_r(buffer, " ", &context);
    
    while(token != NULL){ 

        newNode->socket_fd = atoi(token); 

        token = strtok_r(NULL, " ", &context);

        if(token == NULL){
          perror("Invalid request!");
          exit(EXIT_FAILURE);
        }

        strcpy(newNode->transaction_id,token);

        token = strtok_r(NULL, " ", &context);

        if(token == NULL){
          perror("Invalid request!");
          exit(EXIT_FAILURE);
        }

        strcpy(newNode->real_estate,token);

        token = strtok_r(NULL, " ", &context);

        
        if(token == NULL){
          perror("Invalid request!");
          exit(EXIT_FAILURE);
        }

        strcpy(start_day,token);
        
        token = strtok_r(NULL, " ", &context);

        if(token == NULL){
          perror("Invalid request!");
          exit(EXIT_FAILURE);
        }

        strcpy(end_day,token);

        token = strtok_r(NULL, " ", &context);

        if(token == NULL){
          
          break;
        }

        strcpy(newNode->cityname,token);
        
        token = strtok_r(NULL, " ", &context);
        break;
        if(token != NULL){
          perror("Invalid request!");
          exit(EXIT_FAILURE);
        }
        
      

    }

}

void* function(void* arg){

  time_t t;
  time(&t);
  
  struct RealEstateData* realEstate = ((struct RealEstate*)arg);
  
  int my_id = realEstate->pid;
  char message[512] ;
  strcpy(message, message_array[my_id]);
  
  char message_print[1024];
  sprintf(message_print,"[%.19s] Client-Thread-%d: Thread-%d has been created.\n",ctime(&t),my_id,my_id);
  printMessage(message_print);
  bzero(message_print, 1024);
  if(pthread_mutex_lock(&mutex) != 0){
    perror("error locking mutex");
    exit(EXIT_FAILURE);
  }
  ++arrived;

  if(arrived < count_line){
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
  
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argument.PORT));

  int length_struct = 0;

  if(inet_aton(argument.IP, &addr.sin_addr) == 0){
    fprintf(stderr, "ipv4 address is invalid!!\n");
    exit(EXIT_FAILURE);
  }

  
  
  int fd;
  
  fd = socket(AF_INET, SOCK_STREAM, 0);
 
 

  if((connect(fd, (struct sockaddr*)&addr, sizeof(addr))) == -1){
    close(fd);
    fprintf(stderr, "connection failed!!\n");
    exit(EXIT_FAILURE);
  }

  time(&t);
  //sprintf(message_print,"[%.19s] Client-Thread-%d: I am requesting \"%s\".\n",ctime(&t),my_id,message);
  printMessage(message_print);
  bzero(message_print, 1024);


  length_struct = strlen(message) + 1;
  int choice = 1;

  // write(fd, &choice, sizeof(int));
  
  realEstate->choice = 1;
  // write(fd, &length_struct, sizeof(int));
  
  write(fd, realEstate, sizeof(realEstate)) ;
  
  
  int result = 0;
  if(read(fd, &result, sizeof(int)) == -1){
    fprintf(stderr, "reading error on server");
    exit(EXIT_FAILURE);
  }

  
  time(&t);
  sprintf(message_print,"[%.19s] Client-Thread-%d: The server’s response to \"%s\" is %d.\n",ctime(&t),my_id,message, result);
  printMessage(message_print);
  bzero(message_print, 1024);



  time(&t);
  sprintf(message_print,"[%.19s] Client-Thread-%d: Terminating.\n",ctime(&t),my_id);
  printMessage(message_print);
  bzero(message_print, 1024);
  
  return NULL;
}


int main(int argv, char* argc[]){

    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &sigterm_handler;

    if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
      perror("sigaction");
    }

    int _r_ = 0, _q_ = 0, _s_ = 0;
    int c;

    


    while((c = getopt(argv, argc, "r:q:s:")) != -1){
      switch(c)
      {
        case 'r':
            argument.requestFile = optarg;
            _r_ = 1;
            break;
        case 'q':
            argument.PORT = optarg;
            _q_ = 1;
            break;
        case 's':
            argument.IP = optarg;
            _s_ = 1;
            break;
        default:
            break;
      }
    }
    if((!_r_) || (!_q_) || (!_s_)){
      perror("No input File");
      exit(EXIT_FAILURE);
   }

    
    
    int fd = open(argument.requestFile,O_RDONLY , 0644);
    
    size_t bytes_read;

    if (fd == -1){
      perror("fd failed");
      exit(EXIT_FAILURE);
    }    
    int size = 50;
    char* buffer = malloc(size * sizeof(char*));

    
    int countline = 0;
    strcpy(buffer,"\0");
    do{ 
        
        bytes_read = read(fd, buffer, sizeof(buffer));
        
        count += bytes_read;
        countline++;
    }while(bytes_read == sizeof(buffer));

    char *token,*tokenPt;
    char* buffer_new = calloc(count,sizeof(char*));
    char* temp = calloc(50,sizeof(char*));
    fd = open(argument.requestFile,O_RDONLY , 0644);
    
    

    int line = 0;
    for(int i=0 ; i<count ; i++){
      if(buffer_new[i] == '\n' || buffer_new[i] == '\r' || buffer_new[i] == '\0'){
        line++;
      }
    }



    
    count_line = 0 ;

    
    buffer_token = calloc(line,sizeof(char*));
    
    
    

    
    pthread_t thread [count];
    
    while(token != NULL){ 

        
        buffer_token[count_line] = strdup(token);

        token = strtok(NULL, "\n");
        count_line++;

    }


    
    time_t t;
    time(&t);

    char message_print[1024];
    sprintf(message_print + strlen(message_print),"[%.19s] Client: I have loaded %d requests and I’m creating %d threads.\n",ctime(&t),count_line,count_line);
    printMessage(message_print);
    bzero(message_print, 1024);
    
    int id = 0;
    message_array = calloc(count_line, sizeof(char**));
    

     
    for(int i=0 ; i<count_line ; i++){
      
      //message_array[i] = strdup(buffer_token[i]);

      struct RealEstateData* realEstate = addLast_given_message(buffer_token[i]);
      id = i+1;
      realEstate[i].pid = i+1;
      
      if((pthread_create(&thread[i],NULL, function, (void*)realEstate)) !=0 ){
        perror("creating thread error!");
        exit(EXIT_FAILURE);
      }
       

    }


    

    for(int i=0 ; i < count_line ; i++){
      
      if((pthread_join(thread[i], NULL)) != 0){
        perror("joining thread error");
        exit(EXIT_FAILURE);
      }
    }

    sprintf(message_print + strlen(message_print),"[%.19s] Client: All threads have terminated, goodbye.\n",ctime(&t));
    printMessage(message_print);
    bzero(message_print, 1024);




    return 1;

}