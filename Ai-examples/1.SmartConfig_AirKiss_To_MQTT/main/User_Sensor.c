#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_log.h"
#include "esp_system.h"

#include "Dev_Dht11.h"
#include "Dev_Oled_I2c.h"

TaskHandle_t Sensor_Handle = NULL;

/**
 * @description: Sensor Read_data
 * @param {type} 
 * @return: 
 */
void Task_Sensor(void *pvParameters)
{
    printf("Task_Sensor------------------------------------------ \r\n");
    static uint32_t Dht11_Tick = 0;
    static uint32_t Speed_Tick = 0;
    static uint32_t Xxxx_Tick = 0;

    Dht11_Init();

    while(1)
    {
        if(++Dht11_Tick > 10)
        {
            Dht11_Tick = 0;
            printf("read Dht11 \r\n");
            uint8_t ret = Read_Dht11_Data();
            if(ret == 1)
            {
                char Temperature[10] = {0};
                char Humidity[10] = {0};
                sprintf(Temperature,"T :%.1f",Get_Dht11_Temperature()/10.0f);
                sprintf(Humidity,"H :%.1f",Get_Dht11_Humidity()/10.0f);
                OLED_ShowString(0,0,Temperature,SIZE32);
                OLED_ShowString(0,4,Humidity,SIZE32);
                printf("%s,%s\r\n",Temperature,Humidity);
            }
        }

        if(++Speed_Tick > 50)
        {
            Speed_Tick = 0;
           
        }

        if(++Xxxx_Tick > 100)
        {
            Xxxx_Tick = 0;
        }

         vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}



