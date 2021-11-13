#ifndef __HTTP_WEATHER_H__
#define __HTTP_WEATHER_H__

typedef struct 
{
    char weather[20];
    char temperature[5];
}HttpRequest_Weather;
extern HttpRequest_Weather Http_Weather;


HttpRequest_Weather Get_Http_Weather(void);
char* Get_Http_Weather_W(void);
char* Get_Http_Weather_T(void);

void HTTP_Weather_Init(void);

#endif
