#ifndef __DNSAMPLER_H__
#define __DNSAMPLER_H__
#include "Config.h"
#include "Const.h"
#include "DataTypes.h"
#include "stdint.h"

#define GYRO_SCALE			(PI*INSUPDATE_SUBDATA_INTERVAL/1000.0/180.0/131.2)		//scale of gyroscope       PI/180*0.1/131.2  角度增量系数
#define ACC_SCALE			(GRAVITY_CONST*INSUPDATE_SUBDATA_INTERVAL/1000.0/1000)	//scale of accelerometer 9.78*0.1/1000.0;  速度增量系数



extern IMU_DATA_T    IMUDataBuffer;	 //gty IMU所有指针均指向该地址....
extern GNSS_DATA_T   GNSSDataBuffer;  //gty GPS所有指针均指向该地址....

extern PIMU_DATA_T   pImuData;        //gty 指针IMU....
extern PGNSS_DATA_T  pGnssData;       //gty 指针GPS....




extern BOOL  IMUDataReady;            //gty IMU数据是否准备好，如果准备好，每次用完，清零
extern BOOL  GNSSDataReady;           //gty GNSS数据是否准备好，如果准备好，每次用完，清零

extern BOOL  IMUDataGetFlag;          //
extern BOOL  GNSSDataGetFlag;         //

extern GINAV_INFO_T  g_GINavInfo;     //gty 组合导航过程的所有信息   
extern OUTPUT_INFO_T GINavResult;    //gty 组合导航输出的所有信息



void DataProcerInit(void);
PIMU_DATA_T GetIMUData(void);
PGNSS_DATA_T GetGNSSData(void);

#endif
