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
* use   :��ʼ��pid����
* func  :pid_init
* arg   :void
* return:void
*/
void pid_init(void);


/*
* use   :����pid����
* func  :set_pid
* arg   :p,i,d
* return:void
*/
void set_pid(float* p,float* i,float* d);


/*
* use   :��ȡpid����
* func  :get_pid
* arg   :void
* return:pid�ṹ��
*/
struct PID* get_pid(void);



/*
* use   :pid����
* func  :pid_realize
* arg   :Ŀ��ֵ�ĵ�ַ,ʵ��ֵ�ĵ�ַ
* return:������ֵ
*/
int pid_realize(int* des_v,int* real_v);




#endif


