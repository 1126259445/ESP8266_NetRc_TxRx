/* pwm example

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
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "esp8266/gpio_register.h"
#include "esp8266/pin_mux_register.h"

#include "driver/gpio.h"

static const char *TAG = "Dev_Ppm";

#define PPM_IN_PIN  GPIO_Pin_12
#define GPIO_INPUT_IO_0     12
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)

#define FIRST_FIFTER_COUNTER    5
#define CYCLE_MIN   900
#define CYCLE_MAX   2000

uint16_t RC_ch[10] = {0};


/******************************************************************************
 * FunctionName : gpio_intr_handler
 * Description  : gpio interrupt callback funtion
 * Parameters   : void
 * Returns      : void
*******************************************************************************/
extern uint32_t esp_get_time(void);
void gpio_isr_handler(void)
{
    static uint8_t i = 0;
    static uint8_t first_recv = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    static uint32_t last_time = 0;
    static uint32_t current_time = 0;

    current_time = esp_get_time();
    uint32_t cycle = (current_time - last_time);
    if(first_recv == FIRST_FIFTER_COUNTER)
    {
        if(cycle >= CYCLE_MIN && cycle <= CYCLE_MAX)
        {
            RC_ch[i] = cycle;
        }
        i++;
    }
    if(cycle > 2000)
    {
        i = 0;
        if(first_recv < FIRST_FIFTER_COUNTER)
            first_recv++;
    }

    last_time = current_time;
}
/******************************************************************************
 * FunctionName : GpioConfig
 * Description  : GpioConfig
 * Parameters   : void
 * Returns      : NULL
*******************************************************************************/
#if 0
void GpioConfig(void)
{
      gpio_config_t gpio_in_cfg;    //Define GPIO Init Structure
      gpio_in_cfg.pin_bit_mask = PPM_IN_PIN;  //
      gpio_in_cfg.mode = GPIO_MODE_INPUT;    //Input mode
      gpio_in_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
      gpio_in_cfg.intr_type = GPIO_INTR_POSEDGE;    // up
      gpio_config(&gpio_in_cfg);    //Initialization function

      // Register the interrupt callback function
      esp_err_t ret = gpio_isr_register(gpio_isr_handler, NULL,0,NULL);
      if(ret != ESP_OK)
      {
            ESP_LOGI(TAG, "gpio_isr_register PPM err");
      }
      else
      {
            ESP_LOGI(TAG, "gpio_isr_register PPM OK");
      } 
}
#endif
void GpioConfig(void)
{
    gpio_config_t io_conf;    //Define GPIO Init Structure
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_POSEDGE);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin again
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *) GPIO_INPUT_IO_0);
}

void Task_Show_Ppm(void *pvParameters) 
{
    while(1)
    {
        ESP_LOGI(TAG, "RC_ch= %d,%d,%d,%d,%d,%d,%d,%d,%d%d",RC_ch[0],RC_ch[1],RC_ch[2],RC_ch[3],RC_ch[4],RC_ch[5],RC_ch[6],RC_ch[7],RC_ch[8],RC_ch[9]);
        vTaskDelay(200/portTICK_RATE_MS);
    }

}

void PPM_Init()
{
    GpioConfig();
    xTaskCreate(Task_Show_Ppm, "Task_Show_Ppm", 1024, NULL, 5, NULL);
}

