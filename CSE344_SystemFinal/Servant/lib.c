#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

#include "lib.h"

volatile sig_atomic_t sigtermInterrupt = 0;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		sigtermInterrupt = 1;
}

time_t t;

void printMessage(char *message){
    while(((write(STDERR_FILENO, message, strlen(message))) == -1) && (errno == EINTR));
    
}

char* splitCityRangeString(char* city_range){
    return strtok(city_range, "-");
}

int* splitCityRange(char* city_range, int* cityRange){
    char* token = splitCityRangeString(city_range);
    int count = 0;
    while(token != NULL){
        
        cityRange[count] = atoi(token);
        token = strtok(NULL, "-");
        count++;
    }
    if(count != 2){
        perror("error splitting city range is invalid!!");
        exit(EXIT_FAILURE);
    }
  
    return cityRange;
}


struct CityRangeData* cityRangeData = NULL;




void addLast(struct CityRangeData **head,char* cityname,int* arr){
    
    

    struct CityRangeData *newNode = malloc(sizeof(struct CityRangeData));
    newNode->cityName = NULL;
    newNode->city = NULL;
    newNode->next = NULL;
    newNode->range_begin = 0;
    newNode->range_end = 0;


    newNode->cityName = strdup(cityname);
    
    // calloc((1 + strlen(cityname)),sizeof(char*));
    // newNode->cityName = NULL;

    // strcat(newNode->cityName,cityname);
    newNode->range_begin = arr[0];
    newNode->range_end = arr[1];
    
    if(*head == NULL){
      *head = newNode;
    }else{
      struct CityRangeData *lastNode = *head;

      while(lastNode->next != NULL){
        lastNode = lastNode->next;
      }
      lastNode->next = newNode;
    }      

    
   
    
}

void addLastCity(struct City **head,char* citynamedate){
    

    int date[3];

    struct City *newNode = malloc(sizeof(struct City));
    newNode->cityRealEstateData = NULL;
    newNode->dateName = NULL;
    newNode->day = 0;
    newNode->month = 0;
    newNode->next = NULL;
    newNode->year = 0;
    
    newNode->dateName = strdup(citynamedate);
    
    char* dump = strdup(citynamedate);
    char* token = strtok(dump, "-");
    
    int count = 0;
    while(token != NULL){
        
        date[count] = atoi(token);
        token = strtok(NULL, "-");
        count++;
    }
    if(count != 3){
      perror("date is incorrect!");
      exit(EXIT_FAILURE);
    }
    newNode->day = date[0];
    newNode->month = date[1];
    newNode->year = date[2];
    
    
    if(*head == NULL){
      *head = newNode;
      
    }else{  
      struct City *lastNode = *head;

      while(lastNode->next != NULL){
        lastNode = lastNode->next;
      }
      lastNode->next = newNode;
      
    }      

    free(dump);
    // free(newNode->dateName);
    // free(newNode);

    
}

void addLastToCityDate(struct City** city, ARGUMENT argument, char* cityName){

    
    struct dirent **entry_city;
    
    char* path_inside_city = calloc(strlen(argument.directoryPath)+ strlen(cityName) + 1 , sizeof(path_inside_city));


    strcpy(path_inside_city, argument.directoryPath);
    strcat(path_inside_city, "/");
    strcat(path_inside_city, cityName);
    

    char** city_names_date = malloc(10*sizeof(char**));
    
    int n = scandir(path_inside_city  , &entry_city, NULL, alphasort);

    int big = 1;
    int count_city_date =0;
    while(n--){
      if(strcmp(entry_city[n]->d_name, ".") != 0 && strcmp(entry_city[n]->d_name, "..") != 0){
  
        city_names_date[count_city_date] = strdup(entry_city[n]->d_name);
        count_city_date++;
        if(count_city_date >= 10*big*0.90){ 
            big++;
            
            city_names_date = realloc(city_names_date,10*big*sizeof(char*) );
            
        } 

        free(entry_city[n]);      
        
      }else{
        free(entry_city[n]);
      }
    }
    
    
    *city = NULL;
  
    for(int i=0 ; i<count_city_date ; i++){
      addLastCity(city ,city_names_date[count_city_date-i-1]);
    }

    for(int i=0 ; i<count_city_date ; i++){
      free(city_names_date[i]);
    }
    free(city_names_date);
    free(entry_city);
     free(path_inside_city);
    
}



void addCityDate(ARGUMENT argument, struct CityRangeData** head){


    if(*head == NULL){
      perror("City is Null!!");
      exit(EXIT_FAILURE);
    }else{
      struct CityRangeData* lastNode = *head;

      while (lastNode->next != NULL)
      {
         addLastToCityDate(&(lastNode->city), argument, lastNode->cityName);
         lastNode = lastNode->next;
      }
      
    }

}

void addLastTransactionForm(struct CityRealEstateData** head, char* buffer_new){
    
    struct CityRealEstateData *newNode = malloc(sizeof(struct CityRealEstateData));
    newNode->next = NULL;
    newNode->price = 0;
    newNode->real_estate =NULL;
    newNode->street_real_estate = NULL;
    newNode->surface_per_square = 0;
    newNode->transaction_id = 0;
    char* token = strtok(buffer_new, " ");
    
    while(token != NULL){ 
        


        newNode->transaction_id = atoi(token);
        token = strtok(NULL, " ");
        
        newNode->real_estate = strdup(token);
        token = strtok(NULL, " ");
        
        newNode->street_real_estate = strdup(token);
        token = strtok(NULL, " ");

        newNode->surface_per_square = atoi(token);
        token = strtok(NULL, " ");

        newNode->price = atoi(token);
        token = strtok(NULL, " ");
        
        if(token != NULL){
          perror("more value inside of dates!!!");
          exit(EXIT_FAILURE);
        }


    }

    
    if(*head == NULL){
      *head = newNode;
    }else{ 
      struct CityRealEstateData *lastNode = *head;
      
      while(lastNode->next != NULL){
        
        lastNode = lastNode->next;
      }
      lastNode->next = newNode;
      
    }
   
    // free(newNode->real_estate);
    // free(newNode->street_real_estate);
    // free(newNode);  
    
}

void addCityDateFile_Transactions_from_cityLast(struct CityRealEstateData** cityRealEstate, ARGUMENT argument, char* cityName, char* realEstateDate){


    char* path_inside_city = malloc((3 + strlen(argument.directoryPath) + strlen(cityName) + strlen(realEstateDate)) * sizeof(char));
    
    strcpy(path_inside_city, argument.directoryPath);
    strcat(path_inside_city, "/");
    strcat(path_inside_city, cityName);
    strcat(path_inside_city, "/");
    strcat(path_inside_city, realEstateDate);

    
    int fd = open(path_inside_city,O_RDONLY , 0644);
    
    size_t bytes_read;
    if (fd == -1){
      perror("fd failed");
      exit(EXIT_FAILURE);
    }
    int size = 50;
    char* buffer = malloc(size * sizeof(char*));


    int count = 0;
    strcpy(buffer,"\0");
    do{ 
        
        bytes_read = read(fd, buffer, sizeof(buffer));
        
        count += bytes_read;
    }while(bytes_read == sizeof(buffer));

    
   
    close(fd);

    char* buffer_new = calloc(count+1,sizeof(char*));
    char* temp = calloc(50,sizeof(char*));
    fd = open(path_inside_city,O_RDONLY , 0644);
    
    
    do{ 
      
        
        bytes_read = read(fd, temp, sizeof(temp));
        
        
        strcat(buffer_new, temp);
        
        
    }while(bytes_read == sizeof(buffer));
    
    int line = 0;
    for(int i=0 ; i<count ; i++){
      if(buffer_new[i] == '\n' || buffer_new[i] == '\r' || buffer_new[i] == '\0'){
        line++;
      }
    }


    char* token = strtok(buffer_new, "\n");
    
    int count_line = 0 ;

    (*cityRealEstate) = NULL;
    
    
    char** buffer_token = calloc(line,sizeof(char*));
    
    
    while(token != NULL){ 

        
        buffer_token[count_line] = strdup(token);

        token = strtok(NULL, "\n");
        count_line++;

    }
    
    
    for(int i=0 ; i<line-1 ; i++){
      
      addLastTransactionForm(cityRealEstate, buffer_token[i]);

    }

    
    for(int i=0 ; i<count_line ; i++){
      free(buffer_token[i]);
    }
    free(buffer_token);
    free(buffer_new);
    free(temp);
    free(buffer);
    free(path_inside_city);

}

void addCityDateFile_Transactions_from_city(struct City** city, ARGUMENT argument, char* cityName){

  if(*city == NULL){
    perror("City is NULL log out!!!");
  }else{
    struct City* lastNode = *city;
    
    while(lastNode!= NULL){
        
        addCityDateFile_Transactions_from_cityLast(&(lastNode->cityRealEstateData), argument, cityName, lastNode->dateName);
        lastNode = lastNode->next;
    }
  }

}

void addCityDateFile_Transactions(ARGUMENT argument, struct CityRangeData** head){

    if(*head == NULL){
      perror("City is Null!!");
      exit(EXIT_FAILURE);
    }else{
      struct CityRangeData* lastNode = *head;
      
      while (lastNode->next != NULL)
      {
          
         addCityDateFile_Transactions_from_city(&(lastNode->city), argument, lastNode->cityName);
         lastNode = lastNode->next;
      }
      
      
    }
    
}

//     int start_date[3];

//     token = strtok_r(start_day, "-", &context);
//     int count_day_start = 0;
//     while(token != NULL){
        
//         start_date[count_day_start] = atoi(token);
//         token = strtok_r(NULL, "-", &context);
//         count_day_start++;
//     }

//     newNode->day_start = start_date[0];
//     newNode->month_start = start_date[1];
//     newNode->year_start = start_date[2];

//     int end_date[3];

//     token = strtok_r(end_day, "-", &context);
//     int count_day_end = 0;
//     while(token != NULL){
        
//         end_date[count_day_end] = atoi(token);
//         token = strtok_r(NULL, "-", &context);
//         count_day_end++;
//     }

//     newNode->day_end = end_date[0];
//     newNode->month_end = end_date[1];
//     newNode->year_end = end_date[2];
    
//     return newNode;
      
// }

int traverse_realestate(struct CityRealEstateData** data, struct RealEstateData* node, char* cityname){
  struct CityRealEstateData* it = *data;

  int result = 0;
  
  while(it != NULL){
    if(strcmp(it->real_estate, node->real_estate) == 0){
      
      result = result + 1;
      
    }
    it = it->next;
  }
  
  return result;
}



int traverse_city(struct City** data, struct RealEstateData* node, char* cityname){
  struct City* it = *data;
  int result = 0;

  
  while(it != NULL){
    if(node->year_start < it->year && node->year_end > it->year ){
      struct CityRealEstateData* realEstate = it->cityRealEstateData;
      result =  result + traverse_realestate(&realEstate, node, cityname);
    }else if(node->year_start == it->year || node->year_end == it->year){
      if(node->month_start < it->month && node->month_end > it->month){
        struct CityRealEstateData* realEstate = it->cityRealEstateData;
        result =  result + traverse_realestate(&realEstate, node, cityname);

      }else if(node->month_start == it->month || node->month_end == it->month){
        if(node->day_start < it->day && node->day_end > it->day){
          struct CityRealEstateData* realEstate = it->cityRealEstateData;
          result =  result + traverse_realestate(&realEstate, node, cityname);
        }else{
          if(node->day_start == it->day || node->day_end == it->day){
            struct CityRealEstateData* realEstate = it->cityRealEstateData;
            result =  result + traverse_realestate(&realEstate, node, cityname);
          }
        }
      }
    }
  
    
    it = it->next;
  }
  return result;
}

int traverse(struct CityRangeData** data, struct RealEstateData* node){
  
  struct CityRangeData* it = *data;
  int result = 0;

  
  
  if(node->cityname != NULL){
  
    while(it != NULL){
      
      if(strcmp(node->cityname,it->cityName) == 0){
        struct City* city = it->city;
        
      
        
        result = result + traverse_city(&city, node, it->cityName); 
          
        return result;
        
      }
     
      it = it->next;

    }
    
  }else{
    while(it != NULL){
      struct City* city = it->city;
      result = result + traverse_city(&city, node, NULL);
      it = it->next;
    }
   

  }

  return result;
}

pid_t get_pid_from_proc_self(){
    char target[32];
    int pid;
    readlink("/proc/self",target,sizeof(target));
    sscanf(target,"%d",&pid);
    return (pid_t) pid;
}


void* function(void* arg){
  struct RealEstateData* node = ((struct RealEstateData*)arg);

  
  int result = traverse(&cityRangeData, node);
  int socket = node->socket_fd;
  
  write(socket, &result, sizeof(int));

  return NULL;

}


CityRangeData* read_directory_path(ARGUMENT argument){
    
    
    struct dirent **entry;
    DIR *dp;

    dp = opendir(argument.directoryPath);
    if (dp == NULL) 
    {
      perror("opendir");
      exit(EXIT_FAILURE);
    }
    int n;
    char** city_names = malloc(10*sizeof(char**));
    n = scandir(argument.directoryPath  , &entry, NULL, alphasort);

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
    closedir(dp);
    
    int* cityrange = malloc(2*sizeof(int)); 
    cityrange = splitCityRange(argument.city_range, cityrange);
    
    cityrange[0] -= 1;
    cityrange[1] -= 1;
    
    char message_print[1024];
    int pid = (int)get_pid_from_proc_self();

    
    for(int i=cityrange[0] ; i<cityrange[1] + 1 ; i++){
      addLast(&cityRangeData,city_names[count-i-1],  cityrange);  
    }
    
    addCityDate(argument, &cityRangeData);

    addCityDateFile_Transactions(argument, &cityRangeData);
  

    time(&t);
    sprintf(message_print + strlen(message_print),"[%.19s] Servant %d loaded dataset, cities %s-%s\n",ctime(&t),pid, city_names[count-cityrange[0]-1], city_names[count-cityrange[1]-1]);
    printMessage(message_print);
    bzero(message_print, 1024);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argument.PORT));


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

    int size_of_directory = sizeof(argument.directoryPath);
    //int size_of_IP = sizeof(argument.IP);
    
    struct RealEstateData* realEstate;

    realEstate->choice = 2;

    realEstate->begin = cityRangeData->range_begin;

    realEstate->end = cityRangeData->range_end;

    strcpy(realEstate->dataset, argument.directoryPath);

    strcpy(realEstate->ip, argument.IP);

    realEstate->servant_pid = pid;


    
    close(fd);
  

    char message[1024];
 
    if((fd= socket(AF_INET, SOCK_STREAM, 0)) == -1){
      fprintf(stderr,"socket failed!!");
      exit(EXIT_FAILURE);
    }

    int option = 1;

    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option , sizeof(option)) == -1){
      close(fd);
      fprintf(stderr, "setsocokopt error!!\n");
    }

    struct sockaddr_in addrk;
    addrk.sin_family = AF_INET;
    addrk.sin_port = htons(atoi(argument.PORT) +  cityRangeData->range_begin+cityRangeData->range_end);
    addrk.sin_addr.s_addr = INADDR_ANY;

    if(bind(fd, (struct sockaddr*)(&addrk), sizeof(addrk)) == -1){
      close(fd);
      fprintf(stderr, "bind error!!\n");
      exit(EXIT_FAILURE);
    }

    if(listen(fd, 200) < 0){
      close(fd);
      fprintf(stderr," listen error!!!");
      exit(EXIT_FAILURE);
    }

    int addr_length = sizeof(addrk);
  
    
    int fd_accept;
    bzero(message,1024);

    
    
    pthread_t thread[100];
    int thread_count = 0;
    char socket_fd[1024];
    bzero(socket_fd, 1024);
    
    while(1){
      time(&t);
      sprintf(message_print + strlen(message_print),"[%.19s] Servant %d listening at %d\n",ctime(&t),pid,atoi(argument.PORT) +  cityRangeData->range_begin+cityRangeData->range_end );
      printMessage(message_print);
      bzero(message_print, 1024);
      fd_accept = accept(fd, (struct sockaddr*)(&addrk), (socklen_t*)(&addr_length));
      if(fd_accept == -1){
        fprintf(stderr, "accept error");
        break;
      }

      
      bzero(socket_fd, 1024);
      bzero(message, 1024);
      struct RealEstateData* node = calloc(1, sizeof(node));
      if(read(fd_accept, node, sizeof(node)) == -1){
        perror("reading fault!!");
      }


      node->socket_fd = fd_accept;
      
      
      if((pthread_create(&thread[thread_count],NULL, function, (void*) node)) !=0 ){
        perror("creating thread error!");
        exit(EXIT_FAILURE);
      }

       
      thread_count++;
      
      
    }
    
    for(int i=0 ; i < thread_count ; i++){
      
      if((pthread_join(thread[i], NULL)) != 0){
        perror("joining thread error");
        exit(EXIT_FAILURE);
      }
    }
    
    while(cityRangeData){
      struct CityRangeData* tmp = cityRangeData;
      cityRangeData = cityRangeData->next;
      if(tmp == NULL){
        break;
      }
      struct City* city = tmp->city;
      while(city){
        struct City* temp = city;
        city = city->next;
        struct CityRealEstateData* realEstate = temp->cityRealEstateData;
        while(realEstate){
          struct CityRealEstateData* tempreal = realEstate;
          realEstate = realEstate->next;
          free(tempreal->street_real_estate);
          free(tempreal->real_estate);
          free(tempreal); 
        }
        free(temp->dateName);
        free(temp); 
      } 

      free(tmp->cityName);
      free(tmp);
      
    }
    for(int i=0 ; i<count ; i++){
      free(city_names[i]);
    }
    free(cityRangeData);
    free(cityrange);
   
    free(city_names);
    free(entry);
    return NULL;

}