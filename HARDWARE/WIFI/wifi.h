#ifndef __WIFI
#define __WIFI

#include "sys.h"

typedef int (*func)(char*);
struct CAR_AT
{
	char mes[20];
	func fun_hand;
};


void wifi_init(void);



#endif


