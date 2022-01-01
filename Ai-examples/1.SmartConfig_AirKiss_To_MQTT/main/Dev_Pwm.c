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

#include "driver/pwm.h"

#define PWM_ALL_CH_NUM      8

#define PWM_1_OUT_IO_NUM   15
#define PWM_2_OUT_IO_NUM   13
#define PWM_3_OUT_IO_NUM   2
#define PWM_4_OUT_IO_NUM   12
#define PWM_5_OUT_IO_NUM   4
#define PWM_6_OUT_IO_NUM   5
#define PWM_7_OUT_IO_NUM   14
#define PWM_8_OUT_IO_NUM   16

// PWM period 20000us(50hz), same as sover
#define PWM_PERIOD    (20000)

static const char *TAG = "Dev_Pwm";

// pwm pin number
const uint32_t pin_num[PWM_ALL_CH_NUM] = {
    PWM_1_OUT_IO_NUM,
    PWM_2_OUT_IO_NUM,
    PWM_3_OUT_IO_NUM,
    PWM_4_OUT_IO_NUM,
    PWM_5_OUT_IO_NUM,
    PWM_6_OUT_IO_NUM,
    PWM_7_OUT_IO_NUM,
    PWM_8_OUT_IO_NUM
};

// duties table, real_duty = duties[x]/PERIOD
uint32_t duties[PWM_ALL_CH_NUM] = {
    1500, 1500, 1500, 1500,1500, 1500, 1500, 1500
};

// phase table, delay = (phase[x]/360)*PERIOD
int16_t phase[PWM_ALL_CH_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0
};


void Pwm_Poll()
{
    
}

void Set_Pwm_All_Chinel_Val(uint8_t ch_num,uint32_t *duty)
{
    for(uint8_t i = 0; i < ch_num; i++)
    {
        if(i < PWM_ALL_CH_NUM)
        {
            if(duty[i] >= 1000 && duty[i] <= 2000)
            {
                pwm_set_duty(i, duty[i]);
            }
        }
    }
    pwm_start();
}

/*Server 1000us -- 2000us Test*/
void Pwm_Servo_Test()
{
    static uint32_t count = 1000;
    static uint8_t flag = 0;
    if(flag == 0)
    {
        if(count < 1950)
        {
            count+=10;
        }
        else{
            flag = 1;
        }
    }
    else
    {
        if(count > 1050)
        {
            count -=10;
        }
        else
        {
            flag = 0;            
        }
    }
    for(uint8_t i = 0; i < PWM_ALL_CH_NUM;i++)
    {
        pwm_set_duty(i,count);
    }
    pwm_start();
    ESP_LOGI(TAG, "Test_Pwm_Chinel_Val: %d", count);
}

void Pwm_Init()
{
    pwm_init(PWM_PERIOD, duties, PWM_ALL_CH_NUM, pin_num);
    //pwm_set_channel_invert(0x1 << 0);
    pwm_set_phases(phase);
    pwm_start();
}

