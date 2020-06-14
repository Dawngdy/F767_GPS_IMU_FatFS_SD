#include "usart.h"
#include "GetGPS.h"
#include "VariableType.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "GpsTask.h"
#include "FreeRTOS.h"      //os ʹ��	  
#endif

////////////////////////////////////////////////////////////////////////////////// 	  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR=(u8)ch;      
	return ch;
}
#endif 

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	

UART_HandleTypeDef UART1_Handler; //UART1���
UART_HandleTypeDef UART2_Handler; //UART1���

uint8_t getBuffer1[1]; 
uint8_t getBuffer2[1]; 

uint8_t   Message_Rx_Data[12]; //OK
uint8_t   Message_Rx_Counter;
uint8_t   Message_Rx_Max;

//��ʼ��IO ����1��2
//bound:������
void uart_init(u32 bound)
{	
	//UART1 ��ʼ������
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)getBuffer1, 1);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
	
	//UART2 ��ʼ������
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART2
	
	HAL_UART_Receive_IT(&UART2_Handler, (uint8_t *)getBuffer2, 1);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//����ΪUSART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,3,0);			//��ռ���ȼ�3�������ȼ�3
	}

	if(huart->Instance==USART2)//����Ǵ���2�����д���2 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA2

		GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA3
		
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//ʹ��USART2�ж�ͨ��
		HAL_NVIC_SetPriority(USART2_IRQn,4,0);			//��ռ���ȼ�4�������ȼ�3
	}
}
void USART1_IRQHandler(void)   
{
	HAL_UART_IRQHandler(&UART1_Handler);
	
	while(HAL_UART_Receive_IT(&UART1_Handler, (uint8_t*)getBuffer1,1)!=HAL_OK);

}
	
void USART2_IRQHandler(void)   
{
	HAL_UART_IRQHandler(&UART2_Handler);
	
	while(HAL_UART_Receive_IT(&UART2_Handler, (uint8_t*)getBuffer2,1)!=HAL_OK);


}

u8 usart2_count=0;
u8 gga_trans_flag;
u8 rmc_trans_flag;
u8 gst_trans_flag;
u8 gga_finish_flag;
u8 rmc_finish_flag;
u8 gst_finish_flag;
u8 nmea_count;
u8 usart2_buf[100];

u8 gga_count;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t res1,res2;
  if(huart->Instance==USART1)//����Ǵ���1 
  {
		res1 = getBuffer1[0];
		HAL_UART_Transmit(&UART2_Handler,&res1,1,1000);
  }

	if(huart->Instance==USART2)//����Ǵ���2
  {
		res2 = getBuffer2[0];
		
	  //HAL_UART_Transmit_DMA(&UART1_Handler,&res2,1);//����DMA����
		//HAL_UART_DMAStop(&UART1_Handler);      //��������Ժ�رմ���DMA
		Receive_NMEA(res2);
		Message_Ack_handle(res2);
  }
}

void Receive_NMEA(uint8_t buff)
{
	
	if(gga_trans_flag)
		{
			Gps_raw.gga_buf[usart2_count] = buff;
		}
		else if(rmc_trans_flag)
		{
			Gps_raw.rmc_buf[usart2_count] = buff;
		}
		else if(gst_trans_flag)
		{
			Gps_raw.gst_buf[usart2_count] = buff;
		}
		else
		{
			usart2_buf[usart2_count] = buff;
		}
				
		usart2_count++;
    //HAL_UART_Transmit(&UART1_Handler,&rec2,1,1000); 
		if(usart2_count==1)
		{
			if(usart2_buf[0] == '$')
			{
				
			}
			else
			{
				usart2_count = 0;
			}
		}
		else if(usart2_count == 6)
		{
			if(usart2_buf[1] == 'G' && usart2_buf[3] == 'G' && usart2_buf[4] == 'G' && usart2_buf[5] == 'A')
			{
				Gps_raw.gga_buf[0] = '$';
				Gps_raw.gga_buf[1] = 'G';
				Gps_raw.gga_buf[2] = 'N';
				Gps_raw.gga_buf[3] = 'G';
				Gps_raw.gga_buf[4] = 'G';
				Gps_raw.gga_buf[5] = 'A';
				
				//printf("");
				
				gga_trans_flag = 1;
			}
			else if(usart2_buf[1] == 'G' && usart2_buf[3] == 'R' && usart2_buf[4] == 'M' && usart2_buf[5] == 'C')
			{
				Gps_raw.rmc_buf[0] = '$';
				Gps_raw.rmc_buf[1] = 'G';
				Gps_raw.rmc_buf[2] = 'N';
				Gps_raw.rmc_buf[3] = 'R';
				Gps_raw.rmc_buf[4] = 'M';
				Gps_raw.rmc_buf[5] = 'C';
				
				rmc_trans_flag = 1;
			}
			else if(usart2_buf[1] == 'G' && usart2_buf[3] == 'G' && usart2_buf[4] == 'S' && usart2_buf[5] == 'T')
			{
				Gps_raw.gst_buf[0] = '$';
				Gps_raw.gst_buf[1] = 'G';
				Gps_raw.gst_buf[2] = 'N';
				Gps_raw.gst_buf[3] = 'G';
				Gps_raw.gst_buf[4] = 'S';
				Gps_raw.gst_buf[5] = 'T';
				
				gst_trans_flag = 1;
			}
			else
			{
				usart2_count = 0;
			}
		}
		else if(usart2_count>6)
		{
			if(buff == '*')
			{
				nmea_count = usart2_count+2;
			}
			else if(nmea_count == usart2_count)
			{
				usart2_count = 0;
				nmea_count = 0;
			
				if(gga_trans_flag)
				{
					gga_trans_flag = 0;
					gga_finish_flag = 1;
					
//					gga_count++;
//					printf("%d\n",gga_count);
				}
				else if(rmc_trans_flag)
				{
					rmc_trans_flag = 0;
					rmc_finish_flag = 1;
				}
				else if(gst_trans_flag)
				{
					gst_trans_flag = 0;
					gst_finish_flag = 1;
				}
				if(rmc_finish_flag && gga_finish_flag && gst_finish_flag)
				{
					rmc_finish_flag = 0;
					gga_finish_flag = 0;
					gst_finish_flag = 0;
					
					GpsTask_send_massage(GpsTask_id_num_1,NULL,&Gps_raw);
//					printf("%s\n",Gps_raw.gga_buf);
//					printf("%s\n",Gps_raw.rmc_buf);
//					printf("%s\n",Gps_raw.gst_buf);
				}
			}
		}
}


void Message_Ack_handle(uint8_t recv)
{
	 if(Message_Rx_Counter>=sizeof(Message_Rx_Data))
	 {
		Message_Rx_Counter=sizeof(Message_Rx_Data)-1;
	 }
	
   Message_Rx_Data[Message_Rx_Counter]=recv;
   Message_Rx_Counter++;

	 if(Message_Rx_Counter<=4)
   {
		 
		if(Message_Rx_Data[0] != 0XB5)
	  {	 
	   Message_Rx_Counter=0;			
	  }
	  if(Message_Rx_Counter==4)
	  {			
		 if((Message_Rx_Data[0]==0XB5)&&(Message_Rx_Data[1]==0x62)&&(Message_Rx_Data[3]==0x01)) 
     {
      Message_Rx_Max = 10;
     }
     else
	   {
      Message_Rx_Max = 1;
	    Message_Rx_Counter=0;
	   }		
		}
		
	 }
	 else
	 {
		 if(Message_Rx_Counter>=Message_Rx_Max)
		 {
			 Message_Rx_Counter=0;
				 if((Message_Rx_Data[6]==0x06)&&(Message_Rx_Data[7]==0x01)) 
				 {
					  GpsCon_Data.RMC_Config_flag = 1;
						GpsCon_Data.GGA_Config_flag = 1;
					  GpsCon_Data.GST_Config_flag = 1;
						GpsCon_Data.GLL_Config_flag = 1;
						GpsCon_Data.GSV_Config_flag = 1;
						GpsCon_Data.GSA_Config_flag = 1;
					  GpsCon_Data.VTG_Config_flag = 1;
				 }
				 
				 else if((Message_Rx_Data[6]==0x06)&&(Message_Rx_Data[7]==0x08)) 
				 {
						GpsCon_Data.RATE_Config_flag = 1;
				 }	
					
		 }
	 }

}
 




