#include "lcd_show.h"
#include "lcd.h"
#include "car.h"


void lcd_show_init(void)
{
	struct CAR_OPTION *car_show;
	
	car_show = get_car_op();
	
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	
	LCD_ShowString(15,20,200,24,24,"*******************");
	LCD_ShowString(50,50,200,24,24,"DYQ PLAY CAR");
	LCD_ShowString(15,75,200,24,24,"*******************");
	LCD_ShowString(0,120,200,16,16,"status  mode  speed  dis");
	
	//显示状态
	if(car_show->status == 9)
		LCD_ShowString(0,140,200,16,16,"RUN");
	else if(car_show->status == 10)
		LCD_ShowString(0,140,200,16,16,"WAIT");
	else if(car_show->status == 11)
		LCD_ShowString(0,140,200,16,16,"FAULT");

	//显示模式
	if(car_show->mode == 1)
		LCD_ShowString(62,140,200,16,16,"GO");
	else if(car_show->mode == 2)
		LCD_ShowString(62,140,200,16,16,"STOP");
	else if(car_show->mode == 3)
		LCD_ShowString(62,140,200,16,16,"BACK");
	
	LCD_ShowxNum(104,140,car_show->speed,4,16,0);
	
	//显示测距开关
	if( car_show->dis == 8 )
		LCD_ShowString(164,140,200,16,16,"OFF");
	else
		LCD_ShowxNum(164,140,car_show->dis,3,16,0);

//	LCD_ShowString(50,130,200,16,16,"wifi name:DYQ");
//	LCD_ShowString(50,160,200,16,16,"passwprd:Dyq123456");
//	LCD_ShowString(50,190,200,16,16,"IP:192.168.43.1");
//	LCD_ShowString(50,220,200,16,16,"Port:8888");
	
	return;
}


void lcd_show_dis(u32* data,u8 len,u16 color)
{
	POINT_COLOR=color;
	LCD_ShowxNum(50,240,*data,len,16,0);
	
	return;
}



