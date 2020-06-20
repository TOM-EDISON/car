#include "wifi.h"
#include "usart3.h"
#include "usart.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "delay.h"
#include "car_err.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "car.h"
#include <string.h>


/***************** WiFi模块相关宏 *************************/
#define RESET         "AT+RST"
#define AP            "AT+CWMODE=2"
#define IP            "AT+CIPAP=\"192.168.0.1\""
#define BUILDAP       "AT+CWSAP=\"DYQ\",\"dyq123456\",1,2"
#define MULT_LINK     "AT+CIPMUX=1"
#define START_SERVER  "AT+CIPSERVER=1,8888"
#define WAIT_TIME     "AT+CIPSTO=5000"
#define ACK           "OK"
#define SEND_ACK	  "SEND OK"
#define RECVE_FLAG    "+IPD,"
#define WAIT          50


/***************** 创建任务相关宏 *************************/
//任务堆栈大小	
#define WIFI_STK_SIZE 		128
//任务优先级
#define WIFI_TASK_PRIO		3
//任务句柄
TaskHandle_t wifi_Handler;
//任务函数
void wifi_task(void *pvParameters);

/*函数声明*/
bool wifi_cmd(char *cmd,char *ack,u32 waittime);
int build_wifi(void);
u8 wifi_respond(char *buf,u8 err,u8 *id);


/*小车指令*/
const struct CAR_AT car_hand[] = 
{
	{"car+go"       ,car_go_hand},
	{"car+stop"     ,car_stop_hand},
	{"car+back"     ,car_back_hand},
	{"car+speed"    ,car_speed_hand},
	{"car+dis"      ,car_dis_hand},
	{"car+left"     ,car_left_hand},
	{"car+right"   ,car_right_hand},
	
};
static int car_at_len = sizeof(car_hand)/sizeof(car_hand[0]);

void wifi_init(void)
{
	usart3_init(921600);
	if(build_wifi() != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		return ;
	}
	
    //taskENTER_CRITICAL();           //进入临界区
		
    //创建wifi任务
    xTaskCreate((TaskFunction_t )wifi_task,             
                (const char*    )"wifi_task",           
                (uint16_t       )WIFI_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )WIFI_TASK_PRIO,        
                (TaskHandle_t*  )&wifi_Handler);
	//taskEXIT_CRITICAL();            //退出临界区
	
}

void wifi_task(void *pvParameters)
{
	char *p   = NULL;
	u8   i    = 0;
	u8   ret  = 0;
	u8  *id  = NULL;
	
	while(1)
	{
		if(USART3_RX_STA&0X8000)					               	//接收到一次数据了
		{ 
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;	            	//添加结束符
			
			if((id = (u8*)strstr((char*)USART3_RX_BUF,RECVE_FLAG)) != NULL)
				*id = *(id + strlen(RECVE_FLAG)) - '0';
			
			p = strstr((char*)USART3_RX_BUF,":");                   //WiFi接收到的数据在 : 之后
			if(p != NULL)
			{
				for(i = 0;i<car_at_len;i++)                         //轮循查找是否有小车指令
				{
					if( (p = strstr(p,car_hand[i].mes)) !=NULL )
					{
						p = strstr(p,"=");                           //判断是否有参数
						
						ret = car_hand[i].fun_hand(p+1);             //执行相应的指令函数
						if(ret != OK)
							wifi_respond("set fault!",ret,id);        //回复失败
						else
							wifi_respond("set ok!",ret,id);           //回复成功
						
						p  = NULL;
						id = NULL;
						break;
					}
				}
			}
			
			USART3_RX_STA=0;
		}
		
		vTaskDelay(100);		
	}
}

static u8 wifi_respond(char *buf,u8 err,u8 *id)
{
	char tmp[20];
	u8   ret = 0;
	
	if(*id < 5)
		sprintf(tmp,"AT+CIPSEND=%d,%d",*id,strlen(buf)+2);
	else
		sprintf(tmp,"AT+CIPSEND=%d",strlen(buf)+2);
	
	ret = wifi_cmd(tmp,">",100);
	
	memset(tmp,0,20);
	sprintf(tmp,"%s,%d",buf,err);
	ret = wifi_cmd(buf,SEND_ACK,100);
	
	return ret;
}


static int build_wifi(void)
{
	int ret = 0;
	
	/*复位WIFI模块*/
	ret = wifi_cmd(RESET,ACK,WAIT);
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		return TIMEOUT;
	}
	
	delay_ms(20);
	
	/*查看WIFI模块是否响应*/
	ret = wifi_cmd("AT",ACK,WAIT);
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		//return TIMEOUT;
	}
	
	/*WIFI模块设置为AP模式*/
	ret = wifi_cmd(AP,ACK,WAIT);
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		//return TIMEOUT;
	}
	
	/*设置IP地址*/
	//ret = wifi_cmd(IP,ACK,WAIT);
	while(wifi_cmd(IP,ACK,WAIT));
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		//return TIMEOUT;
	}

	/*建立WIFI服务*/
	//ret = wifi_cmd(BUILDAP,ACK,WAIT);
	while( wifi_cmd(BUILDAP,ACK,WAIT) );
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		//return TIMEOUT;
	}
	
	/*允许多个连接*/
	ret = wifi_cmd(MULT_LINK,ACK,WAIT);
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		//return TIMEOUT;
	}
	
	/*开启服务*/
	ret = wifi_cmd(START_SERVER,ACK,WAIT);
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		//return TIMEOUT;
	}
	
	/*设置连接超时时间*/
	ret = wifi_cmd(WAIT_TIME,ACK,WAIT);
	if(ret != OK)
	{
		printf("file:%s,line:%d,func:%s\n",__FILE__,__LINE__,__FUNCTION__);
		return TIMEOUT;
	}
	
	return OK;
}


static u8* atk_8266_check_cmd(char *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

static bool wifi_cmd(char *cmd,char *ack,u32 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	
	u3_printf("%s\r\n",cmd);	//发送命令
	
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					printf("%s ack:%s\r\n",(u8*)cmd,(u8*)ack);
					return res;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 




