#include "Gnss.h"
#include "Const.h"
#include "Config.h"
#include "DataProc.h"
#include "BasicFunc.h"
#include "GlobalVars.h"
#include "SendGPS.h"
#include <string.h>
#include <math.h>





BOOL GnssEvaluation(PGNSS_DATA_T pGnssData,PIMU_DATA_T pImuData)
{

	static S32 i = 0;


	FLOAT32 scale_std = 0.1;
	
	//-----------------------------------------------------------------------	
	if (pGnssData->Dops[0] < 0.5f)
		pGnssData->Dops[0] = 99.0f;
	if (pGnssData->Dops[1] < 0.5f)
		pGnssData->Dops[1] = 99.0f;
	if (pGnssData->Dops[2] < 0.5f)
		pGnssData->Dops[2] = 99.0f;




	pGnssData->Sigma[0] = pGnssData->Sigma[1] = 0.0125*scale_std;//6.25;     //水平速度噪声初值
	pGnssData->Sigma[2] = 1*scale_std ;//9.0;                            //高度的噪声初值
	pGnssData->Sigma[3] = pGnssData->Sigma[4] = 0.01*scale_std;//0.05;     //水平速度噪声初值
	pGnssData->Sigma[5] = 0.1*scale_std;//0.10;                           //垂直速度噪声初值



	

	return TRUE;
}
