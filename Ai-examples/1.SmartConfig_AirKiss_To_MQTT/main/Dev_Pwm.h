#ifndef __DEV_PWM_H__
#define __DEV_PWM_H__

#include <stdio.h>
#include "esp_system.h"

// 宏定义
//=============================================================================


//=============================================================================

// 函数声明
//=============================================================================
void Pwm_Servo_Test(void);

void Pwm_Poll(void);

void Set_Pwm_All_Chinel_Val(uint8_t ch_num,uint32_t *duty);

void Pwm_Init(void);

#endif

