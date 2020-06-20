#ifndef __CAR__
#define __CAR__

#include "sys.h"

/****** 小车属性  *******/
struct CAR_OPTION
{
	volatile u8   search;      //搜索小车改变状态
	volatile u32  speed;       //小车速度
	volatile u32  last_sp;     //保留一次小车速度
	volatile u8   status;      //小车状态
	volatile u8   mode;        //小车运行模式
    volatile u16  dis;         //测距开关
	volatile u8   turn;        //小车转向
};


/***** 小车属性枚举 ****/
typedef enum 
{
//启动模式
	GO = 1,
	STOP,
	BACK,
//转向值
	DIRECT,
	LEFT,
	RIGHT,
//开关
	ON,
	OFF,
//状态值
	RUN,
	WAIT,
	FAULT,
}CAR_MODE;


struct CAR_OPTION* get_car_op(void);


void car_init(void);

int car_go_hand(char*);

int car_stop_hand(char*);

int car_back_hand(char*);

int car_speed_hand(char*);

int car_dis_hand(char*);

int car_right_hand(char*);

int car_left_hand(char*);

#endif


