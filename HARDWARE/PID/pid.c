#include "pid.h"


#define PUTMAX 100

#define putlimit(value) value>PUTMAX?PUTMAX:value

static struct PID pid = 
{
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0
};

void pid_init(void)
{
	pid.p    = 0.0;
	pid.i    = 0.0;
	pid.d    = 0.0;
	pid.err  = 0.0;
	pid.err1 = 0.0;
	pid.err2 = 0.0;
	
	return;
}


void set_pid(float* p,float* i,float* d)
{
	pid.p = *p;
	pid.i = *i;
	pid.d = *d;
	
	return;
}


struct PID* get_pid(void)
{
	return &pid;
}


int pid_realize(int* des_v,int* real_v)
{
	int   output = 0;
	
	pid.err = *des_v - *real_v;
	output = (int)(pid.p * (pid.err - pid.err1) +
				   pid.i * pid.err +
				   pid.d * (pid.err - 2 * pid.err1 + pid.err2));
	
	return putlimit(output);
}



