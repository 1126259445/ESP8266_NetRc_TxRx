#ifndef __DHT11_H__
#define __DHT11_H__

#define DHT11_IO GPIO_NUM_5
#define GPIO_DHT11_PIN_SEL  (1ULL<<DHT11_IO)

typedef struct 
{
    float Humidity;
    float Temperature;
}Dev_Dht11;
extern Dev_Dht11 Dht11;


Dev_Dht11 Get_Dht11_Data(void);
int Get_Dht11_Humidity(void);
int Get_Dht11_Temperature(void);

void Dht11_Init(void);
uint8_t Read_Dht11_Data(void);
#endif
