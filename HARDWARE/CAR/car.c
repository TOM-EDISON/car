#include "car.h"
#include "car_err.h"
#include "string.h"
#include <stdio.h>
#include "pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



/************ 函数声明 **************/
static int car_go_func(u8* data);
static int car_stop_func(u8* data);
static int car_back_func(u8* data);
static int car_speed_func(u32* data);
static int car_left_func(u8* data);
static int car_right_func(u8* data);
void car_task(void *pvParameters);

/******** 创建任务相关宏 ************/
//任务堆栈大小	
#define CAR_STK_SIZE 		64
//任务优先级
#define CAR_TASK_PRIO		2
//任务句柄
TaskHandle_t car_Handler;



static struct CAR_OPTION car_op = 
{
	0,
	0,
	0,
	FAULT,
	STOP,
	ON,
	DIRECT
};


void car_init(void)
{
	TIM3_PWM_Init(1999,719);
//	/*1850 中值,加减100 舵机90°*/
//	TIM_SetCompare1(TIM3,1000);
//	TIM_SetCompare2(TIM3,2000);
//	TIM_SetCompare3(TIM3,1000);
//	TIM_SetCompare4(TIM3,2000);

	car_op.status = WAIT;
	car_op.search = 0;
	
	/*开始RUN*/
	car_op.speed  = 800;
	car_op.last_sp = 800;
	car_go_func(NULL);

	//创建wifi任务
    xTaskCreate((TaskFunction_t )car_task,             
                (const char*    )"car_task",           
                (uint16_t       )CAR_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )CAR_TASK_PRIO,        
                (TaskHandle_t*  )&car_Handler);
	//taskEXIT_CRITICAL();            //退出临界
}

void car_task(void *pvParameters)
{
	while(1)
	{
		if(car_op.search)
		{
			switch (car_op.search)
			{
			case GO:
				car_go_func(NULL);
				car_op.search = 0;
				break;
			case STOP:
				car_stop_func(NULL);
				car_op.search = 0;
				break;
			case BACK:
				car_back_func(NULL);
				car_op.search = 0;
				break;
			case DIRECT:
				car_speed_func((u32*)&car_op.speed);
				car_op.search = 0;
				break;
			case LEFT:
				car_left_func(NULL);
				car_op.search = 0;
				break;
			case RIGHT:
				car_right_func(NULL);
				car_op.search = 0;
				break;
			default:
				car_op.search = 0;
				break;
			}
		}
		vTaskDelay(20);
	}
}

struct CAR_OPTION* get_car_op()
{
	return &car_op;
}


/************* 小车状态执行句柄 *****************/
static int car_go_func(u8* data)
{
	TIM_SetCompare1(TIM3,2000 - car_op.last_sp);
	TIM_SetCompare2(TIM3,2000);
	TIM_SetCompare3(TIM3,2000 - car_op.last_sp);
	TIM_SetCompare4(TIM3,2000);

	car_op.status = RUN;
	
	return 0;
}

static int car_stop_func(u8* data)
{
	TIM_SetCompare1(TIM3,2000);
	TIM_SetCompare2(TIM3,2000);
	TIM_SetCompare3(TIM3,2000);
	TIM_SetCompare4(TIM3,2000);
	
	car_op.status = WAIT;
	car_op.speed  = 0;
	
	return 0;
}

static int car_back_func(u8* data)
{
	if(car_op.speed)
	{
		car_stop_func(NULL);
		vTaskDelay(300);
	}
	
	TIM_SetCompare1(TIM3,2000);
	TIM_SetCompare2(TIM3,1000);
	TIM_SetCompare3(TIM3,2000);
	TIM_SetCompare4(TIM3,1000);
	
	car_op.status = RUN;
	car_op.speed  = 1500;
	
	vTaskDelay(1000);
	car_stop_func(NULL);
	
	return 0;
}

static int car_speed_func(u32* data)
{
	TIM_SetCompare1(TIM3,2000 - *data);
	TIM_SetCompare2(TIM3,2000);
	TIM_SetCompare3(TIM3,2000 - *data);
	TIM_SetCompare4(TIM3,2000);
	
	car_op.speed = *data;
	car_op.last_sp = *data;
	return 0;
}

static int car_left_func(u8* data)
{
	if( car_op.speed )
	{
		car_stop_func(NULL);
		vTaskDelay(300);
	}
	
	TIM_SetCompare1(TIM3,1000);
	TIM_SetCompare2(TIM3,2000);
	TIM_SetCompare3(TIM3,2000);
	TIM_SetCompare4(TIM3,1000);
	
	car_op.speed = 1000;
	vTaskDelay(1000);
	
	//还原速度
	car_go_func(NULL); 
	return 0;
}

static int car_right_func(u8* data)
{
	if( car_op.speed )
	{
		car_stop_func(NULL);
		vTaskDelay(300);
	}
	
	TIM_SetCompare1(TIM3,2000);
	TIM_SetCompare2(TIM3,1000);
	TIM_SetCompare3(TIM3,1000);
	TIM_SetCompare4(TIM3,2000);
	
	car_op.speed = 1000;
	vTaskDelay(1000);
	
	//还原速度
	car_go_func(NULL); 

	return 0;
}


/************* 修改小车状态句柄 *****************/
int car_go_hand(char* buf)
{
	if(car_op.status == FAULT)
		return STA_ERR;
	
	car_op.mode   = GO;
	car_op.speed  = 10;
	car_op.search   = GO;
	
	return 0;
}

int car_stop_hand(char* buf)
{
	if(car_op.status == FAULT)
		return STA_ERR;
	
	car_op.mode   = STOP;
	car_op.speed  = 0;
	car_op.search   = STOP;
	
	return 0;
}

int car_back_hand(char* buf)
{
	if(car_op.status == FAULT)
		return STA_ERR;
	
	car_op.mode = BACK;
	car_op.search = BACK;
	return 0;
}

int car_speed_hand(char* buf)
{
	char *p = NULL;
	
	if(buf == NULL)
		return ARG_ERR;
	
	if(car_op.status == FAULT)
		return STA_ERR;
	
	p = strstr(buf,"=");
	sscanf(p,"%d",(int*)&car_op.speed);
	car_op.search = DIRECT;

	return 0;
}

int car_dis_hand(char* buf)
{
	char *p = NULL;
	u8   data = 0;
	
	if(buf == NULL)
		return ARG_ERR;
	
	if(car_op.status == FAULT)
		return STA_ERR;
	
	p = strstr(buf,"=");
	sscanf(p,"%d",(int*)&data);
	
	if( data == 0)
	{
		car_op.dis = OFF;
	}
	else
	{
		car_op.dis = ON;
	}
	
	return 0;
}


int car_left_hand(char* data)
{
	if(car_op.status == FAULT)
		return STA_ERR;
	
	car_op.turn = LEFT;
	car_op.search = LEFT;
	
	return 0;
}

int car_right_hand(char* data)
{
	if(car_op.status == FAULT)
		return STA_ERR;
	
	car_op.turn = RIGHT;
	car_op.search = RIGHT;
	
	return 0;
}


