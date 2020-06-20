#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
//#include "malloc.h"
#include "string.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "wifi.h"
#include "car.h"
#include "lcd_show.h"
#include "dis.h"


//任务优先级
#define START_TASK_PRIO		0
//任务堆栈大小	
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		1
//任务堆栈大小	
#define TASK1_STK_SIZE 		256  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define DATAPROCESS_TASK_PRIO 3
//任务堆栈大小	
#define DATAPROCESS_STK_SIZE  256 
//任务句柄
TaskHandle_t DataProcess_Handler;
//任务函数
void DataProcess_task(void *pvParameters);

//二值信号量句柄
//SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(115200);					//初始化串口
	//LED_Init();		  					//初始化LED
	//KEY_Init();							//初始化按键
	//BEEP_Init();						//初始化蜂鸣器
	LCD_Init();							//初始化LCD
	//my_mem_init(SRAMIN);            	//初始化内部内存池

	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
	//wifi_init();
	car_init();
	dis_init();
	
	while(1)
	{
		lcd_show_init();
		
        vTaskDelay(1000);             	//延时500ms，也就是500个时钟节拍	
	}
	
	
//    taskENTER_CRITICAL();           //进入临界区
	
	//创建二值信号量
	//BinarySemaphore=xSemaphoreCreateBinary();	
    //创建TASK1任务
//    xTaskCreate((TaskFunction_t )task1_task,             
//                (const char*    )"task1_task",           
//                (uint16_t       )TASK1_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )TASK1_TASK_PRIO,        
//                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
//    xTaskCreate((TaskFunction_t )DataProcess_task,     
//                (const char*    )"keyprocess_task",   
//                (uint16_t       )DATAPROCESS_STK_SIZE,
//                (void*          )NULL,
//                (UBaseType_t    )DATAPROCESS_TASK_PRIO,
//                (TaskHandle_t*  )&DataProcess_Handler); 
//    vTaskDelete(StartTask_Handler); //删除开始任务
//    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
//void task1_task(void *pvParameters)
//{
//	car_init();
//	wifi_init();
//	
//	lcd_show_init();
//	while(1)
//	{
//        vTaskDelay(1000);             	//延时500ms，也就是500个时钟节拍	
//	}
//}





