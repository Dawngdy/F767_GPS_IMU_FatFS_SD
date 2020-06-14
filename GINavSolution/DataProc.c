#include "DataProc.h"
#include "BasicFunc.h"
#include <stdlib.h>
#include <string.h>

void DataProcerInit(void)
{

	IMUDataReady = FALSE;
	GNSSDataReady = FALSE;
	MEMSET(&IMUDataBuffer, 0, SIZEOF(IMU_DATA_T));
	MEMSET(&GNSSDataBuffer, 0, SIZEOF(GNSS_DATA_T));
}



PIMU_DATA_T GetIMUData(void)//if no data in buffer, GiNavPro() will return directly
{
	if (!IMUDataReady)    //gty ���û��׼���ã��ͷ���... ���׼�����ˣ�����ִ���������
		return NULL;
	IMUDataReady = FALSE; //gty ���׼�����ˣ������øñ�־λΪFALSE����ÿ������ñ�־λ......
	return &IMUDataBuffer;
}

void SetGNSSData(PGNSS_DATA_T pGNSSData)
{
	if (!pGNSSData)
		return;
	MEMCPY(&GNSSDataBuffer, pGNSSData, SIZEOF(GNSS_DATA_T));
	GNSSDataReady = TRUE;
}

PGNSS_DATA_T GetGNSSData(void)
{
	if (!GNSSDataReady) 
		return NULL;
	GNSSDataReady = FALSE;    //gty ÿ������ñ�־λ...
	return &GNSSDataBuffer;
}
