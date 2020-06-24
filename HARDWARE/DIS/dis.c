#include "dis.h"
#include "pwm.h"
#include "car.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "delay.h"


/******** ����������غ� ************/
//�����ջ��С	
#define DIS_STK_SIZE 		64
//�������ȼ�
#define DIS_TASK_PRIO		3
//������
TaskHandle_t dis_Handler;

void dis_task(void *pvParameters);
static u16 get_dis(void);
static chose_turn(void);
void disio_init(void);


#define MINDIS     30
#define SET_TRIG   GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define RESET_TRIG GPIO_ResetBits(GPIOC,GPIO_Pin_10)
#define READ_ECHO  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)


void dis_init(void)
{
	
	disio_init();
	TIM4_PWM_Init(1999,719);
	TIM_SetCompare3(TIM4,1850);
	//��ʼ��������
	disio_init();
	
    xTaskCreate((TaskFunction_t )dis_task,             
                (const char*    )"dis_task",           
                (uint16_t       )DIS_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )DIS_TASK_PRIO,        
                (TaskHandle_t*  )&dis_Handler);
	
}


void dis_task(void *pvParameters)
{
	struct CAR_OPTION *car = get_car_op();

	while(1)
	{
		if( car->dis != OFF )
		{
			car->dis = get_dis();
			
			if(car->dis < MINDIS)
			{
				car_stop_hand(NULL);
				vTaskDelay(100);
				
				if(chose_turn() == LEFT)
					car_left_hand(NULL);
				else
					car_right_hand(NULL);
			}
		}
		vTaskDelay(100);
	}
}


static u16 get_dis(void)
{
	u32 distance;
	
	SET_TRIG;
	delay_us(20);
	RESET_TRIG;
	
	TIM_SetCounter(TIM5, 0);
	
	while(READ_ECHO == 0);
	TIM_Cmd(TIM5, ENABLE);
	
	while(READ_ECHO == 1);
	TIM_Cmd(TIM5, DISABLE);  
	distance = TIM_GetCounter(TIM5);
	TIM_SetCounter(TIM5, 0);
		
	
	return (u16)distance/58.8;
}

static chose_turn(void)
{
	u16 left_dis  = 0;
	u16 right_dis = 0;
	
	TIM_SetCompare3(TIM4,1800);
	vTaskDelay(500);
	left_dis = get_dis();
	
	TIM_SetCompare3(TIM4,1900);
	vTaskDelay(500);
	right_dis = get_dis();
	
	TIM_SetCompare3(TIM4,1850);
	
	return left_dis > right_dis ? LEFT : RIGHT;
}


void disio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	/******* ��ʼ������ *********/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;            //echo���� �����ź�
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //trig����  ���͵�ƽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	
//	/********* ʹ�������ж� ***********/
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);  // �ж����Լ��жϳ�ʼ������
//	
//	EXTI_ClearITPendingBit(EXTI_Line11);
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	
//	
//	/************ �ж����� *************/
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//  	NVIC_Init(&NVIC_InitStructure); 
	
	/************ ��ʼ����ʱ�� *************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* TIME5 base configuration */
	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;                   //  ����65536���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;                  // ����Ԥ��Ƶ,F=72MHz/72=1MHz,T=1us
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;                 // ����ʱ�ӷ�Ƶϵ��,����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);              // ��ʼ��TIME5

	TIM_Cmd(TIM5, DISABLE); 

	return;
}


//static void ECHO_EXTI_IRQHandler(void)
//{
//	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
//		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != 0) {  // �ȴ��ߵ�ƽ�ز�
//			TIM_Cmd(TIM5, ENABLE); 	
//		} else {
//	 		TIM_Cmd(TIM5, DISABLE);  
//			distance = TIM_GetCounter(TIM5);
//			TIM_SetCounter(TIM5, 0); 
//		}
//	}
//    EXTI_ClearITPendingBit(EXTI_Line11);
//}

//void EXTI11_IRQHandler(void)
//{
//    ECHO_EXTI_IRQHandler();
//}


