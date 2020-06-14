#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2016/6/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
	  	
extern UART_HandleTypeDef UART1_Handler; //UART1句柄
extern UART_HandleTypeDef UART2_Handler; //UART2句柄

#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer

extern uint8_t getBuffer1[1]; 
extern uint8_t getBuffer2[1]; 

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void Message_Ack_handle(uint8_t recv);
void Receive_NMEA(uint8_t buff);

#endif


