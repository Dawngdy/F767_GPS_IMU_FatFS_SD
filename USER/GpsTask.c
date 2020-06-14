#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"
#include "dma.h"
#include "GetGPS.h"

#include "VariableType.h"

#include "GpsTask.h"
#include "task2.h"

#define GPSTASK_PRIO		2
#define GPSTASK_STK_SIZE 		256 

TaskHandle_t GpsTask_Handler;
QueueHandle_t GpsTask_Queue;

RAW_NMEA *Gps_data;

static void GpsTask_msg_handler(GpsTask_Message_t *message)
{	
		uint8_t i;
		static int count=0;
	
    if(!message)
    {
        return;
    }
		
		switch(message->message_id)
    {
			case GpsTask_id_num_1:
					Gps_data = message->param2;
			
		   		for(i=0;i<200;i++)
					{
						Gps_data->gga_receive[i] = Gps_data->gga_buf[i+7];
					}
					
					for(i=0;i<200;i++)
					{
						Gps_data->rmc_receive[i] = Gps_data->rmc_buf[i+7];
					}
					
					ProcessGPGGA(Gps_data->gga_receive);
					ProcessGPRMC(Gps_data->rmc_receive);
					
					if(GpsGet_Data.GGA_Get_Flag && GpsGet_Data.RMC_Get_Flag)
					{
						GpsGet_Data.GGA_Get_Flag  = 0;
						GpsGet_Data.RMC_Get_Flag  = 0;
						
						task2_send_massage(task2_id_num_2,NULL,&Gps_raw);
					}

//					printf("%s\n",Gps_raw.gga_buf);
//					printf("%s\n",Gps_raw.rmc_buf);
			
			break;
			
			case GpsTask_id_num_2:
					 //ProcessGPRMC(message->param2);
			
			break;
						
			default :
      break;
		}
}


int32_t GpsTask_send_massage(GpsTask_Message_id_e event_id, int32_t param1, void* param2)
{
	  GpsTask_Message_t message;
		BaseType_t xHigherPriorityTaskWoken;
	
		message.message_id = event_id;
		message.param1 = param1;
		message.param2 = param2;
	
		return xQueueSendFromISR(GpsTask_Queue,&message,&xHigherPriorityTaskWoken);
}

void GpsTask_main(void *pvParameters)
{
		BaseType_t xTaskWokenByReceive=pdFALSE;
		GpsTask_Message_t GpsTask_Message;
	
		while(1)
		{		
				if(xQueueReceiveFromISR(GpsTask_Queue,&GpsTask_Message,&xTaskWokenByReceive))
				{
						GpsTask_msg_handler(&GpsTask_Message);
				}
		}
} 

void GpsTask_init(void)
{
		xTaskCreate((TaskFunction_t )GpsTask_main,     	
                (const char*    )"Gps_task",   	
                (uint16_t       )GPSTASK_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )GPSTASK_PRIO,	
                (TaskHandle_t*  )&GpsTask_Handler); 
		
		GpsTask_Queue=xQueueCreate(1,sizeof(GpsTask_Message_t));
		while(GpsTask_Queue==0);
}