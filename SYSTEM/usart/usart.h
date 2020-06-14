#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2016/6/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
	  	
extern UART_HandleTypeDef UART1_Handler; //UART1���
extern UART_HandleTypeDef UART2_Handler; //UART2���

#define RXBUFFERSIZE   1 //�����С
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL��USART����Buffer

extern uint8_t getBuffer1[1]; 
extern uint8_t getBuffer2[1]; 

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void Message_Ack_handle(uint8_t recv);
void Receive_NMEA(uint8_t buff);

#endif


