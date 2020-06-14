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




//ÈÎÎñÓÅÏÈ¼¶
#define START_TASK_PRIO		1
//ÈÎÎñ¶ÑÕ»´óĞ¡	
#define START_STK_SIZE 		512  
//ÈÎÎñ¾ä±ú
TaskHandle_t StartTask_Handler;
//ÈÎÎñº¯Êı
void start_task(void *pvParameters);

#define LED1_TASK_PRIO		3
#define LED1_STK_SIZE 		50 
TaskHandle_t LED1Task_Handler;
//ÈÎÎñº¯Êı

CONFIG_DATE_T GpsCon_Data;      //GPSÅäÖÃ
CONFIG_GPSInit_T   Gps_Init;    //Gps³õÊ¼»¯
RAW_NMEA Gps_raw;

GPGGA_DATA  GPGGAData;				   //GGA½á¹¹Ìå£¬´æ´¢¸÷¸öĞ­ÒéÖĞµÄÄÚÈİ
GPRMC_DATA  GPRMCData;           //RMC½á¹¹Ìå£¬´æ´¢¸÷¸öĞ­ÒéÖĞµÄÄÚÈİ
GETGPS_DATE_T GpsGet_Data;      //GPS»ñµÃ 

OUTPUT_INFO_T GINavResult;     // ×éºÏµ¼º½Êä³öµÄËùÓĞĞÅÏ¢
IMU_DATA_T    IMUDataBuffer;	 // IMUËùÓĞÖ¸Õë¾ùÖ¸Ïò¸ÃµØÖ·....
GNSS_DATA_T   GNSSDataBuffer;  // GPSËùÓĞÖ¸Õë¾ùÖ¸Ïò¸ÃµØÖ·....

BOOL IMUDataReady;             // IMUÊı¾İÊÇ·ñ×¼±¸ºÃ£¬Èç¹û×¼±¸ºÃ£¬Ã¿´ÎÓÃÍê£¬ÇåÁã
BOOL GNSSDataReady;            // GNSSÊı¾İÊÇ·ñ×¼±¸ºÃ£¬Èç¹û×¼±¸ºÃ£¬Ã¿´ÎÓÃÍê£¬ÇåÁã

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
		uart_init(9600);		        //´®¿Ú³õÊ¼»¯
	  EXTI_Init();
    LED_Init();                     //³õÊ¼»¯LED
	  MYDMA_Config(DMA2_Stream7,DMA_CHANNEL_4);
	  MYDMA_Config(DMA1_Stream6,DMA_CHANNEL_4);
//	  TIM3_Init(10-1,10800-1);      //¶¨Ê±Æ÷3³õÊ¼»¯£¬¶¨Ê±Æ÷Ê±ÖÓÎª108M£¬·ÖÆµÏµÊıÎª10800-1£¬                               
//		TIM2_Init(10000-1,10800-1);			//TIM2¶¨Ê±Æ÷1ms
	  IIC_Init();
		MPU9250_Init();
		Data_Init();
	////////////////////////SD¿¨
	my_mem_init(SRAMIN);		    //³õÊ¼»¯ÄÚ²¿ÄÚ´æ³Ø
	my_mem_init(SRAMEX);		    //³õÊ¼»¯Íâ²¿ÄÚ´æ³Ø
	my_mem_init(SRAMDTCM);		    //³õÊ¼»¯CCMÄÚ´æ³Ø
	 	while(SD_Init())//¼ì²â²»µ½SD¿¨
	{
		delay_ms(500);	
	}
	exfuns_init();							//ÎªfatfsÏà¹Ø±äÁ¿ÉêÇëÄÚ´
	f_mount(fs[0],"0:",1); 					//¹ÒÔØSD¿¨
  sd_creatfile();  //´´½¨SDÎÄ¼ş	
	
}

int main(void)
{
    Cache_Enable();                 //´ò¿ªL1-Cache
    HAL_Init();				        //³õÊ¼»¯HAL¿â
    Stm32_Clock_Init(432,25,2,9);   //ÉèÖÃÊ±ÖÓ,216Mhz 
    delay_init(216);                //ÑÓÊ±³õÊ¼»¯
	  Set_System();
			
		GPS_Rate_Config();
		GpsConfig();	
		
    //´´½¨¿ªÊ¼ÈÎÎñ
    xTaskCreate((TaskFunction_t )start_task,            //ÈÎÎñº¯Êı
                (const char*    )"start_task",          //ÈÎÎñÃû³Æ
                (uint16_t       )START_STK_SIZE,        //ÈÎÎñ¶ÑÕ»´óĞ¡
                (void*          )NULL,                  //´«µİ¸øÈÎÎñº¯ÊıµÄ²ÎÊı
                (UBaseType_t    )START_TASK_PRIO,       //ÈÎÎñÓÅÏÈ¼¶
                (TaskHandle_t*  )&StartTask_Handler);   //ÈÎÎñ¾ä±ú              
    vTaskStartScheduler();          //¿ªÆôÈÎÎñµ÷¶È
}

//¿ªÊ¼ÈÎÎñÈÎÎñº¯Êı
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //½øÈëÁÙ½çÇø
    //´´½¨LED0ÈÎÎñ
    task1_init();
    //´´½¨LED1ÈÎÎñ
    task2_init();

		GpsTask_init();	
  
    vTaskDelete(StartTask_Handler); //É¾³ı¿ªÊ¼ÈÎÎñ
    taskEXIT_CRITICAL();            //ÍË³öÁÙ½çÇø
}






