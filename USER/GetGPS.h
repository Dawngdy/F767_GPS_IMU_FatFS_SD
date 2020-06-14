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
	uint8_t   GGA_Get_Flag;        //Gps��ñ�־
	uint8_t   RMC_Get_Flag;        //Gps��ñ�־
	uint8_t   VTG_Get_Flag;        //Gps��ñ�־
	uint8_t   GST_Get_Flag;        //Gps��ñ�־
	uint8_t   GSA_Get_Flag;	       //Gps��ñ�־
	uint8_t  	GSV_Get_Flag;        //Gps��ñ�־
	uint8_t   Rmc_Vtg_Flag;        //Gps������
	uint8_t   Gps_Fren_Kind;       //Gps��Ƶ��
	uint8_t  	Gps_Ini_Flag;        //Gps�ĳ�ʼ��
	uint8_t   Gps_First_Flag;      //Gps���״�	
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
	double    DTime;                          // ���ʱ��
	uint16_t  DGpsID;                         // ���ID
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
Recommended Minimum Specific GPS/TRANSIT Data��RMC���Ƽ���λ��Ϣ
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
<1> UTCʱ�䣬hhmmss��ʱ���룩��ʽ
<2> ��λ״̬��A=��Ч��λ��V=��Ч��λ
<3> γ��ddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<4> γ�Ȱ���N�������򣩻�S���ϰ���
<5> ����dddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<6> ���Ȱ���E����������W��������
<7> �������ʣ�000.0~999.9�ڣ�ǰ���0Ҳ�������䣩
<8> ���溽��000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ���0Ҳ�������䣩
<9> UTC���ڣ�ddmmyy�������꣩��ʽ
<10> ��ƫ�ǣ�000.0~180.0�ȣ�ǰ���0Ҳ�������䣩
<11> ��ƫ�Ƿ���E��������W������
<12> ģʽָʾ����NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч��
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

extern GPGGA_DATA  GPGGAData;				   //GGA�ṹ�壬�洢����Э���е�����
extern GPRMC_DATA  GPRMCData;           //RMC�ṹ�壬�洢����Э���е�����
extern GETGPS_DATE_T GpsGet_Data;      //GPS��� 

void GPS_Rate_Config(void);
void GpsConfig(void);
void Flag_Clear(void);
void ProcessGPGGA(uint8_t *pData);
void ProcessGPRMC(uint8_t *pData);

#endif


