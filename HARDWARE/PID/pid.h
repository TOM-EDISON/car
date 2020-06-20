#ifndef __PID
#define __PID

struct PID
{
	float p;
	float i;
	float d;
	float err;
	float err1;
	float err2;
};


/*
* use   :初始化pid参数
* func  :pid_init
* arg   :void
* return:void
*/
void pid_init(void);


/*
* use   :设置pid参数
* func  :set_pid
* arg   :p,i,d
* return:void
*/
void set_pid(float* p,float* i,float* d);


/*
* use   :获取pid参数
* func  :get_pid
* arg   :void
* return:pid结构体
*/
struct PID* get_pid(void);



/*
* use   :pid计算
* func  :pid_realize
* arg   :目标值的地址,实际值的地址
* return:计算后的值
*/
int pid_realize(int* des_v,int* real_v);




#endif


