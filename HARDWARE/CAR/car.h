#ifndef __CAR__
#define __CAR__

#include "sys.h"

/****** С������  *******/
struct CAR_OPTION
{
	volatile u8   search;      //����С���ı�״̬
	volatile u32  speed;       //С���ٶ�
	volatile u32  last_sp;     //����һ��С���ٶ�
	volatile u8   status;      //С��״̬
	volatile u8   mode;        //С������ģʽ
    volatile u16  dis;         //��࿪��
	volatile u8   turn;        //С��ת��
};


/***** С������ö�� ****/
typedef enum 
{
//����ģʽ
	GO = 1,
	STOP,
	BACK,
//ת��ֵ
	DIRECT,
	LEFT,
	RIGHT,
//����
	ON,
	OFF,
//״ֵ̬
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


