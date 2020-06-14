#ifndef __TASK2_H__
#define __TASK2_H__

#include "FreeRTOS.h"

typedef enum task2_message_id_e{
    task2_id_num_1,
		task2_id_num_2
} Task2_Message_id_e;

typedef struct task2_message_t {
    Task2_Message_id_e    message_id;
    int    param1;
    void   *param2;
} Task2_Message_t;

typedef struct imu_message_t {
    uint8_t *imu_data;
		uint8_t imu_pack_num;
} Imu_Message_t;

int32_t task2_send_massage(Task2_Message_id_e event_id, int32_t param1, void* param2);
void task2_main(void *pvParameters);
void task2_init(void);

#endif
