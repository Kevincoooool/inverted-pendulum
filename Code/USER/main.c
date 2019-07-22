#include "sys.h"


int Encoder=10000,Position_Zero=10000,Velocity;            //编码器的脉冲计数
int Moto=0;                                   //电机PWM变量 应是Motor的 向Moto致敬	
int Voltage;                                //电池电压采样相关的变量
float Balance_KP=30,Balance_KD=110,Balance_Ratio=0.01,Position_KP=45,Position_KD=120;  //PID系数
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash相关数组
u8 Flag_OLED,State;  //State是倒立摆状态 1：倒立状态  0：非倒立状态
//u8 func=0,X_flag=0;
//int local_pos;
int main(void)
{ 
	Stm32_Clock_Init(9);            //=====系统时钟设置
	delay_init();                 //=====延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);     //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //=====打开SWD接口 可以利用主板的SWD接口调试
	delay_ms(1000);                 //=====延时启动，等待系统稳定
	delay_ms(1000);                 //=====延时启动，等待系统稳定 共2s
	LED_Init();                     //=====初始化与 LED 连接的硬件接口
	EXTII_Init();                    //=====按键初始化(外部中断的形式)
	OLED_Init();                    //=====OLED初始化
	uart_init(115200);    
	//uart3_init(115200);
	//=====初始化串口1
	Moto_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机 
	Encoder_Init_TIM4();            //=====初始化编码器（TIM2的编码器接口模式） 
	Angle_Adc_Init();               //=====角位移传感器模拟量采集初始化
	Baterry_Adc_Init();             //=====电池电压模拟量采集初始化
	//Flash_Read();                   //=====从Flash里面读取参数
	Timer1_Init(49,7199);           //=====定时中断初始化 
	while(1)
	{
		function();
		oled_show();              //===显示屏打开	  	
	} 
}
