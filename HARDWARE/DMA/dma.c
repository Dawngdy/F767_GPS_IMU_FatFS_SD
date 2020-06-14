#include "dma.h"
#include "usart.h"

DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA���
DMA_HandleTypeDef  UART2TxDMA_Handler;      //DMA���

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
		if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
		{
			__HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ��
			//__HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ��	
			__HAL_LINKDMA(&UART1_Handler,hdmatx,UART1TxDMA_Handler);    //��DMA��USART1��ϵ����(����DMA)
			
			//Tx DMA����
			UART1TxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
			UART1TxDMA_Handler.Init.Channel=chx;                                //ͨ��ѡ��
			UART1TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
			UART1TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
			UART1TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
			UART1TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
			UART1TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
			UART1TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //��������ģʽ
			UART1TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
			UART1TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
			UART1TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
			UART1TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
			UART1TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
			
			HAL_DMA_DeInit(&UART1TxDMA_Handler);   
			HAL_DMA_Init(&UART1TxDMA_Handler);			
		}else 
		{
			__HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
			__HAL_LINKDMA(&UART2_Handler,hdmatx,UART2TxDMA_Handler);    //��DMA��USART1��ϵ����(����DMA)
		
			//Tx DMA����
			UART2TxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
			UART2TxDMA_Handler.Init.Channel=chx;                                //ͨ��ѡ��
			UART2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
			UART2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
			UART2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
			UART2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
			UART2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
			UART2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //��������ģʽ
			UART2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
			UART2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
			UART2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
			UART2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
			UART2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
			
			HAL_DMA_DeInit(&UART2TxDMA_Handler);   
			HAL_DMA_Init(&UART2TxDMA_Handler);
		}

		

} 


 
 
