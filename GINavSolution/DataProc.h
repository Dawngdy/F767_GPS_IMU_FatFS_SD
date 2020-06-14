#ifndef __DNSAMPLER_H__
#define __DNSAMPLER_H__
#include "Config.h"
#include "Const.h"
#include "DataTypes.h"
#include "stdint.h"

#define GYRO_SCALE			(PI*INSUPDATE_SUBDATA_INTERVAL/1000.0/180.0/131.2)		//scale of gyroscope       PI/180*0.1/131.2  �Ƕ�����ϵ��
#define ACC_SCALE			(GRAVITY_CONST*INSUPDATE_SUBDATA_INTERVAL/1000.0/1000)	//scale of accelerometer 9.78*0.1/1000.0;  �ٶ�����ϵ��



extern IMU_DATA_T    IMUDataBuffer;	 //gty IMU����ָ���ָ��õ�ַ....
extern GNSS_DATA_T   GNSSDataBuffer;  //gty GPS����ָ���ָ��õ�ַ....

extern PIMU_DATA_T   pImuData;        //gty ָ��IMU....
extern PGNSS_DATA_T  pGnssData;       //gty ָ��GPS....




extern BOOL  IMUDataReady;            //gty IMU�����Ƿ�׼���ã����׼���ã�ÿ�����꣬����
extern BOOL  GNSSDataReady;           //gty GNSS�����Ƿ�׼���ã����׼���ã�ÿ�����꣬����

extern BOOL  IMUDataGetFlag;          //
extern BOOL  GNSSDataGetFlag;         //

extern GINAV_INFO_T  g_GINavInfo;     //gty ��ϵ������̵�������Ϣ   
extern OUTPUT_INFO_T GINavResult;    //gty ��ϵ��������������Ϣ



void DataProcerInit(void);
PIMU_DATA_T GetIMUData(void);
PGNSS_DATA_T GetGNSSData(void);

#endif
