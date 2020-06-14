#ifndef __CONFIG_H__
#define __CONFIG_H__


#define IMU_RAW_SAMPLE_RATE			50	//IMUԭʼ������
#define IMU_COLLECT_RATE			10	//IMU�����ռ�����  
#define INS_UPDATE_RATE				20	//INS��������      10HZ
#define INS_UPDATE_SAMPLE_NUM		1	//INS����������


#define Deta_HDop		    0.85	  //��ֵ---ˮƽ
#define Deta_SatUseNum		5	      //��ֵ---5��....
#define Deta_SatUseRatio  0.4     //��ֵ---ʹ����
#define Deta_MeanCn0      25      //��ֵ---�����
#define Deta_PDop         2.5     //��ֵ---PDOP
#define Deta_Heading      15      //��ֵ---����ֵ...

#define Max_Speed         50      //��ֵ---PDOP
#define Deta_Pos          10

#define M              39940.64       // ����Ȧ     ��λ km
#define N              40075.86       // ���Ȧ     ��λ km

#define IMU_RAWSAMPLE_INTERVAL		(1000/IMU_RAW_SAMPLE_RATE)					//IMU nominal sample period  20,��20ms.
#define IMU_FRAME_INTERVAL			(1000/IMU_COLLECT_RATE)						//IMU frame period          100,��100ms.
#define IMU_FRAME_SAMPLENUM			(IMU_RAW_SAMPLE_RATE/IMU_COLLECT_RATE+2)	//max sample number of each frame, set redundancy as 2  7
#define INSUPDATE_DATA_INTERVAL		(1000/INS_UPDATE_RATE)						//INS update period         100,��100ms.
#define INSUPDATE_SUBDATA_INTERVAL	(1000/INS_UPDATE_RATE/INS_UPDATE_SAMPLE_NUM)//INS update subdata period 100,��100ms.

//GNSS Config
#define LEAP_SECONDS		16		//Diff Second between UTC and GPSTime, modify this number if leap seconds chanaged
#define MAX_SYSTEM_NUM		3		//GNSS system number
#define MAX_SVID			160



#endif
