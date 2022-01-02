#ifndef __DEV_PWM_H__
#define __DEV_PWM_H__

#include <stdio.h>
#include "esp_system.h"

// 宏定义
//=============================================================================
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

extern uint32_t duties[PWM_ALL_CH_NUM];

//=============================================================================

// 函数声明
//=============================================================================
void Pwm_Servo_Test(void);

void Pwm_Poll(void);

void Set_Pwm_All_Chinel_Val(uint8_t ch_num,uint32_t *duty);

void Pwm_Init(void);

#endif

