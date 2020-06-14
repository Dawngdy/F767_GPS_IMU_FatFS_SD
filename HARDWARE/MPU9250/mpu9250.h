#ifndef _MPU9250_H
#define _MPU9250_H
#include "sys.h"
	
#define SMPLRT_DIV     0X19		//�����ǲ����ʵ���ֵΪ0X07 1000/(1+7)=125HZ   
#define CONFIG         0X1A		//��ͨ�˲��� ����ֵΪ0X06 5hz
#define GYRO_CONFIG    0X1B		//�����ǲ�����Χ 0X18 ����2000��
#define ACCEL_CONFIG   0X1C		//���ٶȼƲ�����Χ0X18 ����16g
#define ACCEL_CONFIG2  0X1D		//���ٶȼƵ�ͨ�˲���0X06 5hz
#define PWR_MGMT_1     0X6B		//��Դ����1 ����ֵΪ0X00
#define PWR_MGMT_2     0X6C		//��Դ����2 ����ֵΪ0X00

#define WHO_AM_I       0X75		//����ID MPU9250Ĭ��IDΪ0X71
#define USER_CTRL      0X6A		//�û�����Ϊ0X10ʱΪʹ��SPIģʽ

#define MPU9250_CS     PDout(3)		//MPU9250Ƭѡ�ź�
#define	GYRO_ADDRESS   0xD0	  //���ݵ�ַ
#define MAG_ADDRESS    0x18   //�ų���ַ
#define ACCEL_ADDRESS  0xD0		//���ٶȼƵ�ַ

#define ACCEL_XOUT_H   0X3B		//���ٶȼ��������
#define ACCEL_XOUT_L   0X3C
#define ACCEL_YOUT_H   0X3D
#define ACCEL_YOUT_L   0X3E
#define ACCEL_ZOUT_H   0X3F
#define ACCEL_ZOUT_L   0X40

#define GYRO_XOUT_H    0X43		//�������������
#define GYRO_XOUT_L    0X44
#define GYRO_YOUT_H    0X45
#define GYRO_YOUT_L    0X46
#define GYRO_ZOUT_H    0X47
#define GYRO_ZOUT_L    0X48

void MPU9250_Start(void);
void MPU9250_Init(void);
void Get_IMU(uint8_t *IMU_buff);

#endif

