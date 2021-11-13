#ifndef __HTTP_TIME_H__
#define __HTTP_TIME_H__

typedef struct 
{
    char sysTime2[30];
    char sysTime1[20];
}HttpRequest_Time;
extern HttpRequest_Time Http_Time;


typedef struct 
{
   uint32_t years;
   uint8_t month;
   uint8_t day;
   uint8_t hours;
   uint8_t minute;
   uint8_t second;
}Http_int_Time_t;
extern Http_int_Time_t Http_int_Time;

HttpRequest_Time Get_Http_Time(void);
char* Get_Http_Time1(void);
char* Get_Http_Time2(void);
Http_int_Time_t Get_Http_Int_Time(void);
uint32_t Get_Http_Int_Years(void);
uint8_t Get_Http_Int_Mounth(void);
uint8_t Get_Http_Int_Day(void);
uint8_t Get_Http_Int_Hours(void);
uint8_t Get_Http_Int_Minuter(void);
uint8_t Get_Http_Int_Second(void);

void HTTP_Time_Init(void);
#endif
