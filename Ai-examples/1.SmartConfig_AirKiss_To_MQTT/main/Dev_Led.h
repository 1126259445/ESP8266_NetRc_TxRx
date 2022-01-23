#ifndef __DEV_LED_H__
#define __DEV_LED_H__

#include <stdio.h>
#include "esp_system.h"

#define GPIO_OUTPUT_IO_LED    16
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_IO_LED)
#define LED_ON()    gpio_set_level(GPIO_OUTPUT_IO_LED, 1)
#define LED_OFF()   gpio_set_level(GPIO_OUTPUT_IO_LED, 0)

typedef enum{
    OFF = 0,
    ONE_HZ,
    TWO_HZ,
    THREE_HZ,
    FOUR_HZ,
    FIVE_HZ,
    ON
}Led_t;


void Led_SetState(Led_t state);
Led_t Led_GetState();
void Led_Init(void);
#endif
