#include "GetGPS.h"
#include "usart.h"
#include "delay.h"
#include "dma.h"
#include <string.h>
#include <stdlib.h>
#include "VariableType.h"


void SendConfigMessage(GPS_CONFIG get_state)
{
	static uint8_t  Com_TX_Num=0;
  static uint8_t  Gps_Command_Data[30]; 
	
	switch(get_state)
	{
		case GPS_CONFIG_PRT_SET:
			//B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 C2 01 00 07 
		  //00 03 00 00 00 00 00 C0 7E
			Gps_Command_Data[0] = 0XB5; 																		
			Gps_Command_Data[1] = 0x62; 
			Gps_Command_Data[2] = 0x06; 
			Gps_Command_Data[3] = 0x00; 
			Gps_Command_Data[4] = 0x14; 
			Gps_Command_Data[5] = 0x00; 
			Gps_Command_Data[6] = 0x01; 
			Gps_Command_Data[7] = 0x00; 
			Gps_Command_Data[8] = 0x00; 
			Gps_Command_Data[9] = 0x00; 
		
			Gps_Command_Data[10] = 0xD0;
			Gps_Command_Data[11] = 0x08;
			Gps_Command_Data[12] = 0x00;
			Gps_Command_Data[13] = 0x00;
			Gps_Command_Data[14] = 0x00;
			Gps_Command_Data[15] = 0xC2;
			Gps_Command_Data[16] = 0x01;
			Gps_Command_Data[17] = 0x00;
			Gps_Command_Data[18] = 0x07;
			
			Gps_Command_Data[19] = 0x00;			
			Gps_Command_Data[20] = 0x03;
			Gps_Command_Data[21] = 0x00;
			Gps_Command_Data[22] = 0x00;
			Gps_Command_Data[23] = 0x00; 
			Gps_Command_Data[24] = 0x00;
			Gps_Command_Data[25] = 0x00;
			Gps_Command_Data[26] = 0xC0;
			Gps_Command_Data[27] = 0x7E;
		
			Com_TX_Num = 28;
			
		break;
		
		case GPS_CONFIG_GGA_OPEN:
			//B5 62 06 01 08 00 F0 00 00 01 00 00 00 00 00 28
			Gps_Command_Data[0] = 0xB5; 
			Gps_Command_Data[1] = 0x62; 
			Gps_Command_Data[2] = 0x06; 
			Gps_Command_Data[3] = 0x01; 
			Gps_Command_Data[4] = 0x08; 
			Gps_Command_Data[5] = 0x00; 
			Gps_Command_Data[6] = 0xF0; 
			Gps_Command_Data[7] = 0x00; 
			Gps_Command_Data[8] = 0x00; 
			Gps_Command_Data[9] = 0x01; 
			Gps_Command_Data[10] = 0x00;
			Gps_Command_Data[11] = 0x00;
			Gps_Command_Data[12] = 0x00;
			Gps_Command_Data[13] = 0x00;
			Gps_Command_Data[14] = 0x00;
			Gps_Command_Data[15] = 0x28;
		
			Com_TX_Num = 16;
			
		break;
		
		case GPS_CONFIG_GST_OPEN:
			//B5 62 06 01 08 00 F0 07 01 01 01 01 01 00 0B 68
			Gps_Command_Data[0] = 0xB5;
			Gps_Command_Data[1] = 0x62;
			Gps_Command_Data[2] = 0x06;
			Gps_Command_Data[3] = 0x01;
			Gps_Command_Data[4] = 0x08;
			Gps_Command_Data[5] = 0x00;
			Gps_Command_Data[6] = 0xF0;
			Gps_Command_Data[7] = 0x07;
			Gps_Command_Data[8] = 0x01;
			Gps_Command_Data[9] = 0x01;
			Gps_Command_Data[10] = 0x01;
			Gps_Command_Data[11] = 0x01;
			Gps_Command_Data[12] = 0x01;
			Gps_Command_Data[13] = 0x00;
			Gps_Command_Data[14] = 0x0B;
			Gps_Command_Data[15] = 0x68;
		
			Com_TX_Num = 16;
			
		break;
		
		case GPS_CONFIG_GSV_CLOSE:
			//B5 62 06 01 08 00 F0 03 00 00 00 00 00 01 03 39
			Gps_Command_Data[0] = 0xB5;
			Gps_Command_Data[1] = 0x62;
			Gps_Command_Data[2] = 0x06;
			Gps_Command_Data[3] = 0x01;
			Gps_Command_Data[4] = 0x08;
			Gps_Command_Data[5] = 0x00;
			Gps_Command_Data[6] = 0xF0;
			Gps_Command_Data[7] = 0x03;
			Gps_Command_Data[8] = 0x00;
			Gps_Command_Data[9] = 0x00;
			Gps_Command_Data[10] = 0x00;
			Gps_Command_Data[11] = 0x00;
			Gps_Command_Data[12] = 0x00;
			Gps_Command_Data[13] = 0x01;
			Gps_Command_Data[14] = 0x03;
			Gps_Command_Data[15] = 0x39;
		
			Com_TX_Num = 16;
			
		break;
		
		case GPS_CONFIG_GSA_CLOSE:
			//B5 62 06 01 08 00 F0 02 00 00 00 00 00 01 02 32
			Gps_Command_Data[0] = 0xB5;
			Gps_Command_Data[1] = 0x62;
			Gps_Command_Data[2] = 0x06;
			Gps_Command_Data[3] = 0x01;
			Gps_Command_Data[4] = 0x08;
			Gps_Command_Data[5] = 0x00;
			Gps_Command_Data[6] = 0xF0;
			Gps_Command_Data[7] = 0x02;
			Gps_Command_Data[8] = 0x00;
			Gps_Command_Data[9] = 0x00;
			Gps_Command_Data[10] = 0x00;
			Gps_Command_Data[11] = 0x00;
			Gps_Command_Data[12] = 0x00;
			Gps_Command_Data[13] = 0x01;
			Gps_Command_Data[14] = 0x02;
			Gps_Command_Data[15] = 0x32;
		
			Com_TX_Num = 16;
		
		break;
		
		case GPS_CONFIG_VTG_CLOSE:
			//B5 62 06 01 08 00 F0 05 00 00 00 00 00 01 05 47
			Gps_Command_Data[0] = 0xB5;
			Gps_Command_Data[1] = 0x62;
			Gps_Command_Data[2] = 0x06;
			Gps_Command_Data[3] = 0x01;
			Gps_Command_Data[4] = 0x08;
			Gps_Command_Data[5] = 0x00;
			Gps_Command_Data[6] = 0xF0;
			Gps_Command_Data[7] = 0x05;
			Gps_Command_Data[8] = 0x00;
			Gps_Command_Data[9] = 0x00;
			Gps_Command_Data[10] = 0x00;
			Gps_Command_Data[11] = 0x00;
			Gps_Command_Data[12] = 0x00;
			Gps_Command_Data[13] = 0x01;
			Gps_Command_Data[14] = 0x05;
			Gps_Command_Data[15] = 0x47;
		
			Com_TX_Num = 16;
		
		break;
		
		case GPS_CONFIG_GLL_CLOSE:
			//B5 62 06 01 08 00 F0 01 00 00 00 00 00 01 01 2B
			Gps_Command_Data[0] = 0xB5;
			Gps_Command_Data[1] = 0x62;
			Gps_Command_Data[2] = 0x06;
			Gps_Command_Data[3] = 0x01;
			Gps_Command_Data[4] = 0x08;
			Gps_Command_Data[5] = 0x00;
			Gps_Command_Data[6] = 0xF0;
			Gps_Command_Data[7] = 0x01;
			Gps_Command_Data[8] = 0x00;
			Gps_Command_Data[9] = 0x00;
			Gps_Command_Data[10] = 0x00;
			Gps_Command_Data[11] = 0x00;
			Gps_Command_Data[12] = 0x00;
			Gps_Command_Data[13] = 0x01;
			Gps_Command_Data[14] = 0x01;
			Gps_Command_Data[15] = 0x2B;
		
			Com_TX_Num = 16;
		
		break;
		
		case GPS_CONFIG_RATE:
			//B5 62 06 08 06 00 C8 00 01 00 01 00 DE 6A
			Gps_Command_Data[0] = 0xB5;
			Gps_Command_Data[1] = 0x62;
			Gps_Command_Data[2] = 0x06;
			Gps_Command_Data[3] = 0x08;
			Gps_Command_Data[4] = 0x06;
			Gps_Command_Data[5] = 0x00;
			Gps_Command_Data[6] = 0xC8;
			Gps_Command_Data[7] = 0x00;
			Gps_Command_Data[8] = 0x01;
			Gps_Command_Data[9] = 0x00;
			Gps_Command_Data[10] = 0x01;
			Gps_Command_Data[11] = 0x00;
			Gps_Command_Data[12] = 0xDE;
			Gps_Command_Data[13] = 0x6A;
		
			Com_TX_Num = 14;
		
		break;
		default:  break;
	}
	
	HAL_UART_Transmit_DMA(&UART2_Handler,Gps_Command_Data,Com_TX_Num);
 	while(1)
	{
     if(__HAL_DMA_GET_FLAG(&UART2TxDMA_Handler,DMA_FLAG_TCIF2_6))//等待DMA2_Steam7传输完成
     {
       __HAL_DMA_CLEAR_FLAG(&UART2TxDMA_Handler,DMA_FLAG_TCIF2_6);//清除DMA2_Steam7传输完成标志
       HAL_UART_DMAStop(&UART2_Handler);      //传输完成以后关闭串口DMA
			break; 
     }   
	}
	Com_TX_Num = 0;
}

void GPS_Rate_Config(void)
{
	uart_init(9600);		        //串口初始化
	HAL_UART_Receive_IT(&UART2_Handler, (uint8_t*)getBuffer2, 1);
	
//	SendConfigMessage(GPS_CONFIG_GGA_OPEN);    //open GGA
//	delay_ms(50);
	
	SendConfigMessage(GPS_CONFIG_PRT_SET);  	  //设置端口115200
	delay_ms(50);

	uart_init(115200);		        //串口初始化
}

void GpsConfig(void)
{
	HAL_UART_Receive_IT(&UART2_Handler,getBuffer2,1);
/*
//打开RMC
	if(Gps_Init.RMC_Init_flag == 1)
	{
		SendConfigMessage(GPS_CONFIG_RMC_OPEN);    //open RMC
		delay_ms(50);
		if(GpsCon_Data.RMC_Config_flag)
		{
			Gps_Init.RMC_Init_flag = 0;
			printf("RMC OPEN  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("RMC OPEN  FAILED!\r\n");
			GpsConfig();
		}
	}	
	
//打开GGA
	if(Gps_Init.GGA_Init_flag == 1)
	{
		SendConfigMessage(GPS_CONFIG_GGA_OPEN);    //open GGA
		delay_ms(50);
		if(GpsCon_Data.GGA_Config_flag)
		{
			Gps_Init.GGA_Init_flag = 0;
			printf("GGA OPEN  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("GGA OPEN  FAILED!\r\n");
			GpsConfig();
		}
	}	
*/			
//打开GST
	if(Gps_Init.GST_Init_flag == 1)
	{
		SendConfigMessage(GPS_CONFIG_GST_OPEN);    //open GST
		delay_ms(50);
		if(GpsCon_Data.GST_Config_flag)
		{
			Gps_Init.GST_Init_flag = 0;
			printf("GST OPEN  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("GST OPEN  FAILED!\r\n");
			GpsConfig();
		}
	}	

//关闭GSV
	if(Gps_Init.GSV_Init_flag == 0)
	{
		SendConfigMessage(GPS_CONFIG_GSV_CLOSE);    //close GSV
		delay_ms(50);
		if(GpsCon_Data.GSV_Config_flag)
		{
			Gps_Init.GSV_Init_flag = 1;
			printf("GSV CLOSE  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("GSV CLOSE  FAILED!\r\n");
			GpsConfig();
		}
	}

//关闭GSA
	if(Gps_Init.GSA_Init_flag == 0)
	{
		SendConfigMessage(GPS_CONFIG_GSA_CLOSE);    //close GSA
		delay_ms(50);
		if(GpsCon_Data.GSV_Config_flag)
		{
			Gps_Init.GSA_Init_flag = 1;
			printf("GSA CLOSE  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}	
		else
		{
			printf("GSA CLOSE  FAILED!\r\n");
			GpsConfig();
		}
	}

//关闭VTG	
	if(Gps_Init.VTG_Init_flag == 0)
	{
		SendConfigMessage(GPS_CONFIG_VTG_CLOSE);    //close VTG
		delay_ms(50);
		if(GpsCon_Data.VTG_Config_flag)
		{
			Gps_Init.VTG_Init_flag = 1;
			printf("VTG CLOSE  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("VTG CLOSE  FAILED!\r\n");
			GpsConfig();
		}
	}

//关闭GLL	
	if(Gps_Init.GLL_Init_flag == 0)
	{
		SendConfigMessage(GPS_CONFIG_GLL_CLOSE);    //close GLL
		delay_ms(20);
		if(GpsCon_Data.GLL_Config_flag)
		{
			Gps_Init.GLL_Init_flag = 1;
			printf("GLL CLOSE  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("GLL CLOSE  FAILED!\r\n");
			GpsConfig();
		}
	}

//5HZ	
	if(Gps_Init.RATE_Init_flag == 1)
	{
		SendConfigMessage(GPS_CONFIG_RATE);    //close GLL
		delay_ms(20);
		if(GpsCon_Data.RATE_Config_flag)
		{
			Gps_Init.RATE_Init_flag = 0;
			printf("5HZ  SUCCESSFULLY!\r\n");
			Flag_Clear();
		}
		else
		{
			printf("5HZ  FAILED!\r\n");
			GpsConfig();
		}
	}

	
	HAL_DMA_DeInit(&UART2TxDMA_Handler); 
}

uint8_t GetField(uint8_t *pData, uint8_t *pField, int32_t nFieldNum, int32_t nMaxFieldLen)
{
	
	int32_t    Get_i;
  int32_t    Get_i2;
  int32_t    Get_nField;

	//
	// Validate params
	//
	if(pData == NULL || pField == NULL || nMaxFieldLen <= 0)
	{
		return FALSE;		   //函数立即结束，返回0
	}

	//
	// Go to the beginning of the selected field
	//
	Get_i = 0;
	Get_nField = 0;
	while(Get_nField != nFieldNum && pData[Get_i])	//直到nfiled到达nFieldNum，while结束,nFieldNum就是给出的要到达的字段的序号
	{
		if(pData[Get_i] == ',')		            //遇到逗号说明已经跳过一个字段，nField自加1
		{
			Get_nField++;
		}

		Get_i++;
		
			
		if(Get_i >=NP_MAX_DATA_LEN)               //如果Get_i超过数组长度，则....退出...20140729.....
		{
	    pField[0]  = '\0';			//无数值，那么pField[0]= 字符串结束			
			return FALSE;			   	//return退出当前函数		
    }

		if(pData[Get_i] == NULL)
		{
			pField[0] = '\0';			//无数值，那么pField[0]= 字符串结束
			return FALSE;			   	//return退出当前函数
		}
	}

	if(pData[Get_i] == ',' || pData[Get_i] == '*')
	{
		pField[0] = '\0';			   //若指定字段也无数值，为,或者*号则，
		return FALSE;				   //如果跳出while后的第一个字符还是,或者*，返回错误
	}

	//
	// copy field from pData to Field
	//
	Get_i2 = 0;
	while(pData[Get_i] != ',' && pData[Get_i] != '*' && pData[Get_i])  //若指定字段有数值
	{
		pField[Get_i2] = pData[Get_i];				//将其存放于pData中
		Get_i2++; Get_i++;

		//
		// check if field is too big to fit on passed parameter. If it is,
		// crop returned field to its max length.
		//
		if(Get_i2 >= nMaxFieldLen)			 //如果i2大于该字段应有的最大长度，那么让i2等于该最大长度
		{								 //因为在处理某些字段时，字段会多发数据，为了保证数据的准确
			Get_i2 = nMaxFieldLen-1;		 //这里规定了i2的长度，也就规定了最后返回字段的长度
			break;
		}
		
		if(Get_i >=NP_MAX_DATA_LEN)               //如果Get_i超过数组长度，则....退出...20140729.....
		{
	    pField[0]  = '\0';			//无数值，那么pField[0]= 字符串结束			
			return FALSE;			   	//return退出当前函数		
    }
	}
	pField[Get_i2] = '\0';

	return TRUE;						 //如果整个函数运行完成，字段返回成功，那么返回true=1
}

uint8_t DataJudge(uint8_t *pFieldData)
{
	uint8_t D_i=0,PDataT=0,DFlag=1;
		
	for(D_i=0;D_i<MAXFIELD;D_i++)
	{
		PDataT=pFieldData[D_i];
		
	 if(PDataT!='\0')
	 {
    if(((PDataT>=0x30)&&(PDataT<=0x39))||(PDataT=='.')||(PDataT=='-'))
	  {
     DFlag=1;
    }
	  else
	  {
     DFlag=0;
		 break;
    }
   }
	 else
	 {
		 if(D_i==0)
		 {
      DFlag=0;
     }		 
    break;
   }
 } 
 return  DFlag;	
}

void Flag_Clear(void)
{
	GpsCon_Data.RATE_Config_flag = 0;
	GpsCon_Data.RMC_Config_flag = 0;
	GpsCon_Data.GGA_Config_flag = 0;
	GpsCon_Data.GST_Config_flag = 0;
	GpsCon_Data.GSV_Config_flag = 0;
	GpsCon_Data.GSA_Config_flag = 0;
	GpsCon_Data.VTG_Config_flag = 0;
	GpsCon_Data.GLL_Config_flag = 0;
}

//------------------------------------
//void ProcessGPGGA(uint8_t *pData)
//------------------------------------
void ProcessGPGGA(uint8_t *pData)
{
uint8_t     GGApField[MAXFIELD];
char        GGApBuff[MAXFIELD];
	
static  uint8_t Gps_GError_Flag=0;
	//
	// Time
	//
	if(GetField(pData, GGApField, 0, MAXFIELD))
	{
		// Hour
		GGApBuff[0] = GGApField[0];
		GGApBuff[1] = GGApField[1];
		GGApBuff[2] = '\0';
		Gps_raw.gga_data.Hour = atoi(GGApBuff);		

		// minute
		GGApBuff[0] = GGApField[2];
		GGApBuff[1] = GGApField[3];
		GGApBuff[2] = '\0';
		Gps_raw.gga_data.Minute = atoi(GGApBuff);

		// Second
		GGApBuff[0] = GGApField[4];
		GGApBuff[1] = GGApField[5];
		GGApBuff[2] = '\0';
		Gps_raw.gga_data.Second = atoi(GGApBuff);

		// Second
		GGApBuff[0] = GGApField[7];
		GGApBuff[1] = GGApField[8];
		GGApBuff[2] = GGApField[9];
		GGApBuff[3] = '\0';
		Gps_raw.gga_data.MSecond = atoi(GGApBuff);
		
		if((Gps_raw.gga_data.Hour==0)&&(Gps_raw.gga_data.Minute==0)&&(Gps_raw.gga_data.Second==0))
		{
		 Gps_raw.gga_data.Utc_Flag=0;
		}
		else
		{
		 Gps_raw.gga_data.Utc_Flag=1;
		}
	}
	else
	{
	  Gps_raw.gga_data.Utc_Flag=0;
	}

	
	//
	// Latitude
	//
	if(GetField(pData, GGApField, 1, MAXFIELD))
	{

		
		if(DataJudge(GGApField))
		{
		 Gps_raw.gga_data.Latitude  = atof((char *)GGApField+2) / 60.0;
		 GGApField[2] = '\0';
		 Gps_raw.gga_data.Latitude += atof((char *)GGApField);		
		}
    else
	  {
     Gps_raw.gga_data.Latitude=0.0;
    }
	}
  else
	{
    Gps_raw.gga_data.Latitude=0.0;
  }

	
	if(GetField(pData, GGApField, 2, MAXFIELD))
	{
	
	   Gps_raw.gga_data.SNth=GGApField[0];

	   if(GGApField[0] == 'S')
	   {
		  Gps_raw.gga_data.Latitude = -Gps_raw.gga_data.Latitude;
	   }
	}
	


	//------------------------------------------------
	// Longitude
	//------------------------------------------------
	if(GetField(pData, GGApField, 3, MAXFIELD))
	{
		if(DataJudge(GGApField))
		{
		 Gps_raw.gga_data.Longitude = atof((char *)GGApField+3) / 60.0;
		 GGApField[3] = '\0';
		 Gps_raw.gga_data.Longitude += atof((char *)GGApField);	
		}
		else
		{
     Gps_raw.gga_data.Longitude=0.0;
    }
	}
	else
	{
    Gps_raw.gga_data.Longitude=0.0;
  }

	
	if(GetField(pData, GGApField, 4, MAXFIELD))
	{
	  Gps_raw.gga_data.WEst= GGApField[0]; 

		if(GGApField[0] == 'W')
		{
			Gps_raw.gga_data.Longitude = -Gps_raw.gga_data.Longitude;
		}	 
	}

		//------------------------------------------------
	// GPS quality
	//------------------------------------------------
	if(GetField(pData, GGApField, 5, MAXFIELD))
	{
		Gps_raw.gga_data.GPSQuality = GGApField[0] - '0';
	}
	else
	{
	  Gps_raw.gga_data.GPSQuality  = 0;
	}


	//------------------------------------------------
	// Satellites in use
	//------------------------------------------------
	if(GetField(pData, GGApField, 6, MAXFIELD))
	{
		GGApBuff[0] = GGApField[0];
		GGApBuff[1] = GGApField[1];
		GGApBuff[2] = '\0';
		Gps_raw.gga_data.NumOfSatsInUse = atoi(GGApBuff);
	}
	else
	{
	  Gps_raw.gga_data.NumOfSatsInUse =0;
	}

	//------------------------------------------------
	// HDOP
    //------------------------------------------------
	if(GetField(pData, GGApField, 7, MAXFIELD))
	{
		Gps_raw.gga_data.HDOP = atof((char *)GGApField);
	}


	//------------------------------------------------
	// Altitude
	//------------------------------------------------
	if(GetField(pData, GGApField, 8, MAXFIELD))
	{
		if(DataJudge(GGApField))
		{
		 Gps_raw.gga_data.Altitude = atof((char *)GGApField);
		}
	}

	//------------------------------------------------
	// M
	//------------------------------------------------
	if(GetField(pData, GGApField, 9, MAXFIELD))
	{
	    Gps_raw.gga_data.Auint= GGApField[0]; 
	}

	//------------------------------------------------
	//Geoidal
	//------------------------------------------------
	if(GetField(pData, GGApField, 10, MAXFIELD))
	{
	  Gps_raw.gga_data.Geoidal = atof((char *)GGApField);
	}

	//------------------------------------------------
	// M
	//------------------------------------------------
	if(GetField(pData, GGApField, 11, MAXFIELD))
	{
	  Gps_raw.gga_data.Guint= GGApField[0]; 
	}
	
	//------------------------------------------------
	//Age of Diff. Corr
	//------------------------------------------------
	if(GetField(pData, GGApField, 12, MAXFIELD))
	{
	  Gps_raw.gga_data.DTime = atof((char *)GGApField);

	}
	
	 
	//------------------------------------------------
	//Dif. Ref. Station ID
	//------------------------------------------------
	if(GetField(pData, GGApField, 13, MAXFIELD))
	{
	  Gps_raw.gga_data.DGpsID = atoi((char *)GGApField);	
		Gps_raw.gga_data.IDValid=1;
		
		Gps_raw.gga_data.DFID_Data[0] = GGApField[0];
		Gps_raw.gga_data.DFID_Data[1] = GGApField[1];
		Gps_raw.gga_data.DFID_Data[2] = GGApField[2];
		Gps_raw.gga_data.DFID_Data[3] = GGApField[3];
	}
	else
	{
    Gps_raw.gga_data.IDValid=0;
		Gps_raw.gga_data.DFID_Data[0] ='0';
		Gps_raw.gga_data.DFID_Data[1] ='0';
		Gps_raw.gga_data.DFID_Data[2] ='0';
		Gps_raw.gga_data.DFID_Data[3] ='0';	
		
  }	  

   if((Gps_raw.gga_data.SNth!='S')&&(Gps_raw.gga_data.SNth!='N'))
   {   
   	Gps_GError_Flag=1;
   }

   if((Gps_raw.gga_data.WEst!='W')&&(Gps_raw.gga_data.WEst!='E'))
   {					   
   	Gps_GError_Flag=1;  
   }

   if((Gps_raw.gga_data.Auint!='M')||(Gps_raw.gga_data.Guint!='M'))
   {					   
 	  Gps_GError_Flag=1;  
   }
	 


 	  Gps_raw.gga_data.Count++;
	  GpsGet_Data.GGA_Get_Flag  = 1;                 //获得数据---传输数据  
   
}

//------------------------------------
//void ProcessGPRMC(uint8_t *pData)
//------------------------------------
void ProcessGPRMC(uint8_t *pData)
{
uint8_t     RMCpField[MAXFIELD];
char        RMCpBuff[MAXFIELD];
static  uint8_t Gps_GError_Flag=0;
	//
	// Time
	//
	if(GetField(pData, RMCpField, 0, MAXFIELD))
	{
		// Hour
		RMCpBuff[0] = RMCpField[0];
		RMCpBuff[1] = RMCpField[1];
		RMCpBuff[2] = '\0';
		Gps_raw.rmc_data.Hour = atoi(RMCpBuff);

		// minute
		RMCpBuff[0] = RMCpField[2];
		RMCpBuff[1] = RMCpField[3];
		RMCpBuff[2] = '\0';
		Gps_raw.rmc_data.Minute = atoi(RMCpBuff);

		// Second
		RMCpBuff[0] = RMCpField[4];
		RMCpBuff[1] = RMCpField[5];
		RMCpBuff[2] = '\0';
		Gps_raw.rmc_data.Second = atoi(RMCpBuff);
		
	 // MSecond
		RMCpBuff[0] = RMCpField[7];
		RMCpBuff[1] = RMCpField[8];
		RMCpBuff[2] = RMCpField[9];
		RMCpBuff[3] = '\0';
		Gps_raw.rmc_data.MSecond = atoi(RMCpBuff);
		
		Gps_raw.rmc_data.TimeValid=1;
	}
	else
	{
	  Gps_raw.rmc_data.TimeValid=0;
	}

	//
	// Data valid
	//
	if(GetField(pData, RMCpField, 1, MAXFIELD))
	{
		Gps_raw.rmc_data.DataValid = RMCpField[0];
	}
	
	//
	// latitude
	//
	if(GetField(pData, RMCpField, 2, MAXFIELD))
	{

		if(DataJudge(RMCpField))
		{
		 Gps_raw.rmc_data.Latitude = atof((char *)RMCpField+2) / 60.0;    //把字符串转换成浮点数
		 RMCpField[2] = '\0';
		 Gps_raw.rmc_data.Latitude += atof((char *)RMCpField);
		}
		else
		{
			Gps_raw.rmc_data.Latitude=0.0;   //GetNass处理....
		}
	}
	else
	{
		Gps_raw.rmc_data.Latitude=0.0;   //GetNass处理....
	}
	
	if(GetField(pData, RMCpField, 3, MAXFIELD))
	{
				
	   Gps_raw.rmc_data.SNth=RMCpField[0];

		if(Gps_raw.rmc_data.SNth == 'S')
		{
		  Gps_raw.rmc_data.Latitude = -Gps_raw.rmc_data.Latitude;
		}
	 
	}

	//
	// Longitude
	//
	if(GetField(pData, RMCpField, 4, MAXFIELD))
	{
		if(DataJudge(RMCpField))
		{
		 Gps_raw.rmc_data.Longitude = atof((char *)RMCpField+3) / 60.0;
		 RMCpField[3] = '\0';
		 Gps_raw.rmc_data.Longitude += atof((char *)RMCpField);
		}
		else
		{
		 Gps_raw.rmc_data.Longitude=0.0;  //GetNass处理....
		}
	}
	else
	{
	 Gps_raw.rmc_data.Longitude=0.0;  //GetNass处理....
	}
		

	if(GetField(pData, RMCpField, 5, MAXFIELD))
	{

	   Gps_raw.rmc_data.WEst=RMCpField[0];

		if(Gps_raw.rmc_data.WEst == 'W')
		{
			Gps_raw.rmc_data.Longitude = -Gps_raw.rmc_data.Longitude;
		}
	 
	}
	   
	//
	// Date
	//
	if(GetField(pData, RMCpField, 8, MAXFIELD))
	{
		// Day
		RMCpBuff[0] = RMCpField[0];
		RMCpBuff[1] = RMCpField[1];
		RMCpBuff[2] = '\0';
		Gps_raw.rmc_data.Day = atoi(RMCpBuff);

		// Month
		RMCpBuff[0] = RMCpField[2];
		RMCpBuff[1] = RMCpField[3];
		RMCpBuff[2] = '\0';
		Gps_raw.rmc_data.Month = atoi(RMCpBuff);

		// Year (Only two digits. I wonder why?)
		RMCpBuff[0] = RMCpField[4];
		RMCpBuff[1] = RMCpField[5];
		RMCpBuff[2] = '\0';
		Gps_raw.rmc_data.Year = atoi(RMCpBuff);
		Gps_raw.rmc_data.Year += 2000;				// make 4 digit date -- What assumptions should be made here?
		
		Gps_raw.rmc_data.DayValid=1;
	}
	else
	{
	  Gps_raw.rmc_data.DayValid=0;
	}

	//
	// course over ground, degrees true
	//
	if(GetField(pData, RMCpField, 9, MAXFIELD))
	{
  		
		Gps_raw.rmc_data.MagVar = atof((char *)RMCpField);
		Gps_raw.rmc_data.MagVarValid=1;
	
	}
	else
	{
	  Gps_raw.rmc_data.MagVarValid=0;
  }
	
	if(GetField(pData, RMCpField, 10, MAXFIELD))
	{
	  if(RMCpField[0] == 'W')
	   {
	    Gps_raw.rmc_data.MagVar = -Gps_raw.rmc_data.MagVar;
	   }
		 
		 Gps_raw.rmc_data.MagWEst=RMCpField[0];
	}

	if(GetField(pData, RMCpField, 11, MAXFIELD))
	{
	   Gps_raw.rmc_data.ModeIn = RMCpField[0];	   
	}
	
	//
	// Ground speed
	//
	if(GetField(pData, RMCpField, 6, MAXFIELD))
	{
		if(DataJudge(RMCpField))
		{
		 Gps_raw.rmc_data.GroundSpeed = atof((char *)RMCpField);
		 Gps_raw.rmc_data.SpeedValid = 1;
		}
		else
		{
		 Gps_raw.rmc_data.SpeedValid  = 0;
		 Gps_raw.rmc_data.CourseValid=0;
     Gps_raw.rmc_data.ModeIn='N';
    }
	}
	else
	{
		Gps_raw.rmc_data.SpeedValid  = 0;
	}

	//
	// course over ground, degrees true
	//
	if(GetField(pData, RMCpField, 7, MAXFIELD))
	{
		if(DataJudge(RMCpField))
		{
		 Gps_raw.rmc_data.Course = atof((char *)RMCpField);
		 Gps_raw.rmc_data.CourseValid=1;
		}
		else
		{
		 Gps_raw.rmc_data.CourseValid=0;
     Gps_raw.rmc_data.ModeIn='N';
    }
	}
	else
	{
	  Gps_raw.rmc_data.CourseValid=0;
  }
  //--------------------------------------------------
  // 纠错
  //--------------------------------------------------

  if((Gps_raw.rmc_data.SNth!='S')&&(Gps_raw.rmc_data.SNth!='N'))
   {   
   	Gps_GError_Flag=1;
   }

   if((Gps_raw.rmc_data.WEst!='W')&&(Gps_raw.rmc_data.WEst!='E'))
   {					   
 	  Gps_GError_Flag=1;  
   }
	 
	// if(Gps_GError_Flag==0)
   {
  	Gps_raw.rmc_data.Count++;   							
//	  GpsGet_Data.RMC_Get_Flag  = 1;						  //	
//	  GpsGet_Data.Rmc_Vtg_Flag  = 1;              // RMC模式...  
   }

		 
    Gps_GError_Flag  = 0;
	 
		GpsGet_Data.RMC_Get_Flag = 1;
}



