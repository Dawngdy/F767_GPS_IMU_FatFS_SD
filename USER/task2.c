#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "VariableType.h"
#include "SendGPS.h"

#include "task2.h"
#include "mpu9250.h"
////gdy
#include "exfuns.h"

#define TASK2_PRIO		4
#define TASK2_STK_SIZE 		1024                //256 

TaskHandle_t Task2_Handler;
QueueHandle_t Task2_Queue;

RAW_NMEA *Gps_Sendbuff;
Imu_Message_t *imu_massage2;

static void task2_msg_handler(Task2_Message_t *message)
{
		//int count=0;
	
    if(!message)
    {
        return;
    }
    switch(message->message_id)
    {
        case task2_id_num_1:
						imu_massage2 = (Imu_Message_t*)message->param2;
						printf("ACC_DATA X: %d, Y: %d Z:%d\n",imu_massage2->imu_data[0],imu_massage2->imu_data[1],imu_massage2->imu_data[2]);
						printf("GYR_DATA X: %d, Y: %d Z:%d\n",imu_massage2->imu_data[3],imu_massage2->imu_data[4],imu_massage2->imu_data[5]);
						printf("Packed time is %d!!\n",imu_massage2->imu_pack_num);
            //将IMU数据保存至SD卡		   
						sd_saveimu("0:SaveData/ImuData.txt",imu_massage2->imu_data);
            break;
				
				case task2_id_num_2:		
						Gps_Sendbuff = message->param2;
			
						GetLGPGGA(Gps_Sendbuff->gga_data);
						GetGPRMC (Gps_Sendbuff->rmc_data);
				
						Gps_raw.gga_buf[2] = 'I';
						printf("%s\n",Gps_Sendbuff->gga_buf);
						Gps_raw.rmc_buf[2] = 'I';
						printf("%s\n",Gps_Sendbuff->rmc_buf);
						Gps_raw.gst_buf[2] = 'I';
						printf("%s\n",Gps_Sendbuff->gst_buf);
					//保存GPS数据
					sd_savegps("0:SaveData/GpsData.txt",Gps_Sendbuff->gga_buf);
			  	sd_savegps("0:SaveData/GpsData.txt",Gps_Sendbuff->rmc_buf);
				  sd_savegps("0:SaveData/GpsData.txt",Gps_Sendbuff->gst_buf);				
            break;
				
        default :
            break;
    }
}

int32_t task2_send_massage(Task2_Message_id_e event_id, int32_t param1, void* param2)
{
    Task2_Message_t message;
		TickType_t ticks;
		
    message.message_id = event_id;
    message.param1 = param1;
    message.param2 = param2;
		
    return xQueueSend(Task2_Queue,&message,ticks);
}

void task2_main(void *pvParameters)
{
		Task2_Message_t Task2_Message;
		
		while(1)
		{
				if(xQueueReceive(Task2_Queue,&Task2_Message,portMAX_DELAY))
				{
						task2_msg_handler(&Task2_Message);
				}
		}
} 

void task2_init(void)
{
		xTaskCreate((TaskFunction_t )task2_main,     	
                (const char*    )"task2_main",   	
                (uint16_t       )TASK2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_PRIO,	
                (TaskHandle_t*  )&Task2_Handler); 
		
		Task2_Queue=xQueueCreate(1,sizeof(Task2_Message_t));
		while(Task2_Queue==0);
}
