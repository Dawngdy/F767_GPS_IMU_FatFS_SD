#include "myiic.h"
#include "delay.h"

//IIC初始化
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOH_CLK_ENABLE();   //使能GPIOH时钟
    
    //PH4,5初始化设置
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    IIC_SDA(1);
    IIC_SCL(1);  
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA(1);	  	  
	IIC_SCL(1);
	delay_us(4);
 	IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL(0);
	IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL(1); 
	delay_us(4);			
	IIC_SDA(1);//发送I2C总线结束信号				   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA(1);delay_us(1);	   
	IIC_SCL(1);delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL(1);
		delay_us(2); 
		IIC_SCL(0);	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL(0); 
        delay_us(2);
		IIC_SCL(1);
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

unsigned char I2C_RadeByte(void)  //数据从高位到低位//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    IIC_SDA(1); 				
    while(i--)
    {
      ReceiveByte<<=1;      
      IIC_SCL(0); 
      delay_us(1); 
	    IIC_SCL(1);
      delay_us(1); 	
      if(READ_SDA)
      {
        ReceiveByte|=0x01;
      }
    }
    IIC_SCL(0);
    return ReceiveByte;
} 

u8 MPU_Write(u8 MPU_Adr,u8 address,u8 val)
{
	IIC_Start();
	IIC_Send_Byte(MPU_Adr);
	IIC_Ack();
	IIC_Send_Byte(address);
	IIC_Ack();
	IIC_Send_Byte(val);
	delay_ms(10);
	IIC_Ack();
	IIC_Stop();
	delay_ms(10);
	
	return 1;
}

u8 MPU_Read(u8 MPU_Adr,u8 address)
{
	u8 temp = 0;
	
	IIC_Start();
	IIC_Send_Byte(MPU_Adr);
	IIC_Ack();
	IIC_Send_Byte(address);
	
	IIC_Ack();
	IIC_Start();
	IIC_Send_Byte(MPU_Adr|0X01);
	
	IIC_Ack();
	temp = I2C_RadeByte();
	IIC_NAck();
	IIC_Stop();
	
	return temp;
	
}

unsigned char _i2c_write(unsigned char device_addr, unsigned char sub_addr, unsigned char *buff, int Byte)
{
	
	IIC_Start();  
	IIC_Send_Byte(device_addr);	                  //发送写命令
	IIC_Wait_Ack();    
  IIC_Send_Byte(sub_addr);               //发送低地址
	IIC_Wait_Ack(); 	 
	
	while(Byte--)
	{
		IIC_Send_Byte(*buff);             //发送字节							   
	  IIC_Wait_Ack();  
		
		buff++;
	}
	
	IIC_Stop();                             //产生一个停止条件 
	delay_ms(10);	 
	
	return 0;
}


unsigned char _i2c_read(unsigned char device_addr, unsigned char sub_addr, unsigned char *buff, int Byte)
{	
	
  IIC_Start();  
	IIC_Send_Byte(device_addr);	               //发送写命令
	IIC_Wait_Ack();  
  IIC_Send_Byte(sub_addr);             //发送低地址
	IIC_Wait_Ack();  
	IIC_Start(); 
	IIC_Send_Byte(device_addr+1);             //发送低地址
	IIC_Wait_Ack();  
	
	while(Byte)
	{
		*buff++=IIC_Read_Byte(0);	
		Byte--;
	}
	
	IIC_Stop();
	
	return 0;
}


