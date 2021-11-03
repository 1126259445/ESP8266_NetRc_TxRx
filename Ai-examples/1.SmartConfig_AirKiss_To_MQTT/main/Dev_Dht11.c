#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_log.h"
#include "esp_system.h"

#include "Dev_Dht11.h"


Dev_Dht11 Dht11;

/*******************Data interface****************/
Dev_Dht11 Get_Dht11_Data()
{
    return Dht11;
}

int Get_Dht11_Humidity()
{
    return (int)(Dht11.Humidity *10);
}

int Get_Dht11_Temperature()
{
    return (int)(Dht11.Temperature *10);
}


#if 0
// 毫秒延时函数
//===========================================
void  delay_ms(uint32_t C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
#else
void delay_ms(uint32_t delay_ms)
{
   vTaskDelay(delay_ms/portTICK_PERIOD_MS);
}
#endif

/*******************Data read*********************/

//DHT11端口复位，发出起始信号（IO发送）
static void DHT11_rest (void)
{ 
	gpio_set_direction(DHT11_IO, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_IO, 0);
	delay_ms(20); //拉低至少18ms						
    gpio_set_level(DHT11_IO, 1);							
	os_delay_us(5); 
}

//等待DHT11回应，返回1:未检测到DHT11，返回0:成功（IO接收）
uint8_t DHT11_check(void)
{ 	   
    uint8_t retry=0;
    gpio_set_direction(DHT11_IO, GPIO_MODE_INPUT);//IO到输入状态
     while(gpio_get_level(DHT11_IO)&&retry<100)//等待变为低电平
    {
        retry++;
        os_delay_us(1);
    }
    if(retry>=100)return 1; else retry=0;

    while(!gpio_get_level(DHT11_IO)&&retry<100)//等待变高电平
    {
        retry++;
        os_delay_us(1);
    }
    if(retry>=100)return 1;	    
    return 0;
}

static uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry=0;
    while(gpio_get_level(DHT11_IO)&&retry<100)//等待变为低电平
    {
        retry++;
        os_delay_us(1);
    }
    retry=0;
    while(!gpio_get_level(DHT11_IO)&&retry<100)//等待变高电平
    {
        retry++;
        os_delay_us(1);
    }
    os_delay_us(40);//等待 40us
    if(gpio_get_level(DHT11_IO))return 1;else return 0;
}

//从DHT11读取一个字节  返回值：读到的数据
uint8_t Dht11_ReadByte(void)
{  
    uint8_t i = 0,dat = 0;
    for (i=0;i<8;i++){ 
        dat<<=1; 
        dat|=DHT11_Read_Bit();
    }						    
    return dat;
}


//读取一次数据//湿度值(十进制，范围:20%~90%) ，温度值(十进制，范围:0~50°)，返回值：0,正常;1,失败 
uint8_t Read_Dht11_Data()
{   
    uint8_t buf[5] = {0};
    uint8_t i = 0;
    DHT11_rest();//DHT11端口复位，发出起始信号
    if(DHT11_check()==0) //等待DHT11回应
    { 
        for(i=0;i<5;i++)  //读取5位数据
        {
            buf[i]=Dht11_ReadByte(); //读出数据
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]) //数据校验
        {	
            Dht11.Humidity=((buf[0]*10) + (buf[1]%10))/10.0; //将湿度值放入
            Dht11.Temperature=((buf[2]*10) + (buf[3]%10))/10.0; //将温度值放入
            
            printf("Dht11.Temperature= %f\t  Dht11.Humidity = %f\r\n", Dht11.Temperature, Dht11.Humidity);
            printf("temp = %d.%d\t hum = %d.%d\r\n", buf[2],buf[3],buf[0],buf[1]);
            return 1;
        }      
    }
    
    return 0;  
}


void dht11_gpio_init(void)
{
    gpio_config_t io_cf;

    io_cf.intr_type = GPIO_INTR_DISABLE;
    io_cf.mode = GPIO_MODE_OUTPUT;
    io_cf.pin_bit_mask = GPIO_DHT11_PIN_SEL;
    io_cf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_cf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_set_level(DHT11_IO, 1);
    gpio_config(&io_cf);
}

void Dht11_Init()
{
     dht11_gpio_init();
}

