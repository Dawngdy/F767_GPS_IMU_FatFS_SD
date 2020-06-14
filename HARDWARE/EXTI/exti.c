#include "exti.h"
#include "delay.h"

void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //����GPIOAʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_13;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;				//����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //�ж���13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,6,3);   //��ռ���ȼ�Ϊ3�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���13  
}


//�жϷ�����

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);  //�����жϴ����ú���
}

//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
u8 count;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {

        case GPIO_PIN_13:
					count++;
						//printf("pps= %d\r\n",count);
            break;
    }
}

