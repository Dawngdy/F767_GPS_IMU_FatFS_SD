#ifndef __TASK1_H__
#define __TASK1_H__

#include "FreeRTOS.h"

typedef enum task1_message_id_e{
    task1_id_num_1,
		task1_id_num_2
} Task1_Message_id_e;

typedef struct task1_message_t {
    Task1_Message_id_e         message_id;
    int         param1;
    void        *param2;
} Task1_Message_t;

int32_t task1_send_massage(Task1_Message_id_e event_id, int32_t param1, void* param2);
void task1_main(void *pvParameters);
void task1_init(void);

#endif
