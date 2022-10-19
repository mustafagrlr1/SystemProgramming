#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"

#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE)+20)



struct request{         /*Request Client TO Server */ 
    pid_t pid;  
    int message[1024];    
    int seqLen;
    int busy_n;
};

struct response{        /*Response TO Client */
    int resValue;
    pid_t pid;
};