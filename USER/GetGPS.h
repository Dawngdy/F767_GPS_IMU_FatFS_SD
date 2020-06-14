#ifndef  __GETGPS_H
#define  __GETGPS_H

#include "stdint.h"

#define MAXFIELD	25
#define NP_MAX_CMD_LEN			8		  // maximum command length (NMEA address)
#define NP_MAX_DATA_LEN			180		// maximum data length
#define NP_ALL_DATA_LEN			1100	// maximum data length
#define NP_IMU_DATA_LEN			300		// maximum data length
#define NP_MAX_CHAN				  36		// maximum number of channels
#define NP_WAYPOINT_ID_LEN	32		// waypoint max string len  
#define FALSE   0
#define TRUE    1

typedef enum  tag_GPS_CONFIG
{
	GPS_CONFIG_PRT_SET,
	GPS_CONFIG_RATE,
	GPS_CONFIG_RMC_OPEN,
	GPS_CONFIG_GGA_OPEN,
	GPS_CONFIG_GST_OPEN,
	GPS_CONFIG_GSV_CLOSE,
	GPS_CONFIG_GSA_CLOSE,
	GPS_CONFIG_VTG_CLOSE,
	GPS_CONFIG_GLL_CLOSE,
	
}GPS_CONFIG;

typedef struct _GETGPS_DATE_T
{
	uint8_t   GGA_Get_Flag;        //Gps获得标志
	uint8_t   RMC_Get_Flag;        //Gps获得标志
	uint8_t   VTG_Get_Flag;        //Gps获得标志
	uint8_t   GST_Get_Flag;        //Gps获得标志
	uint8_t   GSA_Get_Flag;	       //Gps获得标志
	uint8_t  	GSV_Get_Flag;        //Gps获得标志
	uint8_t   Rmc_Vtg_Flag;        //Gps的类型
	uint8_t   Gps_Fren_Kind;       //Gps的频率
	uint8_t  	Gps_Ini_Flag;        //Gps的初始化
	uint8_t   Gps_First_Flag;      //Gps的首次	
}GETGPS_DATE_T, *PGETGPS_DATE_T;

typedef	struct tag_GPGGA_DATA 
{ 
	uint8_t 	Hour;							//
	uint8_t 	Minute;							//
	uint8_t 	Second;							//
	uint16_t 	MSecond;					    //
	double 	  Latitude;						// < 0 = South, > 0 = North
	double 	  BLatitude;						// < 0 = South, > 0 = North
	uint8_t   SNth;
	double 	  Longitude;						// < 0 = West, > 0 = East
	double 	  BLongitude;						// < 0 = West, > 0 = East
	uint8_t   WEst;
	uint8_t 	GPSQuality;						// 0 = fix not available, 1 = GPS sps mode, 2 = Differential GPS, SPS mode, fix valid, 3 = GPS PPS mode, fix valid
	uint8_t 	NumOfSatsInUse;					//
	double 	  HDOP;							//
	double 	  Altitude;						// Altitude: mean-sea-level (geoid) meters
	uint8_t   Auint;
	double    Geoidal;                        // Geoid Separation
	uint8_t   Guint;
	double    DTime;                          // 差分时间
	uint16_t  DGpsID;                         // 差分ID
	uint8_t   IDValid;
	uint32_t 	Count;							   //
	int32_t 	OldVSpeedSeconds;				//
	double 	  OldVSpeedAlt;					//
	double 	  VertSpeed;						//
  uint8_t   DFID_Data[4];
	uint8_t   Gp_PN_Kind;
	uint8_t   Utc_Flag;
} GPGGA_DATA;

/* GPRMC
Recommended Minimum Specific GPS/TRANSIT Data（RMC）推荐定位信息
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
<1> UTC时间，hhmmss（时分秒）格式
<2> 定位状态，A=有效定位，V=无效定位
<3> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
<4> 纬度半球N（北半球）或S（南半球）
<5> 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
<6> 经度半球E（东经）或W（西经）
<7> 地面速率（000.0~999.9节，前面的0也将被传输）
<8> 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输）
<9> UTC日期，ddmmyy（日月年）格式
<10> 磁偏角（000.0~180.0度，前面的0也将被传输）
<11> 磁偏角方向，E（东）或W（西）
<12> 模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
*/
typedef	struct tag_GPRMC_DATA { 
	uint8_t 	Hour;							//
	uint8_t 	Minute;							//
	uint8_t 	Second;							//
	uint16_t 	MSecond;							//
	uint8_t 	DataValid;						// A = Data valid, V = navigation rx warning
	double 	  Latitude;						// current latitude
	uint8_t   SNth;
	double 	  Longitude;						// current longitude
	uint8_t   WEst;
	double 	  GroundSpeed;					// speed over ground, knots
	double 	  Course;							// course over ground, degrees true
	uint8_t   SpeedValid;
	uint8_t   CourseValid;
	uint8_t 	Day;							//
	uint8_t 	Month;							//
	uint16_t 	Year;							//
	double 	  MagVar;							// magnitic variation, degrees East(+)/West(-)
	uint8_t   MagVarValid;
	uint8_t   MagWEst;                        // ModeIndicator A=Autonomous,D=Differential,E=Dead Reckoning,N=None;
	uint8_t   ModeIn;                         // ModeIndicator A=Autonomous,D=Differential,E=Dead Reckoning,N=None;
	uint32_t 	Count;							//
	uint8_t   Gp_PN_Kind;
	uint8_t   TimeValid;
	uint8_t   DayValid;
} GPRMC_DATA; 

extern GPGGA_DATA  GPGGAData;				   //GGA结构体，存储各个协议中的内容
extern GPRMC_DATA  GPRMCData;           //RMC结构体，存储各个协议中的内容
extern GETGPS_DATE_T GpsGet_Data;      //GPS获得 

void GPS_Rate_Config(void);
void GpsConfig(void);
void Flag_Clear(void);
void ProcessGPGGA(uint8_t *pData);
void ProcessGPRMC(uint8_t *pData);

#endif


