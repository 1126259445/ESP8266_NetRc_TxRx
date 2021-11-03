#ifndef __OLED_I2C_H__
#define __OLED_I2C_H__

#include <stdio.h>
#include "esp_system.h"

// 宏定义
//=============================================================================
#define		OLED_CMD  	0		// 命令
#define		OLED_DATA 	1		// 数据

#define 	SIZE16 		16		//显示字符的大小8*16
#define     SIZE32      32      //显示字符的大小16*32

#define 	Max_Column	128		//最大列数
#define		Max_Row		64		//最大行数
#define		X_WIDTH 	128		//X轴的宽度
#define		Y_WIDTH 	64	    //Y轴的宽度
#define		IIC_ACK		0		//应答
#define		IIC_NO_ACK	1		//不应答
//=============================================================================

// 函数声明
//=============================================================================
void  OLED_I2C_Init(void);
void  OLED_Clear(void);
void  OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);
void  OLED_ShowIP(uint8_t x, uint8_t y, char*Array_IP);
void  OLED_ShowString(uint8_t x, uint8_t y, char * Show_char,uint8_t fount_size);

#endif

