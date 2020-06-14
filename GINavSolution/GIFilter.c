//**************************************************************//
//    GNSS/INS Loose-Coupled Integrated Navigation Algorithm	//
//**************************************************************//
//	Author: 									//
//	This algorithm is used for GNSS/INS Integrated Navigation	//
//	System, its core is a Extended Kalman Filter. For Saving	//
//	RAM, P Matrix of Filter only save half triangular section,	//
//	moreover, the huge matrix calculation is expanded to some 	//
//	small pieces(3*3 matrix) for saving CPU resource(See de-	//
//	tails in Head file).										//
//									2015.4.12					//
//																//
//	Copyright  All Right Reserved.				//
//**************************************************************//

#include "GIFilter.h"
#include "GlobalVars.h"
#include "BasicFunc.h"
#include "Gnss.h"
#include "DataProc.h"
#include <string.h>
#include <math.h>


STATIC GIKF_DATA_T GIKFdata;
FLOAT64 *pBlock[15][3];
CONST U8 line_start[] = { 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120 };

//��ʼ��kalman�˲���
void GIKFInit(void)
{
	MEMSET(GIKFdata.X, 0, SIZEOF(GIKFdata.X));
	MEMSET(GIKFdata.P, 0, SIZEOF(GIKFdata.P));
	MEMSET(GIKFdata.Q, 0, SIZEOF(GIKFdata.Q));
	MEMSET(GIKFdata.Fpp, 0, SIZEOF(GIKFdata.Fpp));
	MEMSET(GIKFdata.Fpv, 0, SIZEOF(GIKFdata.Fpv));
	MEMSET(GIKFdata.Fvp, 0, SIZEOF(GIKFdata.Fvp));
	MEMSET(GIKFdata.Fvv, 0, SIZEOF(GIKFdata.Fvv));
	MEMSET(GIKFdata.Fva, 0, SIZEOF(GIKFdata.Fva));
	MEMSET(GIKFdata.Faa, 0, SIZEOF(GIKFdata.Faa));
	MEMSET(GIKFdata.Fvba, 0, SIZEOF(GIKFdata.Fvba));

	//GetQMatrix();
	GIKFModularizePMatrix(pBlock, GIKFdata.P);


}

//P����ֿ飬15�������Ǿ����Ϊ15��3*3�Ӿ���
void GIKFModularizePMatrix(FLOAT64 *pBlock[][3], FLOAT64 *pmtx)
{
	S32 i, j, k;
	for (i = 0; i < 5; i++)
	{
		for (j = line_start[i]; j < line_start[i + 1]; j++)
		{
			for (k = 0; k < 3; k++)
				pBlock[j][k] = &pmtx[line_start[3 * i + k] + 3 * (j - line_start[i])];
		}
	}
}

//��ʼ����������������
void GetQMatrix(void)		// Init Q Matrix            //gty�� ������ʼ���ٶȡ��Ƕȡ����ٶȺͽ��ٶȵ�������λ�ú͸߶ȵ�����Ϊ�㣿��
{
	float TTScale = 1.0,GTScale=1.0;
	if (g_GINavInfo.Jugde == 0)
	{
		TTScale = 1.0;
	}
	else
	{
		TTScale = 0.2;
	}
	

	 GTScale=12.0;
	

	GIKFdata.Q[0] = GIKFdata.Q[1] = POS_RAND_WALK_LEVEL*POS_RAND_WALK_LEVEL;
	GIKFdata.Q[2] = POS_RAND_WALK_ALT*POS_RAND_WALK_ALT;
	GIKFdata.Q[3] = GIKFdata.Q[4] = GIKFdata.Q[5] = VEL_RAND_WALK*VEL_RAND_WALK;
	GIKFdata.Q[6] = GIKFdata.Q[7] = GIKFdata.Q[8] = ANG_RAND_WALK*ANG_RAND_WALK*TTScale;

	GIKFdata.Q[9] = GIKFdata.Q[10] = GIKFdata.Q[11] = 2*ACC_CONST_BIAS*ACC_CONST_BIAS/3600.0;
	GIKFdata.Q[12] = GIKFdata.Q[13] = GIKFdata.Q[14] = 2*(GYRO_CONST_BIAS/GTScale)*(GYRO_CONST_BIAS/GTScale)/3600.0;
}

//��ʼ��P��
void GIKFInitPMatrix(void)		// Init P Matrix
{

	GIKFdata.P[0] = GIKFdata.P[2] = 25.0;
	GIKFdata.P[5] = 64.0;
	GIKFdata.P[9] = GIKFdata.P[14] = 0.25;
	GIKFdata.P[20] = 0.64;
	GIKFdata.P[27] = GIKFdata.P[35] = GIKFdata.P[44] = 1.0*1.0*DEG2RAD*DEG2RAD;
	GIKFdata.P[54] = GIKFdata.P[65] = GIKFdata.P[77] = 0.15*0.15;//ACC_CONST_BIAS*ACC_CONST_BIAS;//
	GIKFdata.P[90] = GIKFdata.P[104] = GIKFdata.P[119] =0.25*0.25*DEG2RAD*DEG2RAD;

}

//���չߵ����ģ�ͼ���״̬����ĸ��ӿ�
void GIKFCalcPHIMatrix(U32 MsInterval)
{
	FLOAT64 r, temp[3], dt = MsInterval / 1000.;

	// λ��vsλ��
	GIKFdata.Fpp[0] = GIKFdata.Fpp[4] = GIKFdata.Fpp[8] = 1.0;
	GIKFdata.Fpp[1] = g_GINavInfo.Wen[2] * dt;		GIKFdata.Fpp[2] = -g_GINavInfo.Wen[1] * dt;
	GIKFdata.Fpp[3] = -g_GINavInfo.Wen[2] * dt;		GIKFdata.Fpp[5] = g_GINavInfo.Wen[0] * dt;
	GIKFdata.Fpp[6] = g_GINavInfo.Wen[1] * dt;		GIKFdata.Fpp[7] = -g_GINavInfo.Wen[0] * dt;
	// λ��vs�ٶ�
	GIKFdata.Fpv[0] = GIKFdata.Fpv[4] = GIKFdata.Fpv[8] = dt;
	// �ٶ�vsλ��
	r = sqrt(g_GINavInfo.Rm * g_GINavInfo.Rn);
	GIKFdata.Fvp[0] = GIKFdata.Fvp[4] = -g_GINavInfo.Gravity / (r + g_GINavInfo.Position.Alt)*dt;
	GIKFdata.Fvp[8] = -2.0 * GIKFdata.Fvp[0];
	// �ٶ�vs�ٶ�
	temp[0] = (2.0*g_GINavInfo.Wie[0] + g_GINavInfo.Wen[0]) * dt;
	temp[1] = (2.0*g_GINavInfo.Wie[1] + g_GINavInfo.Wen[1]) * dt;
	temp[2] = (2.0*g_GINavInfo.Wie[2] + g_GINavInfo.Wen[2]) * dt;
	GIKFdata.Fvv[0] = GIKFdata.Fvv[4] = GIKFdata.Fvv[8] = 1.0;
	GIKFdata.Fvv[1] = temp[2];		GIKFdata.Fvv[2] = -temp[1];
	GIKFdata.Fvv[3] = -temp[2];		GIKFdata.Fvv[5] = temp[0];
	GIKFdata.Fvv[6] = temp[1];		GIKFdata.Fvv[7] = -temp[0];
	// �ٶ�vs��̬
	GIKFdata.Fva[1] = -g_GINavInfo.SF_n[2];		GIKFdata.Fva[2] = g_GINavInfo.SF_n[1];
	GIKFdata.Fva[3] = g_GINavInfo.SF_n[2];		GIKFdata.Fva[5] = -g_GINavInfo.SF_n[0];
	GIKFdata.Fva[6] = -g_GINavInfo.SF_n[1];		GIKFdata.Fva[7] = g_GINavInfo.SF_n[0];
	// ��̬vs��̬
	temp[0] = (g_GINavInfo.Wie[0] + g_GINavInfo.Wen[0]) * dt;
	temp[1] = (g_GINavInfo.Wie[1] + g_GINavInfo.Wen[1]) * dt;
	temp[2] = (g_GINavInfo.Wie[2] + g_GINavInfo.Wen[2]) * dt;
	GIKFdata.Faa[0] = GIKFdata.Faa[4] = GIKFdata.Faa[8] = 1.0;
	GIKFdata.Faa[1] = temp[2];		GIKFdata.Faa[2] = -temp[1];
	GIKFdata.Faa[3] = -temp[2];		GIKFdata.Faa[5] = temp[0];
	GIKFdata.Faa[6] = temp[1];		GIKFdata.Faa[7] = -temp[0];
	// �ٶ�vs�Ӽ�bias
	GIKFdata.Fvba[0] = g_GINavInfo.CM_bn.C11 * dt;
	GIKFdata.Fvba[1] = g_GINavInfo.CM_bn.C12 * dt;
	GIKFdata.Fvba[2] = g_GINavInfo.CM_bn.C13 * dt;
	GIKFdata.Fvba[3] = g_GINavInfo.CM_bn.C21 * dt;
	GIKFdata.Fvba[4] = g_GINavInfo.CM_bn.C22 * dt;
	GIKFdata.Fvba[5] = g_GINavInfo.CM_bn.C23 * dt;
	GIKFdata.Fvba[6] = g_GINavInfo.CM_bn.C31 * dt;
	GIKFdata.Fvba[7] = g_GINavInfo.CM_bn.C32 * dt;
	GIKFdata.Fvba[8] = g_GINavInfo.CM_bn.C33 * dt;
	// ��̬vs����bias
	GIKFdata.Fabg[0] = -g_GINavInfo.CM_bn.C11 * dt;
	GIKFdata.Fabg[1] = -g_GINavInfo.CM_bn.C12 * dt;
	GIKFdata.Fabg[2] = -g_GINavInfo.CM_bn.C13 * dt;
	GIKFdata.Fabg[3] = -g_GINavInfo.CM_bn.C21 * dt;
	GIKFdata.Fabg[4] = -g_GINavInfo.CM_bn.C22 * dt;
	GIKFdata.Fabg[5] = -g_GINavInfo.CM_bn.C23 * dt;
	GIKFdata.Fabg[6] = -g_GINavInfo.CM_bn.C31 * dt;
	GIKFdata.Fabg[7] = -g_GINavInfo.CM_bn.C32 * dt;
	GIKFdata.Fabg[8] = -g_GINavInfo.CM_bn.C33 * dt;
	//����bias vs ����bias
	GIKFdata.Fbg[0] = exp(-1/3600.0/20);
	GIKFdata.Fbg[4] = exp(-1/3600.0/20);
	GIKFdata.Fbg[8] = exp(-1/3600.0/20);
	//�Ӽ�bias vs �Ӽ�bias
	GIKFdata.Fba[0] = exp(-1/3600.0/20);
	GIKFdata.Fba[4] = exp(-1/3600.0/20);
	GIKFdata.Fba[8] = exp(-1/3600.0/20);
}

void GIKFPredictPMatrix(U32 MsInterval)
{
	S32 i;
	FLOAT64 Pk_1[STATE_DIM*(STATE_DIM + 1) / 2], dt = MsInterval / 1000.;
	FLOAT64 *pBlock_1[15][3];

	MEMCPY(Pk_1, GIKFdata.P, SIZEOF(GIKFdata.P));
	MEMSET(GIKFdata.P, 0, SIZEOF(GIKFdata.P));

	//Modularize Pk_1 matrix
	GIKFModularizePMatrix(pBlock_1, Pk_1);

	//PHI*P*PHI' չ����ʽ������0Ԫ�س˷�
	// PHI*P*PHI'
	//p0 = Fpp*conj(p1)*conj(Fpv) + Fpp*p0*conj(Fpp) + Fpv*p1*conj(Fpp) + Fpv*p2*conj(Fpv)
	AddMxMtxMt(pBlock[0], GIKFdata.Fpp, pBlock_1[1], GIKFdata.Fpv, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[0], GIKFdata.Fpp, pBlock_1[0], GIKFdata.Fpp, 3, 3, 3, 3, 0x03);
	AddMxMxMt(pBlock[0], GIKFdata.Fpv, pBlock_1[1], GIKFdata.Fpp, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[0], GIKFdata.Fpv, pBlock_1[2], GIKFdata.Fpv, 3, 3, 3, 3, 0x03);
	//p1 = Fvp*conj(p1)*conj(Fpv) + Fvp*p0*conj(Fpp) + Fvv*p1*conj(Fpp) + Fvv*p2*conj(Fpv) + Fva*p3*conj(Fpp) + Fva*p4*conj(Fpv) + Fvba*p6*conj(Fpp) + Fvba*p7*conj(Fpv)
	AddMxMtxMt(pBlock[1], GIKFdata.Fvp, pBlock_1[1], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[1], GIKFdata.Fvp, pBlock_1[0], GIKFdata.Fpp, 3, 3, 3, 3, 0x01);
	AddMxMxMt(pBlock[1], GIKFdata.Fvv, pBlock_1[1], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[1], GIKFdata.Fvv, pBlock_1[2], GIKFdata.Fpv, 3, 3, 3, 3, 0x01);
	AddMxMxMt(pBlock[1], GIKFdata.Fva, pBlock_1[3], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[1], GIKFdata.Fva, pBlock_1[4], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[1], GIKFdata.Fvba, pBlock_1[6], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[1], GIKFdata.Fvba, pBlock_1[7], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	//p2 = Fvp*conj(p1)*conj(Fvv) + Fvp*conj(p3)*conj(Fva) + Fvv*conj(p4)*conj(Fva) + Fvp*conj(p6)*conj(Fvba) + Fvv*conj(p7)*conj(Fvba) + Fva*conj(p8)*conj(Fvba) + Fvp*p0*conj(Fvp) +
	//	Fvv*p1*conj(Fvp) + Fvv*p2*conj(Fvv) + Fva*p3*conj(Fvp) + Fva*p4*conj(Fvv) + Fva*p5*conj(Fva) + Fvba*p6*conj(Fvp) + Fvba*p7*conj(Fvv) + Fvba*p8*conj(Fva) + Fvba*p9*conj(Fvba)
	AddMxMtxMt(pBlock[2], GIKFdata.Fvp, pBlock_1[1], GIKFdata.Fvv, 3, 3, 3, 3, 0x02);
	AddMxMtxMt(pBlock[2], GIKFdata.Fvp, pBlock_1[3], GIKFdata.Fva, 3, 3, 3, 3, 0x02);
	AddMxMtxMt(pBlock[2], GIKFdata.Fvv, pBlock_1[4], GIKFdata.Fva, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fvp, pBlock_1[0], GIKFdata.Fvp, 3, 3, 3, 3, 0x03);
	AddMxMxMt(pBlock[2], GIKFdata.Fvv, pBlock_1[1], GIKFdata.Fvp, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fvv, pBlock_1[2], GIKFdata.Fvv, 3, 3, 3, 3, 0x03);
	AddMxMxMt(pBlock[2], GIKFdata.Fva, pBlock_1[3], GIKFdata.Fvp, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fva, pBlock_1[4], GIKFdata.Fvv, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fva, pBlock_1[5], GIKFdata.Fva, 3, 3, 3, 3, 0x03);
	AddMxMtxMt(pBlock[2], GIKFdata.Fvp, pBlock_1[6], GIKFdata.Fvba, 3, 3, 3, 3, 0x02);
	AddMxMtxMt(pBlock[2], GIKFdata.Fvv, pBlock_1[7], GIKFdata.Fvba, 3, 3, 3, 3, 0x02);
	AddMxMtxMt(pBlock[2], GIKFdata.Fva, pBlock_1[8], GIKFdata.Fvba, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fvba, pBlock_1[6], GIKFdata.Fvp, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fvba, pBlock_1[7], GIKFdata.Fvv, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fvba, pBlock_1[8], GIKFdata.Fva, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[2], GIKFdata.Fvba, pBlock_1[9], GIKFdata.Fvba, 3, 3, 3, 3, 0x03);
	//p3 = Fabg*p10*conj(Fpp) + Fabg*p11*conj(Fpv) + Faa*p3*conj(Fpp) + Faa*p4*conj(Fpv)
	AddMxMxMt(pBlock[3], GIKFdata.Fabg, pBlock_1[10], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[3], GIKFdata.Fabg, pBlock_1[11], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[3], GIKFdata.Faa, pBlock_1[3], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[3], GIKFdata.Faa, pBlock_1[4], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	//p4 = Faa*conj(p8)*conj(Fvba) + Fabg*p12*conj(Fva) + Fabg*p13*conj(Fvba) + Fabg*p10*conj(Fvp) + Fabg*p11*conj(Fvv) + Faa*p3*conj(Fvp) + Faa*p4*conj(Fvv) + Faa*p5*conj(Fva)
	AddMxMxMt(pBlock[4], GIKFdata.Faa, pBlock_1[3], GIKFdata.Fvp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[4], GIKFdata.Faa, pBlock_1[4], GIKFdata.Fvv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[4], GIKFdata.Faa, pBlock_1[5], GIKFdata.Fva, 3, 3, 3, 3, 0x01);
	AddMxMtxMt(pBlock[4], GIKFdata.Faa, pBlock_1[8], GIKFdata.Fvba, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[4], GIKFdata.Fabg, pBlock_1[10], GIKFdata.Fvp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[4], GIKFdata.Fabg, pBlock_1[11], GIKFdata.Fvv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[4], GIKFdata.Fabg, pBlock_1[12], GIKFdata.Fva, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[4], GIKFdata.Fabg, pBlock_1[13], GIKFdata.Fvba, 3, 3, 3, 3, 0x00);
	//p5 = Faa*conj(p12)*conj(Fabg) + Fabg*p12*conj(Faa) + Fabg*p14*conj(Fabg) + Faa*p5*conj(Faa)
	AddMxMxMt(pBlock[5], GIKFdata.Faa, pBlock_1[5], GIKFdata.Faa, 3, 3, 3, 3, 0x03);
	AddMxMtxMt(pBlock[5], GIKFdata.Faa, pBlock_1[12], GIKFdata.Fabg, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[5], GIKFdata.Fabg, pBlock_1[12], GIKFdata.Faa, 3, 3, 3, 3, 0x02);
	AddMxMxMt(pBlock[5], GIKFdata.Fabg, pBlock_1[14], GIKFdata.Fabg, 3, 3, 3, 3, 0x03);
	//p6 = Fba*p6*conj(Fpp) + Fba*p7*conj(Fpv)
	AddMxMxMt(pBlock[6], GIKFdata.Fba, pBlock_1[6], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[6], GIKFdata.Fba,pBlock_1[7], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	//p7 = Fba*p6*conj(Fvp) + Fba*p7*conj(Fvv) + Fba*p8*conj(Fva) + Fba*p9*conj(Fvba)
	AddMxMxMt(pBlock[7], GIKFdata.Fba, pBlock_1[6], GIKFdata.Fvp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[7], GIKFdata.Fba, pBlock_1[7], GIKFdata.Fvv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[7], GIKFdata.Fba, pBlock_1[8], GIKFdata.Fva, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[7], GIKFdata.Fba, pBlock_1[9], GIKFdata.Fvba, 3, 3, 3, 3, 0x01);
	//p8 = Fba*conj(p13)*conj(Fabg) + Fba*p8*conj(Faa)
	AddMxMtxMt(pBlock[8], GIKFdata.Fba, pBlock_1[13], GIKFdata.Fabg, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[8], GIKFdata.Fba, pBlock_1[8], GIKFdata.Faa, 3, 3, 3, 3, 0x00);
	//p9 = Fba*p9*conj(Fba)
	//Addequal(pBlock[9], pBlock_1[9], 3, 3, 0x03);
	AddMxMxMt(pBlock[9], GIKFdata.Fba, pBlock_1[9], GIKFdata.Fba, 3, 3, 3, 3, 0x03);

	//p10 = Fbg*p10*conj(Fpp) + Fbg*p11*conj(Fpv)
	AddMxMxMt(pBlock[10], GIKFdata.Fbg, pBlock_1[10], GIKFdata.Fpp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[10], GIKFdata.Fbg, pBlock_1[11], GIKFdata.Fpv, 3, 3, 3, 3, 0x00);
	//p11 = Fbg*p10*conj(Fvp) + Fbg*p11*conj(Fvv) + Fbg*p12*conj(Fva) + Fbg*p13*conj(Fvba)
	AddMxMxMt(pBlock[11], GIKFdata.Fbg, pBlock_1[10], GIKFdata.Fvp, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[11], GIKFdata.Fbg, pBlock_1[11], GIKFdata.Fvv, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[11], GIKFdata.Fbg, pBlock_1[12], GIKFdata.Fva, 3, 3, 3, 3, 0x00);
	AddMxMxMt(pBlock[11], GIKFdata.Fbg, pBlock_1[13], GIKFdata.Fvba, 3, 3, 3, 3, 0x00);
	//p12 = Fbg*p12*conj(Faa) + Fbg*p14*conj(Fabg)
	AddMxMxMt(pBlock[12], GIKFdata.Fbg, pBlock_1[12], GIKFdata.Faa, 3, 3, 3, 3, 0x00); 
	AddMxMxMt(pBlock[12], GIKFdata.Fbg, pBlock_1[14], GIKFdata.Fabg, 3, 3, 3, 3, 0x01);
	//p13 = Fbg*p13*conj(Fba)
	//Addequal(pBlock[13], pBlock_1[13], 3, 3, 0x00);
	AddMxMxMt(pBlock[13], GIKFdata.Fbg, pBlock_1[13], GIKFdata.Fba, 3, 3, 3, 3, 0x00);

	//p14 = Fbg*p14*conj(Fbg)
	//Addequal(pBlock[14], pBlock_1[14], 3, 3, 0x03);
	AddMxMxMt(pBlock[14], GIKFdata.Fbg, pBlock_1[14], GIKFdata.Fbg, 3, 3, 3, 3, 0x03);

	// Add Q
	GetQMatrix();
	for (i = 0; i < 15; i++)
		GIKFdata.P[line_start[i + 1] - 1] += GIKFdata.Q[i] * dt;


}

//Kalman��������
//Mask - �������룬�������λ��ĳ״̬���ĸ���
void GIKFBatchSolution(U32 Mask, U32 MsrCount, FLOAT64* Msr, FLOAT64* HMatrix, FLOAT64* Sigma)
{
	U32 i, j, k;
	FLOAT64 PHt[STATE_DIM*MAX_MSR_DIM] = { 0.0 }, KMatrix[STATE_DIM*MAX_MSR_DIM] = { 0.0 };
	FLOAT64 temp[STATE_DIM*STATE_DIM];
	FLOAT64 *pHPHt;

	// PHt
	for (i = 0; i < STATE_DIM; i++)
	{
		for (j = 0; j < MsrCount; j++)
		{
			PHt[i*MsrCount + j] = 0.0;
			for (k = 0; k < STATE_DIM; k++)
			{
				if (k <= i)
					PHt[i*MsrCount + j] += GIKFdata.P[line_start[i] + k] * HMatrix[j*STATE_DIM + k];
				else
					PHt[i*MsrCount + j] += GIKFdata.P[line_start[k] + i] * HMatrix[j*STATE_DIM + k];
			}
		}
	}
	// HPHt
	pHPHt = &temp[0];
	for (i = 0; i < MsrCount; i++)
	{
		for (j = line_start[i]; j < line_start[i + 1]; j++)
		{
			pHPHt[j] = 0.0;
			for (k = 0; k < STATE_DIM; k++)
				pHPHt[j] += HMatrix[i*STATE_DIM + k] * PHt[k*MsrCount + j - line_start[i]];
		}
	}
	// HPHt+R
	for (i = 0; i < MsrCount; i++)
		pHPHt[line_start[i + 1] - 1] += Sigma[i];
	// inv(H*P*Ht+R)
	TriangleMatInv(pHPHt, MsrCount);
	// K = P*Ht * inv(H*P*Ht+R)
	for (i = 0; i < STATE_DIM; i++)
	{
		for (j = 0; j < MsrCount; j++)
		{
			KMatrix[i*MsrCount + j] = 0.0;
			for (k = 0; k < MsrCount; k++)
			{
				if ((Mask & (0x01 << i)) > 0)
				{
					if (j <= k)
						KMatrix[i*MsrCount + j] += PHt[i*MsrCount + k] * pHPHt[line_start[k] + j];
					else
						KMatrix[i*MsrCount + j] += PHt[i*MsrCount + k] * pHPHt[line_start[j] + k];
				}
			}
		}
	}
	// state update: dx = K*Z;
	memset(GIKFdata.X, 0, sizeof(GIKFdata.X));
	for (i = 0; i < STATE_DIM; i++)
	{
		for (j = 0; j < MsrCount; j++)
		{
			GIKFdata.X[i] += KMatrix[i*MsrCount + j] * Msr[j];
		}
	}
	// P matrix update: P = (I-K*H)*P*(I-K*H)' + K*R*K'
	// temp = (I-K*H)*P = P-K*H*P = P-K*(PHt)'
	for (i = 0; i < STATE_DIM; i++)
	{
		for (j = 0; j < STATE_DIM; j++)
		{
			if (j <= i)
				temp[i*STATE_DIM + j] = GIKFdata.P[line_start[i] + j];
			else
				temp[i*STATE_DIM + j] = GIKFdata.P[line_start[j] + i];
			for (k = 0; k < MsrCount; k++)
				temp[i*STATE_DIM + j] -= KMatrix[i*MsrCount + k] * PHt[j*MsrCount + k];
		}
	}
	// recalculate PHt
	for (i = 0; i < STATE_DIM; i++)
	{
		for (j = 0; j < MsrCount; j++)
		{
			PHt[i*MsrCount + j] = 0.0;
			for (k = 0; k < STATE_DIM; k++)
				PHt[i*MsrCount + j] += temp[i*STATE_DIM + k] * HMatrix[j*STATE_DIM + k];
		}
	}
	// P = P*(I-K*H)' = P-P*H'*K' = P-PHt*K'
	for (i = 0; i < STATE_DIM; i++)
	{
		for (j = line_start[i]; j < line_start[i + 1]; j++)
		{
			GIKFdata.P[j] = temp[i*STATE_DIM + j - line_start[i]];
			for (k = 0; k < MsrCount; k++)
				GIKFdata.P[j] -= PHt[i*MsrCount + k] * KMatrix[(j - line_start[i])*MsrCount + k];
		}
	}
	// P = P + K*R*K'
	for (i = 0; i<STATE_DIM; i++)
	{
		for (j = line_start[i]; j < line_start[i + 1]; j++)
		{
			for (k = 0; k < MsrCount; k++)
				GIKFdata.P[j] += KMatrix[i*MsrCount + k] * KMatrix[(j - line_start[i])*MsrCount + k] * Sigma[k];
		}
	}
}





//GNSS/INS��ϵ�������
BOOL GIKFUpdateByGNSS(PGNSS_DATA_T pGnssData, PIMU_DATA_T pImuData)
{
	int MsrCount = 0;
	U32 UpdateMask;

	BOOL retGnssEvaluation;
	FLOAT64 deltaPosVel[6], Sigma[6], HMat[6 * STATE_DIM] = { 0.0 }, SigmaSum[2] = { 0 };

	if (!pGnssData)
		return TRUE;


	if (!pGnssData || pGnssData->NavType == 0 || (!IS_POS_VALID(pGnssData->NavFlag) && !IS_LEVEL_VEL_VALID(pGnssData->NavFlag)))  //���һ���жϵ������ǣ����ͬʱû��λ�ú��ٶȷ����򷵻ء���
	{
		g_GINavInfo.Jugde = 0;
		return TRUE;
	}





	retGnssEvaluation = GnssEvaluation(pGnssData, pImuData);
	



	//-------------------------------------

	g_GINavInfo.GNSSHaltCount = 0;            //gty  ����?...

	if (!retGnssEvaluation)                    //Gps�ź�̫��,ֱ���˳�...
	{
		g_GINavInfo.Jugde = 0;

		return TRUE;
	}
	else
	{
		g_GINavInfo.Jugde = 1;
	}



	//���۲�����H��R��
	if (IS_POS_VALID(pGnssData->NavFlag))  //gty  ��γ�߶���Ч
	{//dPosition
		deltaPosVel[MsrCount] = (g_GINavInfo.Position.Lat - pGnssData->Position.Lat) * (g_GINavInfo.Rm + g_GINavInfo.Position.Alt);
		HMat[MsrCount * STATE_DIM + 0] = 1.0;
		Sigma[MsrCount++] = pGnssData->Sigma[0];
		SigmaSum[0] += pGnssData->Sigma[0];

		deltaPosVel[MsrCount] = (g_GINavInfo.Position.Lon - pGnssData->Position.Lon) * (g_GINavInfo.Rn + g_GINavInfo.Position.Alt) * g_GINavInfo.CM_ne.C31;
		HMat[MsrCount * STATE_DIM + 1] = 1.0;
		Sigma[MsrCount++] = pGnssData->Sigma[1];
		SigmaSum[0] += pGnssData->Sigma[1];

		deltaPosVel[MsrCount] = -(g_GINavInfo.Position.Alt - pGnssData->Position.Alt);
		HMat[MsrCount * STATE_DIM + 2] = 1.0;
		Sigma[MsrCount++] = pGnssData->Sigma[2];


	}
	if (IS_LEVEL_VEL_VALID(pGnssData->NavFlag))//gty �����ٶ���Ч
	{//dVn dVe
		deltaPosVel[MsrCount] = g_GINavInfo.Velocity.Vn - pGnssData->Velocity.Vn;
		HMat[MsrCount * STATE_DIM + 3] = 1.0;
		Sigma[MsrCount++] = pGnssData->Sigma[3];
		SigmaSum[1] += pGnssData->Sigma[3];


		deltaPosVel[MsrCount] = g_GINavInfo.Velocity.Ve - pGnssData->Velocity.Ve;
		HMat[MsrCount * STATE_DIM + 4] = 1.0;
		Sigma[MsrCount++] = pGnssData->Sigma[4];
		SigmaSum[1] += pGnssData->Sigma[4];

	}
	if (IS_UP_VEL_VALID(pGnssData->NavFlag))//gty  �����ٶ���Ч��Ŀǰ����汾û������۲���������
	{//dVd
		deltaPosVel[MsrCount] = -(g_GINavInfo.Velocity.Vu - pGnssData->Velocity.Vu);
		HMat[MsrCount * STATE_DIM + 5] = 1.0;
		Sigma[MsrCount++] = pGnssData->Sigma[5];
		SigmaSum[1] += pGnssData->Sigma[5];
	}

	if (MsrCount == 0)
		return TRUE;




	  UpdateMask = 0x00007fff;//��������״̬ Ĭ��


	GIKFBatchSolution(UpdateMask, MsrCount, deltaPosVel, HMat, Sigma);


	if (!GIKFCheckPMatrix())
		return FALSE;

	GIKFINSErrorFix(1);     //gty ��������....


	if (!GIKFCheckGIResult())
		return FALSE;


	g_GINavInfo.KFCount++;



	if (SigmaSum[0] < 16.0 && ((SigmaSum[1] * 100) < 36))
		g_GINavInfo.INSAloneMsCount -= g_GINavInfo.INSAloneMsCount * 2 / 3;
	else if (SigmaSum[0] < 36.0 && SigmaSum[1] < 1.0)
		g_GINavInfo.INSAloneMsCount -= g_GINavInfo.INSAloneMsCount / 2;
	else if (SigmaSum[0] < 64.0 && SigmaSum[1] < 2.25)
		g_GINavInfo.INSAloneMsCount -= g_GINavInfo.INSAloneMsCount / 4;
	else if (SigmaSum[0] < 100.0 && SigmaSum[1] < 4.0)
		g_GINavInfo.INSAloneMsCount -= g_GINavInfo.INSAloneMsCount / 6;
	else if (SigmaSum[0] < 200.0 && SigmaSum[1] < 8.0)
		g_GINavInfo.INSAloneMsCount -= g_GINavInfo.INSAloneMsCount / 8;
	



	return TRUE;
}


BOOL GIKFUpdateByOli(PGNSS_DATA_T pBGnssData, PIMU_DATA_T pImuData)
{

	return FALSE;

}



//Kalman����У��
void GIKFINSErrorFix(U8 RunKind)
{
	FLOAT64 dLat, dLon, DetaSpeed;
	FLOAT64 dTheta[3], temp[3], Phi[3];
	QUAT_T dQuat, tempQ;
	U8  RunFlag, i;
	static  U8 speedxzNum = 0, speedxfNum = 0,  speedzfNum = 0;

	DetaSpeed = sqrt(GIKFdata.X[3] * GIKFdata.X[3] + GIKFdata.X[4] * GIKFdata.X[4]);

	if ((g_GINavInfo.GstStatus == 4) || (RunKind == 2) || (RunKind == 3))
	{
		RunFlag = 1;
	}
	else
	{
		if (DetaSpeed<5.0)  //20160408
		{
			RunFlag = 1;
		}
		else
		{
			RunFlag = 0;
		}
	}


	if (RunFlag)
	{

		//position feedback
		dLat = GIKFdata.X[0] / (g_GINavInfo.Rm + g_GINavInfo.Position.Alt);
		dLon = GIKFdata.X[1] / ((g_GINavInfo.Rn + g_GINavInfo.Position.Alt)*g_GINavInfo.CM_ne.C31);
		dTheta[0] = -dLon * g_GINavInfo.CM_ne.C31;
		dTheta[1] = dLat;
		dTheta[2] = -dLon * g_GINavInfo.CM_ne.C33;
		RotVec2Quat(dTheta, &dQuat);
		MEMCPY(&tempQ, &g_GINavInfo.Quat_ne, SIZEOF(QUAT_T));
		QuatMulti(&tempQ, &dQuat, &g_GINavInfo.Quat_ne);
		NormQuat(&g_GINavInfo.Quat_ne);
		Quat2CM(&g_GINavInfo.Quat_ne, &g_GINavInfo.CM_ne);
		/*g_GINavInfo.Position.Lat = -asin(g_GINavInfo.CM_ne.C33);
		g_GINavInfo.Position.Lon = atan2(-g_GINavInfo.CM_ne.C23, -g_GINavInfo.CM_ne.C13);*/
		g_GINavInfo.Position.Lat -= dLat;
		g_GINavInfo.Position.Lon -= dLon;
		g_GINavInfo.Position.Alt += GIKFdata.X[2];


		//velocity feedback
		temp[0] = g_GINavInfo.Velocity.Vn - GIKFdata.X[3];
		temp[1] = g_GINavInfo.Velocity.Ve - GIKFdata.X[4];
		temp[2] = -g_GINavInfo.Velocity.Vu - GIKFdata.X[5];
		g_GINavInfo.Velocity.Vn = temp[0] + dTheta[2] * temp[1] - dTheta[1] * temp[2];
		g_GINavInfo.Velocity.Ve = temp[1] + dTheta[0] * temp[2] - dTheta[2] * temp[0];
		g_GINavInfo.Velocity.Vu = -(temp[2] + dTheta[1] * temp[0] - dTheta[0] * temp[1]);

	

	
		//attitude correction

		Phi[0] = GIKFdata.X[6] + dTheta[0];
		Phi[1] = GIKFdata.X[7] + dTheta[1];
		Phi[2] = GIKFdata.X[8] + dTheta[2];
		RotVec2Quat(Phi, &dQuat);
		MEMCPY(&tempQ, &g_GINavInfo.Quat_bn, SIZEOF(QUAT_T));
		QuatMulti(&dQuat, &tempQ, &g_GINavInfo.Quat_bn);
		NormQuat(&g_GINavInfo.Quat_bn);
		Quat2CM(&g_GINavInfo.Quat_bn, &g_GINavInfo.CM_bn);
		CM2Euler(&g_GINavInfo.CM_bn, &g_GINavInfo.Euler);



		g_GINavInfo.ImuCfg.AccBias[0] += GIKFdata.X[9];
		g_GINavInfo.ImuCfg.AccBias[1] += GIKFdata.X[10];
		g_GINavInfo.ImuCfg.AccBias[2] += GIKFdata.X[11];


		g_GINavInfo.ImuCfg.GyroBias[0] += GIKFdata.X[12];
		g_GINavInfo.ImuCfg.GyroBias[1] += GIKFdata.X[13];
		g_GINavInfo.ImuCfg.GyroBias[2] += GIKFdata.X[14];

	}
}

//P��������
BOOL GIKFCheckPMatrix(void)
{
	int i;

	for (i = 0; i < STATE_DIM; i++)
	{
		if (GIKFdata.P[line_start[i + 1] - 1] < 0)
			return FALSE;
	}

	for (i = 0; i<5; i++)
	{
		if (pBlock[line_start[i + 1] - 1][0][0] * pBlock[line_start[i + 1] - 1][1][1] * pBlock[line_start[i + 1] - 1][2][2] +
			pBlock[line_start[i + 1] - 1][1][0] * pBlock[line_start[i + 1] - 1][2][1] * pBlock[line_start[i + 1] - 1][2][0] +
			pBlock[line_start[i + 1] - 1][2][0] * pBlock[line_start[i + 1] - 1][1][0] * pBlock[line_start[i + 1] - 1][2][1] -
			pBlock[line_start[i + 1] - 1][2][0] * pBlock[line_start[i + 1] - 1][1][1] * pBlock[line_start[i + 1] - 1][2][0] -
			pBlock[line_start[i + 1] - 1][2][1] * pBlock[line_start[i + 1] - 1][2][1] * pBlock[line_start[i + 1] - 1][0][0] -
			pBlock[line_start[i + 1] - 1][2][2] * pBlock[line_start[i + 1] - 1][1][0] * pBlock[line_start[i + 1] - 1][1][0] < 0.0)
			return FALSE;
	}
	return TRUE;
}

//��������ȷ�Լ��
BOOL GIKFCheckGIResult()
{
	FLOAT32 VLevel = sqrt(SQR(g_GINavInfo.Velocity.Ve) + SQR(g_GINavInfo.Velocity.Vn));

	if (g_GINavInfo.Position.Alt > 10000.0 || g_GINavInfo.Position.Alt < -1000.0)
		return FALSE;

	if (ABS(g_GINavInfo.Velocity.Ve) > 1000.0 || ABS(g_GINavInfo.Velocity.Vn) > 1000.0 || ABS(g_GINavInfo.Velocity.Vu) > 1000.0)
		return FALSE;

	if (ABS(g_GINavInfo.dVelocity.Ve) > 100.0 || ABS(g_GINavInfo.dVelocity.Vn) > 100.0 || ABS(g_GINavInfo.dVelocity.Vu) > 100.0)
		return FALSE;



	
		 if (ABS(g_GINavInfo.Euler.Theta) > 80 * DEG2RAD)
		 return FALSE;

	    if (ABS(g_GINavInfo.Euler.Gamma) > 80 * DEG2RAD)
		  return FALSE;	
		
		
	

	return TRUE;
}


