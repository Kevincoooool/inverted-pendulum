#include "control.h"		
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define ZHONGZHI 1052
#define ORIGIN 3225

int Balance_Pwm,Count_P2,Position_Pwm,Moto_qb,Count_Moto;
u8 Flag_Target,Flag_qb,Flag_qb2;
int Voltage_Temp,Voltage_Count,Voltage_All;
double Count_qb,Ratio=1;
u32 Count_Position;
u8 Flag_Position;
u16 target = 1080,Tar_pos=10000;
u16 angle0,angle1;             //依次为当前角度，上次角度，上次位置    
int all_pwm,target_a;                   //依次为耦合后的pwm值，修正后的预期值
int pos0,pos1,speed,pos_pwm,pos_angle; //位置，速度，位置积分，位置pid，位置偶合成的角度值
int ap_error,ad_error,ai_error;                //角度差，角加速度
int nu=0;							//计数积分次数
u8 func2=0;   							//功能2的标志位
//整体思路：通过将位置pid得出的参数整合到角度中实现与角度环的耦合
u8 func=0,X_flag=0;
int local_pos;
u16 kpa=24,kia=53,kda=150;//角度环的pid参数
u16 kpv=15,kdv=150;  //位置环的pd参数

void pid(void)
{
	angle0=Get_Adc_Average(3,12);         //读出角度值  
	
//	if(count>count_exp+500) count=count_exp;        //限幅
//  else if(count<count_exp-500)	count=count_exp;
	Encoder = Read_Encoder(4);
	
	if(X_flag==1)
	{
		
		Tar_pos+=1;
		if(Encoder-local_pos>1800)
		{
			X_flag=0;
		}
	}
	pos0=Encoder-Tar_pos;                //读出当前位置差 pos0
	speed=pos0-pos1;                 //位置的微分 即速度
	pos_pwm=kpv*pos0+kdv*speed;      //位置pid
	
	pos_angle=pos_pwm/32;               //整合成角度

	if(pos_angle>170) pos_angle=170;        //对整合值限幅
	else if(pos_angle<-170) pos_angle=-170;
	target_a=target-pos_angle;          //产生新的预期值

	ap_error=target_a-angle0;             //耦合后角度差
	ad_error=angle0-angle1;	              //角度微分  
	
	if(myabs(target-angle0)<300)      
	{	
		ai_error=ai_error+target-angle0;    //角度积分
		all_pwm=kpa*ap_error+kia*ai_error/100-kda*ad_error;    //耦合后的pid调节
	}
	else  all_pwm=0;
	angle1=angle0;
	pos1=pos0;               //锁值
	nu++;                                 //给积分项限制次数，防止无限大
	if(nu==200) ai_error=0,nu=0;
}

void TIM3_IRQHandler(void)              //定时器5中断函数
{
	static u8 SpCount=0,LedCount=0,count0;//SpCount速度环调节计数，LedCount状态灯调节计数
	static s16 Angle_Pwm,Speed_Pwm;//角度环和速度环PWM输出
	 
	if(TIM3->SR&0X0001)//溢出中断
	{	
		pid();
		Set_Pwm(all_pwm);
		Led_Flash(100);                         //===LED闪烁指示系统正常运行 
		Voltage_Temp=Get_battery_volt();		    //=====读取电池电压		
		Voltage_Count++;                        //=====平均值计数器
		Voltage_All+=Voltage_Temp;               //=====多次采样累积
		if(Voltage_Count==100) Voltage=Voltage_All/100,Voltage_All=0,Voltage_Count=0;//=====求平均值		
	}	
  TIM3->SR&=~(1<<0);
}	
	u8 i =0;
void function(void)//倒立摆中扩展功能1（起摆），功能2（摆臂旋转1圈）实现函数
{
	switch(func)
	{
		case(10):
			TIM_Cmd(TIM3,DISABLE); 
			for(i=0;i<3;i++)
			{
				Set_Pwm(2000);
				delay_ms(500);
				Set_Pwm(-2000);
				delay_ms(500);
			}
			//TIM_Cmd(TIM3,ENABLE); 
			Set_Pwm(0);
			func=0;
		break;
		case(20):
			TIM_Cmd(TIM3,DISABLE); 
			for(i=0;i<3;i++)
			{
				Set_Pwm(3000);
				delay_ms(500);
				Set_Pwm(-2000);
				delay_ms(200);
			}
			//TIM_Cmd(TIM3,ENABLE);
			Set_Pwm(0);			
			func=0;
		break;

		case(30):
			TIM_Cmd(TIM3,ENABLE); 
			Tar_pos= TIM4->CNT;
			func=0;
		break;
        case(40):
			TIM_Cmd(TIM3,DISABLE); 
			Set_Pwm(3000);
			delay_ms(430);
			Set_Pwm(-1700);
			delay_ms(160);
			TIM_Cmd(TIM3,ENABLE); 
			delay_ms(300);
			Tar_pos= TIM4->CNT;
			func=0;
		break;
		case(50):
			TIM_Cmd(TIM3,ENABLE);
			Tar_pos= TIM4->CNT;		
			func=0;
		case(60):
			TIM_Cmd(TIM3,ENABLE); 
			local_pos=TIM4->CNT;
			X_flag=1;
			func=0;
		break;
        
        default:break;
    }
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto)
{
    	if(moto<0)			AIN2=1,			AIN1=0;
			else 	          AIN2=0,			AIN1=1;
			PWMA=myabs(moto);//简单的TB6612 控制
}

int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
