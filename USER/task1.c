#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "task1.h"
#include "task2.h"

#include "mpu9250.h"
#include "string.h"
#include "timer.h"

#define TASK1_PRIO		2
#define TASK1_STK_SIZE 		50 

TaskHandle_t Task1_Handler;
QueueHandle_t Task1_Queue;

uint8_t *imu_get_buf;
uint8_t *imu_avr_buf;
uint8_t imu_get_count=0;
uint8_t imu_avr_count=0;

Imu_Message_t imu_massage;

static void task1_msg_handler(Task1_Message_t *message)
{
		int i;
	
    if(!message)
    {
        return;
    }
    switch(message->message_id)
    {
				case task1_id_num_1:
        		Get_IMU(imu_get_buf);
						//printf("==============\n");
						
						imu_get_count++;
						if(imu_get_count%5)
						{
								for(i=0;i<6;i++)
										imu_avr_buf[i]+=imu_get_buf[i];
						}
						else 
						{
								//printf("==============\n");
								imu_avr_count++;
								
								for(i=0;i<6;i++)
										imu_avr_buf[i]/=5;
							
								memcpy(imu_massage.imu_data,imu_get_buf,6);
								imu_massage.imu_pack_num = imu_avr_count;
								task2_send_massage(task2_id_num_1,0,&imu_massage);
							
								memset(imu_avr_buf,0,10);
						}
						
						if(imu_get_count==20)
						{
								imu_get_count=0;
								imu_avr_count=0;		
						}
					
            break;
        default :
            break;
    }
}

int32_t task1_send_massage(Task1_Message_id_e event_id, int32_t param1, void* param2)
{
    Task1_Message_t message;
		BaseType_t xHigherPriorityTaskWoken;
		
    message.message_id = event_id;
    message.param1 = param1;
    message.param2 = param2;
	
		//printf("task1_send_massage");
		
    return xQueueSendFromISR(Task1_Queue,&message,&xHigherPriorityTaskWoken);
}

void task1_main(void *pvParameters)
{
		BaseType_t xTaskWokenByReceive=pdFALSE;
		Task1_Message_t Task1_Message;
		
		TIM3_Init(400-1,10800-1);      //¶¨Ê±Æ÷3³õÊ¼»¯£¬¶¨Ê±Æ÷Ê±ÖÓÎª108M£¬·ÖÆµÏµÊıÎª10800-1£
		while(1)
		{
				if(xQueueReceiveFromISR(Task1_Queue,&Task1_Message,&xTaskWokenByReceive))
				{
						task1_msg_handler(&Task1_Message);
				}
		}
} 

void task1_init(void)
{
		xTaskCreate((TaskFunction_t )task1_main,     	
                (const char*    )"led0_task",   	
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_PRIO,	
                (TaskHandle_t*  )&Task1_Handler); 
		
		Task1_Queue=xQueueCreate(1,sizeof(Task1_Message_t));
		while(Task1_Queue==0);
}
