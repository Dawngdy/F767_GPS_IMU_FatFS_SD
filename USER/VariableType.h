#ifndef  __VARIABLETYPE_H
#define  __VARIABLETYPE_H

#include "stdint.h"
#include "GetGPS.h"

typedef struct _CONFIG_DATE_T
{
	uint8_t RATE_Config_flag;
	uint8_t RMC_Config_flag;
	uint8_t GGA_Config_flag;
	uint8_t GST_Config_flag;
	uint8_t GSV_Config_flag;
	uint8_t GSA_Config_flag;
	uint8_t VTG_Config_flag;
	uint8_t GLL_Config_flag;

}CONFIG_DATE_T, *PCONFIG_DATE_T;

typedef struct _CONFIG_GPSInit_T
{
	uint8_t RATE_Init_flag;
	uint8_t RMC_Init_flag;
	uint8_t GGA_Init_flag;
	uint8_t GST_Init_flag;
	uint8_t GSV_Init_flag;
	uint8_t GSA_Init_flag;
	uint8_t VTG_Init_flag;
	uint8_t GLL_Init_flag;
	
}CONFIG_GPSInit_T, *PCONFIG_GPSInit_T;

typedef struct _RECEIVE_RAW_NMEA
{
	GPGGA_DATA gga_data;
	GPRMC_DATA rmc_data;
	uint8_t gga_buf[200];
	uint8_t rmc_buf[200];
	uint8_t gst_buf[200];
	uint8_t gga_receive[200];
	uint8_t rmc_receive[200];
	
}RAW_NMEA, *RECEIVE_DATA_T;


extern CONFIG_DATE_T      GpsCon_Data;
extern CONFIG_GPSInit_T   Gps_Init;
extern RAW_NMEA Gps_raw;

#endif



