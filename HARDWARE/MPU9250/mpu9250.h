#ifndef _MPU9250_H
#define _MPU9250_H
#include "sys.h"
	
#define SMPLRT_DIV     0X19		//陀螺仪采样率典型值为0X07 1000/(1+7)=125HZ   
#define CONFIG         0X1A		//低通滤波器 典型值为0X06 5hz
#define GYRO_CONFIG    0X1B		//陀螺仪测量范围 0X18 正负2000度
#define ACCEL_CONFIG   0X1C		//加速度计测量范围0X18 正负16g
#define ACCEL_CONFIG2  0X1D		//加速度计低通滤波器0X06 5hz
#define PWR_MGMT_1     0X6B		//电源管理1 典型值为0X00
#define PWR_MGMT_2     0X6C		//电源管理2 典型值为0X00

#define WHO_AM_I       0X75		//器件ID MPU9250默认ID为0X71
#define USER_CTRL      0X6A		//用户配置为0X10时为使用SPI模式

#define MPU9250_CS     PDout(3)		//MPU9250片选信号
#define	GYRO_ADDRESS   0xD0	  //陀螺地址
#define MAG_ADDRESS    0x18   //磁场地址
#define ACCEL_ADDRESS  0xD0		//加速度计地址

#define ACCEL_XOUT_H   0X3B		//加速度计输出数据
#define ACCEL_XOUT_L   0X3C
#define ACCEL_YOUT_H   0X3D
#define ACCEL_YOUT_L   0X3E
#define ACCEL_ZOUT_H   0X3F
#define ACCEL_ZOUT_L   0X40

#define GYRO_XOUT_H    0X43		//陀螺仪输出数据
#define GYRO_XOUT_L    0X44
#define GYRO_YOUT_H    0X45
#define GYRO_YOUT_L    0X46
#define GYRO_ZOUT_H    0X47
#define GYRO_ZOUT_L    0X48

void MPU9250_Start(void);
void MPU9250_Init(void);
void Get_IMU(uint8_t *IMU_buff);

#endif

