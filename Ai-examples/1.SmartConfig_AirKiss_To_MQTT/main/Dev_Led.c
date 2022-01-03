/* gpio example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "Dev_Led.h"

static const char *TAG = "Dev_Led";

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO16: output
 *
 */

Led_t g_LedState;

static void Led_Task(void *arg)
{
    static uint8_t count = 0;
    static uint8_t flag = 0;
    while(1)
    {
        vTaskDelay(10 / portTICK_RATE_MS);

        switch (g_LedState)
        {
            case OFF:
                LED_OFF();
                break;

            case ONE_HZ:
            case TWO_HZ:
            case THREE_HZ:
            case FOUR_HZ:
            case FIVE_HZ:
            if(++count >= 50/g_LedState)
            {
                count = 0;
                if(flag == 0)
                {
                    LED_ON();
                    flag = 1;
                }
                else if(flag == 1)
                {
                    LED_OFF();
                    flag = 0;
                }        
            }
            break;
            
            case ON:
                LED_ON();
            break;
            
            default:
                break;
        }   
    }
}

void Led_SetState(Led_t state)
{
    g_LedState = state;
    ESP_LOGI(TAG, "Led_SetState: %d\n", state);
}
Led_t Led_GetState()
{
    return g_LedState;
}

void Led_Init(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    gpio_set_level(GPIO_OUTPUT_IO_LED, 0);

    //start led_task
    xTaskCreate(Led_Task, "led_task", 1024, NULL, 10, NULL);
}


