#ifndef  __SENDGPS_H
#define  __SENDGPS_H

#include "stdint.h"
#include "GetGPS.h"

static uint8_t	 GGA_TX_Max;        //ÿ�η������ݵĸ���
static uint8_t	 RMC_TX_Max;        //ÿ�η������ݵĸ���
static uint8_t	 VTG_TX_Max;        //ÿ�η������ݵĸ���

void ProcessSSFTOA(double GGAIData,uint8_t bit);
void GetGPRMC(GPRMC_DATA GPRMCData);
void GetLGPGGA(GPGGA_DATA GPGGAData);

#endif