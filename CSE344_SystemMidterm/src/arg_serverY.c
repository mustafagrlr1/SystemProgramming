#include "arg_serverY.h"


void argument(int argc, char *argv[]){
    int _s_ = 0;                                                /* The place that I am implementing getopt function     */
    int _o_ = 0;                                                /* Generally I got them to a char pointer               */
    int _p_ = 0;                                                /* There are certain character we need from command line*/
    int _r_ = 0;
    int _t_ = 0;
    int c;
    
    while((c = getopt(argc, argv, "s:o:p:r:t:")) != -1){
        switch (c)
        {
        case 's':
            pathToServerFifo = optarg;
            _s_ = 1;
            break;
        
        case 'o':
            pathToLogFile = optarg;
            _o_ = 1;
            break;
        
        case 'p':
            poolSize = atoi(optarg);
            _p_ = 1;
            break;
        
        case 'r':
            poolSize2 = atoi(optarg);
            _r_ = 1;
            break;

        case 't':
            if(optarg==NULL){
                fprintf(stderr, "invalid command argument!! %s", *argv);
            }
            _time = atoi(optarg);
            _t_ = 1;
            break;

        default:
            break;
        }
    }


    if((!_s_) || (!_o_) || (!_p_) || (!_r_) || (!_t_)                /*If flags are zero wrong command line error*/
                       ){

        fprintf(stderr, "invalid command argument not take all parameterrs!! %s", *argv);
        exit(EXIT_FAILURE);
    }
}

