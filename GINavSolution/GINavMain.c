#include "stdio.h"
#include "GINavMain.h"
#include "GlobalVars.h"
#include "DataProc.h"
#include "InsAlign.h"
#include "InsNav.h"
#include "GIFilter.h"
#include "BasicFunc.h"
#include "SendGPS.h"
#include <string.h>
#include <math.h>


void GINavInit(void)
{

	MEMSET(&g_GINavInfo, 0, SIZEOF(GINAV_INFO_T));
	g_GINavInfo.INSState = INS_ACTIVE;

	g_GINavInfo.ImuCfg.InstallMat.C11 = g_GINavInfo.ImuCfg.InstallMat.C22 = g_GINavInfo.ImuCfg.InstallMat.C33 = 1.0;

	g_GINavInfo.Ini0_Mat.C11 = g_GINavInfo.Ini0_Mat.C22 = g_GINavInfo.Ini0_Mat.C33 = 1.0;
	g_GINavInfo.Ini1_Mat.C11 = g_GINavInfo.Ini1_Mat.C22 = g_GINavInfo.Ini1_Mat.C33 = 1.0;
	g_GINavInfo.Ini2_Mat.C11 = g_GINavInfo.Ini2_Mat.C22 = g_GINavInfo.Ini2_Mat.C33 = 1.0;
	g_GINavInfo.Ini3_Mat.C11 = g_GINavInfo.Ini3_Mat.C22 = g_GINavInfo.Ini3_Mat.C33 = 1.0;
	g_GINavInfo.Ini4_Mat.C11 = g_GINavInfo.Ini4_Mat.C22 = g_GINavInfo.Ini4_Mat.C33 = 1.0;
	g_GINavInfo.Ini5_Mat.C11 = g_GINavInfo.Ini5_Mat.C22 = g_GINavInfo.Ini5_Mat.C33 = 1.0;
	g_GINavInfo.Ini6_Mat.C11 = g_GINavInfo.Ini6_Mat.C22 = g_GINavInfo.Ini6_Mat.C33 = 1.0;



	g_GINavInfo.Ini1_Flag = 0;
	g_GINavInfo.Ini2_Flag = 0;
	g_GINavInfo.Ini3_Flag = 0;
	g_GINavInfo.Ini4_Flag = 0;

	DataProcerInit();
	GIKFInit();
		

	
}












//-------------------------------
//Low-Speed
//-------------------------------
void GetGnssGst(PGNSS_DATA_T pGnssData)
{
	FLOAT32  GST_Deta, GST_Diff;
	STATIC FLOAT32   GST_DetaD[20];
	STATIC U8  GST_Num = 0;

	STATIC U16 StaticTimeMax = 0, ScaleMax;
	U8 i;


	GST_Deta = sqrt(pGnssData->GstDetaLon*pGnssData->GstDetaLon + pGnssData->GstDetaLat*pGnssData->GstDetaLat);
	
	if(GST_Deta<0.01)
	{
	  GST_Deta=100;
	}

	if (g_GINavInfo.GstDetaMin <= 0)
	{
		g_GINavInfo.GstDetaMin = 5.0f;
	}

	//g_GINavInfo.GstDetaMin = 0.5;

	if (GST_Deta>0.0f)
	{
		if (GST_Deta<99)
		{
			g_GINavInfo.GstDeta = GST_Deta;
		}
		else
		{
			g_GINavInfo.GstDeta = 99;
			g_GINavInfo.GstStatus = 0;
		}
	}
	else
	{
		g_GINavInfo.GstStatus = 0;
	}

	//----------------------------------------------------
	// 计算卫星导航质量标志
	//----------------------------------------------------
	if (g_GINavInfo.GstFlag == 0)
	{
		if (g_GINavInfo.GstDeta<10.0f)
		{
			g_GINavInfo.GstFlag = 1;
			g_GINavInfo.GstStatus = 0;
			g_GINavInfo.GstDetaMin = g_GINavInfo.GstDeta;
		}
		else
		{
			g_GINavInfo.GstStatus = 0;
		}

		//GST_Diff = 0;
	}
	else
	{
		if (g_GINavInfo.GstDeta<g_GINavInfo.GstDetaMin)
		{
			g_GINavInfo.GstDetaMin = g_GINavInfo.GstDeta;
		}

		if (g_GINavInfo.GstDeta<g_GINavInfo.GstDetaMin*1.5f)
		{
			g_GINavInfo.GstStatus = 4;
		}
		else if (g_GINavInfo.GstDeta<g_GINavInfo.GstDetaMin * 4)
		{
			g_GINavInfo.GstStatus = 5;
		}
		else if (g_GINavInfo.GstDeta<g_GINavInfo.GstDetaMin * 8)
		{
			g_GINavInfo.GstStatus = 2;
		}
		else if (g_GINavInfo.GstDeta<g_GINavInfo.GstDetaMin * 20)
		{
			g_GINavInfo.GstStatus = 1;
		}
		else
		{
			g_GINavInfo.GstStatus = 0;
		}



	

		//-----------------------------------------------------
		//  计算2秒内GST差，用于GNSS评估....
		//-----------------------------------------------------
		if (GST_Num<20)
		{
			GST_DetaD[GST_Num] = g_GINavInfo.GstDeta;
			GST_Num++;
			GST_Diff = 0;
		}
		else
		{
			for (i = 1; i<20; i++)
			{
				GST_DetaD[i - 1] = GST_DetaD[i];
			}

			GST_DetaD[19] = g_GINavInfo.GstDeta;

			GST_Diff = GST_DetaD[19] - GST_DetaD[0];
		}
		g_GINavInfo.GST_Diff = GST_Diff;
	}


}
//---------------------------------------------
//void GetLine(PIMU_DATA_T pImuData)
//---------------------------------------------
void GetLine(PIMU_DATA_T pImuData)
{

	STATIC FLOAT32 GyrData[5];
	STATIC U8 GyrHighNum = 0, GyrLowNum = 0, GyrIndex = 0;

	FLOAT32 GyrTtemp;
	U8 i;


	GyrTtemp = pImuData->Gyro[0][2] * RAD2DEG;
	GyrTtemp /= INS_UPDATE_SAMPLE_NUM*pImuData->MsrInterval / 1000.0;

	g_GINavInfo.Gyr_Rate = GyrTtemp;



	if (ABS(g_GINavInfo.Gyr_Rate)<3)
	{
		GyrLowNum++;
		GyrHighNum = 0;
	}
	else
	{
		GyrHighNum++;
		if (GyrHighNum>2)
		{
			GyrHighNum = 2;
			GyrLowNum = 0;
		}
	}

	if (GyrLowNum >= (g_GINavInfo.Frenquecy / 2) * 5)
	{
		GyrLowNum = (g_GINavInfo.Frenquecy / 2) * 5;
		g_GINavInfo.LineFlag = 1;
	}
	else
	{
		g_GINavInfo.LineFlag = 0;
	}


	//-----------------------------------------------
	if (GyrIndex<5)
	{
		GyrData[GyrIndex] = g_GINavInfo.Gyr_Rate;
		GyrIndex++;
	}
	else
	{
		for (i = 1; i<5; i++)
		{
			GyrData[i - 1] = GyrData[i];
		}

		GyrData[4] = g_GINavInfo.Gyr_Rate;

		g_GINavInfo.Gyr_Diff = (GyrData[4] - GyrData[1]) / 0.3f;
	}

}




//---------------------------------------------
//BOOL GINavProc(POUTPUT_INFO_T pNavResult
//---------------------------------------------

BOOL GINavProc(POUTPUT_INFO_T pNavResult)
{	
	BOOL UpdateByGNSS=FALSE;	


	PGNSS_DATA_T   pGnssData;
	
  U16 i=0,j=0;

	STATIC U8    TimeNum=0;
	STATIC U8    NavStatus,GNSS_Run_Flag,ZeroSecond;



//----------------------------------

	PIMU_DATA_T pImuData = GetIMUData();      //gty ?IMUDataBuffer??IMU??
	                                          //??IMU????pImuData??IMU??
	                                          //??IMU????pImuData??NULL
	

	if (!pImuData || g_GINavInfo.INSState == INS_INACTIVE)  
		return FALSE;


	
	
	pGnssData = GetGNSSData();                 //gty ?GNSSDataBuffer??GPS?? 
	                                           //??GNSS????pGnssData??Gps??
											                       //??GNSS????pGnssData??Null  
	

	if(GNSS_Run_Flag==1)                       //??GNSS_Run_Flag?1???????GNSS????....
	{
	 GNSS_Run_Flag=0;
	 return FALSE;
  }
	
	GNSS_Run_Flag=1;                           //??GNSS??....
	
//------------------------------------------------------------------
  if(pGnssData)
	{
		
		
		g_GINavInfo.NavStatus = pGnssData->NavStatus;


		//-----------------------------------------------
		// ??GST
		//-----------------------------------------------

		GetGnssGst(pGnssData);


	
	 
	 //----------------------------------------------
	 //
	 //----------------------------------------------
		
    if((pGnssData->UtcTime.Hour==0)&&(pGnssData->UtcTime.Minute==0)&&(pGnssData->UtcTime.Second==0))
		{
      TimeNum++; 
      if(TimeNum>=5)
		  {
       TimeNum=0;
			 g_GINavInfo.ZeroSecond=1;
      }
			else
			{
			 g_GINavInfo.ZeroSecond=0;
      }
    }       
		else
		{
	   if(pGnssData->UtcTime.MillSecond==0)
	   {
      g_GINavInfo.ZeroSecond=1;
     }   
	   else
	   {
      g_GINavInfo.ZeroSecond=0;
     }		
	  }
		
		//-----------------------------------------------
    //   ......计算角度阈值....
		//-----------------------------------------------

		NavStatus = g_GINavInfo.GstStatus;

		}
	//------------------------------------------------------------------	

	
	//UTC和GPS时间更新
	g_GINavInfo.Tag++;                        //gty 该变量一直在增加....
	MEMCPY(&g_GINavInfo.UtcTime, &pImuData->UtcTime, sizeof(UTC_T));
	//GetGpsTime(&pImuData->UtcTime, &g_GINavInfo.GPSTime);             //gty 计算GPS周和周秒...


	//忽略刚开机时的0.5s数据
	//	if (g_GINavInfo.Tag <= INS_UPDATE_RATE/2)             //gty？ 不担心g_GINavInfo.Tag溢出？？？从而小于INS_UPDATE_RATE/2
	//		goto END;


  //------------------------
	//静态检测
	DynamicModeIdentify(pImuData);                         //保持原来坐标系进行判断，不影响任何效果

	
	//---------------------------------------
	// 
	//---------------------------------------
  GetLine(pImuData);
	   
		 
  //-------------------------------------------------------------
  //固定安装角
  //-------------------------------------------------------------

	 g_GINavInfo.Ini4_Flag = 1;	 
		

 
	if (!IS_INS_ALIGNED(g_GINavInfo.INSState))      //gty，    如果姿态，位置速度，方向启动，则完成初始化...
	{	
			if(g_GINavInfo.ImuCfg.InstallMatInitFlag == 0)
			{				
				InitInstallMat(pImuData);                           //如果InstallMatInitFlag=1  则刚刚初始化安装矩阵
			}
			else
			{
				g_GINavInfo.ImuCfg.InstallMatInitFlag <<= 1;      //gty 如果InstallMatInitFlag=3，则认为IMU补偿完成....
				g_GINavInfo.ImuCfg.InstallMatInitFlag |= 0x01;

			}
			
	   
		
		
		//初始化陀螺Bias
		InitGyroBias(pImuData, pGnssData);

		//INS初始对准
		INSAlign(pImuData, pGnssData);
		
		if (IS_INS_ALIGNED(g_GINavInfo.INSState))    //gty 第一次启动...则初始化...
		{//对准完成，初始化Kalman
			GIKFInitPMatrix();
			g_GINavInfo.INSAloneMsCount = 200000;
			g_GINavInfo.KFCount = 0;
			g_GINavInfo.GNSSHaltCount = 0;
		}	
		
	  //------------------------------------------------------------

   	g_GINavInfo.Jugde     = 1;
	}
	else
	{
		//INS航向确认
		if ((g_GINavInfo.INSState & INS_HEADING_GOOD) == 0)    //gty 初始化完成后，在一段速度区间内，gps方向角初始化INS方向....
			ConfirmHeading(pGnssData);

		 //---------------------------------------
	   //IMU补偿，为组合导航服务
		 //---------------------------------------
	   IMUCompensate(pImuData);                               //gty IMU安装角矩阵补偿，并且完成偏置补偿....



	
		//----------------------------------------
		//  位置备份
		//----------------------------------------


		//捷联解算---17ms

		INSUpdate(pImuData, TRUE, TRUE, TRUE);                        //1 5ms


		//计算PHI阵
		GIKFCalcPHIMatrix(pImuData->MsrInterval*INS_UPDATE_SAMPLE_NUM); //2

		//P阵预测
		GIKFPredictPMatrix(pImuData->MsrInterval*INS_UPDATE_SAMPLE_NUM);//3  1+2+3=15

		if (g_GINavInfo.StaticCount == 0)
			g_GINavInfo.INSAloneMsCount += pImuData->MsrInterval*INS_UPDATE_SAMPLE_NUM;  //如果动态，则INS参与运算时间增加100ms,



		if (g_GINavInfo.INSAloneMsCount>500000)
			g_GINavInfo.INSAloneMsCount = 500000;

		g_GINavInfo.GNSSHaltCount++;


		UpdateByGNSS = !GIKFUpdateByGNSS(pGnssData, pImuData);  //26ms		



		//Kalman量测更新
		if (UpdateByGNSS )   //gty? 如果有一个失败的，则INSAloneMsCount清零，并置位INSState.....
		{
			g_GINavInfo.KFCount = 0;
			g_GINavInfo.INSAloneMsCount = 0;
			g_GINavInfo.INSState = INS_ACTIVE;


		}


  	
		

		//导航质量控制
		if (g_GINavInfo.INSAloneMsCount <= 10000 && g_GINavInfo.KFCount > 60 && (g_GINavInfo.INSState & INS_HEADING_GOOD) != 0)
			g_GINavInfo.PositionQuality = Excellent;
		else if (g_GINavInfo.INSAloneMsCount <= 100000 && g_GINavInfo.KFCount > 60 && (g_GINavInfo.INSState & INS_HEADING_GOOD) != 0)
			g_GINavInfo.PositionQuality = Good;
		else if (g_GINavInfo.INSAloneMsCount <= 800000)
			g_GINavInfo.PositionQuality = Bad;
		else
		{
			g_GINavInfo.PositionQuality = Unknow;
			//g_GINavInfo.INSState = INS_ACTIVE;
		}

	}

END:
	//保留上一秒最后一个采样点的IMU数据用于下次捷联更新
	MEMCPY(g_GINavInfo.LastGyro, pImuData->Gyro[INS_UPDATE_SAMPLE_NUM - 1], sizeof(FLOAT64)* 3);
	MEMCPY(g_GINavInfo.LastAcc, pImuData->Acc[INS_UPDATE_SAMPLE_NUM - 1], sizeof(FLOAT64)* 3);
	
	//?????

	

	
  pNavResult->Jugde               = g_GINavInfo.Jugde;	
	pNavResult->GyroBiasIsConverage =  g_GINavInfo.ImuCfg.GyroBiasIsConverage;
  pNavResult->GpsStatus           =  g_GINavInfo.GpsStatus;
	pNavResult->GpsInsGetFlag       =  1; 


 //----------------------------------------------------
 //	                        ??
 //----------------------------------------------------

	 memset(&IMUDataBuffer, 0, sizeof(IMU_FRAME_T));  //????
	
	 
	if(pGnssData)
	{
		//-------------------------------------------------------------
		
     memset(&GNSSDataBuffer, 0, sizeof(GNSS_DATA_T));  //
		
//	 	if((Select_Data.DebSelect)||(Select_Data.HighSelect))
//		{
//	    	pNavResult->Get_Gnss_Flag       =  1;  
//		}
//    else
//		{			
     if(g_GINavInfo.ZeroSecond)               //1HZ
		 {
 	       pNavResult->Get_Gnss_Flag       =  1; 
     }
//	  }     
	}

	//----------------------------------------------------
  //	               ??IMU????
  //----------------------------------------------------	
	
	 GNSS_Run_Flag = 0;           //??GNSS??.......
		


	return TRUE;
	
}
