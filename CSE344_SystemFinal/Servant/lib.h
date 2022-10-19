#ifndef _LIB_H
#define _LIB_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "servantLib.h"
#include "../Helper/helper.h"

int* splitDay(char* day);

void setCityRangeDataSize(CityRangeData,int);
City getNextCity(CityRangeData);
void setNextCity(CityRangeData, City);

char* splitCityRangeString(char* );
int* splitCityRange(char*, int*);

void sigterm_handler(int signo);

CityRangeData* read_directory_path(ARGUMENT);

#endif