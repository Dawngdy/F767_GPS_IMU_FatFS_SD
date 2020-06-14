#include "INSAlign.h"
#include "BasicFunc.h"
#include "GlobalVars.h"
#include "Const.h"
#include "InsNav.h"
#include <string.h>
#include <math.h>


//��ʼ��������Ư
void InitGyroBias(PIMU_DATA_T pImuData,PGNSS_DATA_T pGnssData)
{
	S32 i;
	STATIC U32 SmoothCount = 0;	
	FLOAT64 temp[3] = { 0.0 }, dt = pImuData->MsrInterval / 1000.0;

	if (g_GINavInfo.StaticCount == 0)           //gty �����̬���򷵻�.....
		return;
	
	for (i = 0; i < INS_UPDATE_SAMPLE_NUM; i++)
	{
		temp[0] += pImuData->Gyro[i][0];
		temp[1] += pImuData->Gyro[i][1];
		temp[2] += pImuData->Gyro[i][2];
	}
	temp[0] /= dt*INS_UPDATE_SAMPLE_NUM;
	temp[1] /= dt*INS_UPDATE_SAMPLE_NUM;
	temp[2] /= dt*INS_UPDATE_SAMPLE_NUM;
		  

	SmoothCount++;
	SmoothCount = MIN(SmoothCount, 30*INS_UPDATE_RATE);  //gty �����������30���ƽ��....
	for (i = 0; i < 3; i++)
	{
		g_GINavInfo.ImuCfg.GyroBias[i] -= g_GINavInfo.ImuCfg.GyroBias[i] / SmoothCount;
		g_GINavInfo.ImuCfg.GyroBias[i] += temp[i] / SmoothCount;
	}

	if (g_GINavInfo.ImuCfg.InstallMatInitFlag == 0x03)
	{
		MEMCPY(temp, g_GINavInfo.ImuCfg.GyroBias, sizeof(FLOAT64)* 3);
		CMRotation(&g_GINavInfo.ImuCfg.InstallMat, temp, g_GINavInfo.ImuCfg.GyroBias);  //gty�� ���ﻹ��Ҫ��ת�𣿣�
	}

}

STATIC FLOAT64 AccSmoother[3];
void INSAlign(PIMU_DATA_T pImuData, PGNSS_DATA_T pGnssData)
{
	S32 i, SmoothCount;
	FLOAT64 SumAcc[3] = { 0 }, temp[3];
	FLOAT64 HeadingTemp;
	U8  P_RunFlag;
	STATIC U8 Att_Flag = 0, StartNum = 0,AStartNum = 0;

	if ((g_GINavInfo.StaticCount > 5) && ((g_GINavInfo.ImuCfg.InstallMatInitFlag >= 0x03)) && (g_GINavInfo.INSState & INS_LEVELATT_GOOD) == 0)  //gty �����ֹ�����ǻ�û��ʵ��INS Level Attitude Initialized and good
	{

		for (i = 0; i < INS_UPDATE_SAMPLE_NUM; i++)
		{
			SumAcc[0] += pImuData->Acc[i][0];
			SumAcc[1] += pImuData->Acc[i][1];
			SumAcc[2] += pImuData->Acc[i][2];
		}
		SmoothCount = MIN(g_GINavInfo.StaticCount, STATIC_ALIGN_SMOOTHOR_LEN);

		AccSmoother[0] += (SumAcc[0] - AccSmoother[0]) / SmoothCount;
		AccSmoother[1] += (SumAcc[1] - AccSmoother[1]) / SmoothCount;
		AccSmoother[2] += (SumAcc[2] - AccSmoother[2]) / SmoothCount;

		//��������ˮƽ��̬

		if(g_GINavInfo.EInstallMatInitFlag==1)
		{
		  g_GINavInfo.Euler.Theta = 0;                   //gty �����Ǽ���
		  g_GINavInfo.Euler.Gamma =0;                    //gty������Ǽ��㷽���е�֣���������ĺô��ǣ���		
		}
		else
		{
		  temp[0] = sqrt(SQR(AccSmoother[0]) + SQR(AccSmoother[1]) + SQR(AccSmoother[2]));
		  g_GINavInfo.Euler.Theta = asin(AccSmoother[0] / temp[0]);                                   //gty �����Ǽ���
		  g_GINavInfo.Euler.Gamma = -asin(AccSmoother[1] / (cos(g_GINavInfo.Euler.Theta)*temp[0]));  //gty������Ǽ��㷽���е�֣���������ĺô��ǣ���
		}
		
		Euler2Quat(&g_GINavInfo.Euler, &g_GINavInfo.Quat_bn);
		Quat2CM(&g_GINavInfo.Quat_bn, &g_GINavInfo.CM_bn);
		if (g_GINavInfo.StaticCount > STATIC_ALIGN_THD)//ƽ�������㹻ʱ����Ϊˮƽ��̬��ʼ�����
		{
			g_GINavInfo.INSState |= INS_LEVELATT_GOOD;
			g_GINavInfo.SYSFlag  = 1;
			Att_Flag=1;
		}
	}
	else
	{  //����ƽ���˲���
		AccSmoother[0] = AccSmoother[1] = AccSmoother[2] = 0.0;
	}



	 if (pGnssData && IS_POS_VALID(pGnssData->NavFlag) && (pGnssData->SatUseNum>3) )   //���GPS��γ�߶���ά��λ�����ԣ���....
	 { 
		//��GNSS���ݿ���ʱ������λ���ٶȸ���INS
		MEMCPY(&g_GINavInfo.Position, &pGnssData->Position, SIZEOF(POS_T));
		GetNavFrameQuat(&g_GINavInfo.Position, &g_GINavInfo.Quat_ne);
		Quat2CM(&g_GINavInfo.Quat_ne, &g_GINavInfo.CM_ne);
		 
		MEMCPY(&g_GINavInfo.Velocity, &pGnssData->Velocity, SIZEOF(VEL_T));   //���ٶȸ��Ƹ�INS

		g_GINavInfo.Gravity = GetLocalGravity(-g_GINavInfo.CM_ne.C33, g_GINavInfo.Position.Alt);
		temp[0] = sqrt(1.0 - WGS_E1_SQR * g_GINavInfo.CM_ne.C33 * g_GINavInfo.CM_ne.C33);
		g_GINavInfo.Rm = WGS_AXIS_A * (1 - WGS_E1_SQR) / (temp[0] * temp[0] * temp[0]);
		g_GINavInfo.Rn = WGS_AXIS_A / temp[0];
		g_GINavInfo.Wie[0] = WGS_OMEGDOTE * g_GINavInfo.CM_ne.C31;
		g_GINavInfo.Wie[1] = 0;
		g_GINavInfo.Wie[2] = WGS_OMEGDOTE * g_GINavInfo.CM_ne.C33;
	

		
		//-------------------------------------
		if (IS_LEVEL_VEL_VALID(pGnssData->NavFlag)&&(g_GINavInfo.GstStatus >= 2))      //���GPSˮƽ�ٶȺͷ��򶼿��ԣ���.....
		{
			MEMCPY(&g_GINavInfo.Velocity, &pGnssData->Velocity, SIZEOF(VEL_T));   //���ٶȸ��Ƹ�INS
			temp[0] = sqrt(SQR(pGnssData->Velocity.Ve) + SQR(pGnssData->Velocity.Vn));
			
			g_GINavInfo.KFCount=0;	
			
			//-----------------------------------------------
			//GNSS�ٶȴ������ޣ����㺽�򣬶�׼���
			//-----------------------------------------------
			if ((temp[0] > ALIGN_HEADING_VEL_THD)&&((g_GINavInfo.ImuCfg.InstallMatInitFlag >= 0x03)))  //
			{
				AStartNum++;
			}
			else
			{
			  AStartNum=0;
			}
			
	

			//-----------------------------------------------
			//GNSS�ٶȴ������ޣ����㺽�򣬶�׼���
			//-----------------------------------------------
			if ((temp[0] > ALIGN_HEADING_VEL_THD)&&((g_GINavInfo.ImuCfg.InstallMatInitFlag >= 0x03))&&(g_GINavInfo.Ini4_Flag==1))  //
			{
			   StartNum++;
			}
			else
			{
			   StartNum=0;
			}
				
			//-----------------------------------------------
			if(StartNum>30)
			{
				StartNum=30;
				
	       HeadingTemp = atan2(g_GINavInfo.Velocity.Ve, g_GINavInfo.Velocity.Vn);//gty �Ѻ���Ǹ��Ƹ�INS
				
				if(ABS(HeadingTemp*RAD2DEG)<=360)
				{					
				g_GINavInfo.Euler.Phi = atan2(g_GINavInfo.Velocity.Ve, g_GINavInfo.Velocity.Vn);//gty ����ǳ�ʼ��,��ʼ����һ����//

				if ((g_GINavInfo.INSState & INS_LEVELATT_GOOD) == 0)                  //gty�������û�о�������̬��ʼ����̬������̬����.....
				{
					g_GINavInfo.Euler.Gamma = 0;                                      //gty  ����ǽ��г�ʼ��..
					g_GINavInfo.Euler.Theta = 0;                                       //gty? �����ǽ��г�ʼ��...
					g_GINavInfo.INSState |= INS_LEVELATT_GOOD;
				}

				Euler2Quat(&g_GINavInfo.Euler, &g_GINavInfo.Quat_bn);
				Quat2CM(&g_GINavInfo.Quat_bn, &g_GINavInfo.CM_bn);

				g_GINavInfo.dVelocity.Ve = g_GINavInfo.dVelocity.Vn = g_GINavInfo.dVelocity.Vu = 0.0;      //gty �ٶ����Ϊ�㣬�Ӷ�....
				g_GINavInfo.SF_n[0] = g_GINavInfo.SF_n[1] = g_GINavInfo.SF_n[2] = 0.0;

				g_GINavInfo.Wen[0] = g_GINavInfo.Velocity.Ve / (g_GINavInfo.Rn + g_GINavInfo.Position.Alt); // Ve / (N+h)
				g_GINavInfo.Wen[1] = -g_GINavInfo.Velocity.Vn / (g_GINavInfo.Rm + g_GINavInfo.Position.Alt); // -Vn / (M+h)
				g_GINavInfo.Wen[2] = g_GINavInfo.Velocity.Ve*g_GINavInfo.CM_ne.C33 / (g_GINavInfo.CM_ne.C31*(g_GINavInfo.Rn + g_GINavInfo.Position.Alt)); // -Ve*tan(lat) / (N+h)
				
			
				 g_GINavInfo.INSState |= INS_HEADING_INIT; //�����ʼ����ʼ....
				 StartNum=0;
				
			 }
     }		
		}
		
	
	
		  g_GINavInfo.INSState |= INS_POSVEL_GOOD;  //gty ��ʼ����λ�ú��ٶ�....
		  g_GINavInfo.PositionQuality = Bad;
		  g_GINavInfo.SYSFlag  = 2;		

	
		//--------------------------------------
		
	 }
 
 
}

//�ڶ�׼��ɺ�ļȶ��ٶ������ڣ�ʼ������GNSS�ٶȼ��㺽�򲢸���INS
void ConfirmHeading(PGNSS_DATA_T pGnssData)
{
	FLOAT64 VLevel,HeadingTemp,delta_Heading;
	U8 RunFlag;
	static U16 HeadNum;

	if(g_GINavInfo.GstStatus>=1)
		RunFlag=1;
	else
		RunFlag=0;

	if (!pGnssData || !IS_LEVEL_VEL_VALID(pGnssData->NavFlag)||(RunFlag==0))
		return;

	VLevel = sqrt(SQR(pGnssData->Velocity.Ve) + SQR(pGnssData->Velocity.Vn));
	if (VLevel < START_CONFIRM_HEADING_VEL_THD)
		return;
	
	
	HeadingTemp = atan2(pGnssData->Velocity.Ve, pGnssData->Velocity.Vn);  
	
	if(ABS(HeadingTemp*RAD2DEG)<=360)
	{
		delta_Heading = (atan2(pGnssData->Velocity.Ve, pGnssData->Velocity.Vn) - g_GINavInfo.Euler.Phi)*RAD2DEG;
		if (delta_Heading > 180.0f)
			delta_Heading -= 360.0f;
		if (delta_Heading < -180.0f)
			delta_Heading += 360.0f;

		delta_Heading = ABS(delta_Heading);

		if(delta_Heading<10)
		{
			g_GINavInfo.Euler.Phi = atan2(pGnssData->Velocity.Ve, pGnssData->Velocity.Vn);
			Euler2Quat(&g_GINavInfo.Euler, &g_GINavInfo.Quat_bn);
			Quat2CM(&g_GINavInfo.Quat_bn, &g_GINavInfo.CM_bn);
			
			HeadNum++;

			if (VLevel > END_CONFIRM_HEADING_VEL_THD)
				g_GINavInfo.INSState |= INS_HEADING_GOOD;
		}
		else
		{
		  if(HeadNum>5*6)
		  {
		   g_GINavInfo.INSState |= INS_HEADING_GOOD;
		  }

		}

		if(HeadNum>=5*10)
			HeadNum=5*10;

   }
}
