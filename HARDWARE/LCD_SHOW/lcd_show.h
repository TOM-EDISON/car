#ifndef __LCD_SHOW
#define __LCD_SHOW

#include "sys.h"

/*
* use   :初始化显示界面
* func  :lcd_show_init
* arg   :void
* return:void
*/
void lcd_show_init(void);


/*
* use   :显示距离
* func  :lcd_show_dis
* arg   :距离的地址,显示长度,显示颜色
* return:void
*/
void lcd_show_dis(u32* data,u8 len,u16 color);



#endif

