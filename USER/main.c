#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timer.h"
#include "mpu9250.h"
#include "myiic.h"
#include "exti.h"
#include "dma.h"
#include "GetGPS.h"
#include "VariableType.h"
#include "GINavMain.h"

#include "task1.h"
#include "task2.h"
#include "GpsTask.h"
////////////////
#include "malloc.h"
#include "ff.h"
#include "exfuns.h"
#include "sdmmc_sdcard.h"

DIR recdir;




//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		512  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

#define LED1_TASK_PRIO		3
#define LED1_STK_SIZE 		50 
TaskHandle_t LED1Task_Handler;
//������

CONFIG_DATE_T GpsCon_Data;      //GPS����
CONFIG_GPSInit_T   Gps_Init;    //Gps��ʼ��
RAW_NMEA Gps_raw;

GPGGA_DATA  GPGGAData;				   //GGA�ṹ�壬�洢����Э���е�����
GPRMC_DATA  GPRMCData;           //RMC�ṹ�壬�洢����Э���е�����
GETGPS_DATE_T GpsGet_Data;      //GPS��� 

OUTPUT_INFO_T GINavResult;     // ��ϵ��������������Ϣ
IMU_DATA_T    IMUDataBuffer;	 // IMU����ָ���ָ��õ�ַ....
GNSS_DATA_T   GNSSDataBuffer;  // GPS����ָ���ָ��õ�ַ....

BOOL IMUDataReady;             // IMU�����Ƿ�׼���ã����׼���ã�ÿ�����꣬����
BOOL GNSSDataReady;            // GNSS�����Ƿ�׼���ã����׼���ã�ÿ�����꣬����

void Data_Init(void)
{
	Gps_Init.RATE_Init_flag = 1;		//5HZ
	Gps_Init.RMC_Init_flag = 1;		//GST open
	Gps_Init.GGA_Init_flag = 1;		//GST open
	Gps_Init.GST_Init_flag = 1;		//GST open
	Gps_Init.GSA_Init_flag = 0;		//GSA close
	Gps_Init.GSV_Init_flag = 0;		//GSV close
	Gps_Init.VTG_Init_flag = 0;		//VTG close
	Gps_Init.GLL_Init_flag = 0;		//GLL close
}	

void Set_System(void)
{
		uart_init(9600);		        //���ڳ�ʼ��
	  EXTI_Init();
    LED_Init();                     //��ʼ��LED
	  MYDMA_Config(DMA2_Stream7,DMA_CHANNEL_4);
	  MYDMA_Config(DMA1_Stream6,DMA_CHANNEL_4);
//	  TIM3_Init(10-1,10800-1);      //��ʱ��3��ʼ������ʱ��ʱ��Ϊ108M����Ƶϵ��Ϊ10800-1��                               
//		TIM2_Init(10000-1,10800-1);			//TIM2��ʱ��1ms
	  IIC_Init();
		MPU9250_Init();
		Data_Init();
	////////////////////////SD��
	my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMDTCM);		    //��ʼ��CCM�ڴ��
	 	while(SD_Init())//��ⲻ��SD��
	{
		delay_ms(500);	
	}
	exfuns_init();							//Ϊfatfs��ر��������ڴ
	f_mount(fs[0],"0:",1); 					//����SD��
  sd_creatfile();  //����SD�ļ�	
	
}

int main(void)
{
    Cache_Enable();                 //��L1-Cache
    HAL_Init();				        //��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
	  Set_System();
			
		GPS_Rate_Config();
		GpsConfig();	
		
    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����LED0����
    task1_init();
    //����LED1����
    task2_init();

		GpsTask_init();	
  
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}






