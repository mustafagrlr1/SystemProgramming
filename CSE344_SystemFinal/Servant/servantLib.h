#ifndef _SERVANTLIB_H_
#define _SERVANTLIB_H_

#include <signal.h>


typedef struct ARGUMENT{
    char* directoryPath;
    char* city_range;
    char* IP;
    char* PORT;
}ARGUMENT;


#define MAX_CITY 81
#define MAX_REAL_ESTATE 10
#define MAX_CITY_DATES 10



typedef struct CityRangeData{
    char* cityName;
    struct City* city;
    int range_begin;
    int range_end;
    struct CityRangeData* next;
}CityRangeData;

typedef struct City{
    char* dateName;
    int day;
    int month;
    int year;
    struct City* next;
    struct CityRealEstateData* cityRealEstateData;
}City;


typedef struct CityRealEstateData{
    int transaction_id;
    char* real_estate;
    char* street_real_estate;
    int surface_per_square;
    int price;    
    struct CityRealEstateData* next;
}CityRealEstateData;



#endif