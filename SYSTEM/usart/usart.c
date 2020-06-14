#include "usart.h"
#include "GetGPS.h"
#include "VariableType.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "GpsTask.h"
#include "FreeRTOS.h"      //os 使用	  
#endif

////////////////////////////////////////////////////////////////////////////////// 	  
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR=(u8)ch;      
	return ch;
}
#endif 

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	

UART_HandleTypeDef UART1_Handler; //UART1句柄
UART_HandleTypeDef UART2_Handler; //UART1句柄

uint8_t getBuffer1[1]; 
uint8_t getBuffer2[1]; 

uint8_t   Message_Rx_Data[12]; //OK
uint8_t   Message_Rx_Counter;
uint8_t   Message_Rx_Max;

//初始化IO 串口1、2
//bound:波特率
void uart_init(u32 bound)
{	
	//UART1 初始化设置
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)getBuffer1, 1);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
	
	//UART2 初始化设置
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART2
	
	HAL_UART_Receive_IT(&UART2_Handler, (uint8_t *)getBuffer2, 1);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,3,0);			//抢占优先级3，子优先级3
	}

	if(huart->Instance==USART2)//如果是串口2，进行串口2 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
		GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART2
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA2

		GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA3
		
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART2中断通道
		HAL_NVIC_SetPriority(USART2_IRQn,4,0);			//抢占优先级4，子优先级3
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
  if(huart->Instance==USART1)//如果是串口1 
  {
		res1 = getBuffer1[0];
		HAL_UART_Transmit(&UART2_Handler,&res1,1,1000);
  }

	if(huart->Instance==USART2)//如果是串口2
  {
		res2 = getBuffer2[0];
		
	  //HAL_UART_Transmit_DMA(&UART1_Handler,&res2,1);//开启DMA传输
		//HAL_UART_DMAStop(&UART1_Handler);      //传输完成以后关闭串口DMA
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
 




