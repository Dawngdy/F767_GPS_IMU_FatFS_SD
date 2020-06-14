#ifndef __GPSTASK_H__
#define __GPSTASK_H__

#include "FreeRTOS.h"

typedef enum GpsTask_message_id_e{
    GpsTask_id_num_1,
		GpsTask_id_num_2
} GpsTask_Message_id_e;

typedef struct GpsTask_message_t {
    GpsTask_Message_id_e         message_id;
    int         param1;
    void        *param2;
} GpsTask_Message_t;

void GpsTask_init(void);
int32_t GpsTask_send_massage(GpsTask_Message_id_e event_id, int32_t param1, void* param2);
#endif


