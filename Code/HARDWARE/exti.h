#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define INT PBin(5)   //PB5连接到MPU6050的中断引脚
void EXTII_Init(void);	//外部中断初始化		 					    
#define KEY_UP  PAin(5)
#define KEY_DOWN  PAin(6)	
#define KEY_ENTER  PAin(7)
#endif

























