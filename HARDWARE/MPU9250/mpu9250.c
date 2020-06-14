#include "myiic.h"
#include "delay.h"
#include "mpu9250.h"

void MPU9250_Init(void)
{
		 u8 res=0;
		
		 res = MPU_Read(GYRO_ADDRESS,WHO_AM_I);
	
		 if(res == 0x71)
		 {
				  MPU_Write(GYRO_ADDRESS,PWR_MGMT_1,0x00);	//解除休眠状态
				  MPU_Write(GYRO_ADDRESS,SMPLRT_DIV,0x07);
				  MPU_Write(GYRO_ADDRESS,CONFIG,0x06);
				  MPU_Write(GYRO_ADDRESS,GYRO_CONFIG,0x18);
			  	MPU_Write(GYRO_ADDRESS,ACCEL_CONFIG,0x01);
	   }
}

void Get_IMU(uint8_t *IMU_buff)
{
		 unsigned char BUF[10];       //接收数据缓存区
		 uint8_t Acc_x,Acc_y,Acc_z;
		 uint8_t Gyr_x,Gyr_y,Gyr_z;

		 BUF[0]=MPU_Read(ACCEL_ADDRESS,ACCEL_XOUT_L); 
		 BUF[1]=MPU_Read(ACCEL_ADDRESS,ACCEL_XOUT_H);
		 Acc_x=	(BUF[1]<<8)|BUF[0];//读取计算X轴数据				   

		 BUF[2]=MPU_Read(ACCEL_ADDRESS,ACCEL_YOUT_L);
		 BUF[3]=MPU_Read(ACCEL_ADDRESS,ACCEL_YOUT_H);
		 Acc_y=	(BUF[3]<<8)|BUF[2];//读取计算Y轴数据
								 
		 BUF[4]=MPU_Read(ACCEL_ADDRESS,ACCEL_ZOUT_L);
		 BUF[5]=MPU_Read(ACCEL_ADDRESS,ACCEL_ZOUT_H);
		 Acc_z=	(BUF[5]<<8)|BUF[4];//读取计算Z轴数据
	
		 BUF[0]=MPU_Read(GYRO_ADDRESS,GYRO_XOUT_L); 
		 BUF[1]=MPU_Read(GYRO_ADDRESS,GYRO_XOUT_H);
		 Gyr_x=	(BUF[1]<<8)|BUF[0];//读取计算X轴数据						   

		 BUF[2]=MPU_Read(GYRO_ADDRESS,GYRO_YOUT_L);
		 BUF[3]=MPU_Read(GYRO_ADDRESS,GYRO_YOUT_H);
		 Gyr_y=	(BUF[3]<<8)|BUF[2];//读取计算Y轴数据
							 
		 BUF[4]=MPU_Read(GYRO_ADDRESS,GYRO_ZOUT_L);
		 BUF[5]=MPU_Read(GYRO_ADDRESS,GYRO_ZOUT_H);
		 Gyr_z=	(BUF[5]<<8)|BUF[4];//读取计算Z轴数据	
	
		 IMU_buff[0] = Gyr_x;
		 IMU_buff[1] = Gyr_y;
		 IMU_buff[2] = Gyr_z;
		 IMU_buff[3] = Acc_x;
		 IMU_buff[4] = Acc_y;
		 IMU_buff[5] = Acc_z;
}

void MPU9250_Start(void)
{
		 MPU9250_Init();
}


