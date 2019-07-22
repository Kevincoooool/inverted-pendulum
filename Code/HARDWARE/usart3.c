#include "usart3.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Usart3_Receive;
 u8 mode_data[8];
 u8 six_data_stop[3]={0X59,0X59,0X59};  //停止数据样本
 u8 six_data_start[3]={0X58,0X58,0X58};  //启动数据样本

/**************************************************************************
函数功能：串口3初始化
入口参数： bound:波特率
返回  值：无
**************************************************************************/
void uart3_init(u32 bound)
{  	 
	  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能UGPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
	//USART3_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //USART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure);     //初始化串口3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口3 
}

/**************************************************************************
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{	  
		static	int uart_receive=0;//蓝牙接收相关变量
		static u8 Flag_PID,i,j,Receive[50];
		static float Data;
		uart_receive=USART_ReceiveData(USART3); 
		Usart3_Receive=uart_receive;
//		if(uart_receive==0x59)  Flag_sudu=2;  //低速挡（默认值）
//		if(uart_receive==0x58)  Flag_sudu=1;  //高速档
//		
//	  if(uart_receive>10)  //默认使用app
//    {			
//			if(uart_receive==0x5A)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
//			else if(uart_receive==0x41)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////前
//			else if(uart_receive==0x45)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////后
//			else if(uart_receive==0x42||uart_receive==0x43||uart_receive==0x44)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //左
//			else if(uart_receive==0x46||uart_receive==0x47||uart_receive==0x48)	    //右
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
//  	}
//		if(uart_receive<10)     //备用app为：MiniBalanceV1.0  因为MiniBalanceV1.0的遥控指令为A~H 其HEX都小于10
//		{			
//			Flag_sudu=1;//切换至高速档
//			if(uart_receive==0x00)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
//			else if(uart_receive==0x01)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////前
//			else if(uart_receive==0x05)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////后
//			else if(uart_receive==0x02||uart_receive==0x03||uart_receive==0x04)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //左
//			else if(uart_receive==0x06||uart_receive==0x07||uart_receive==0x08)	    //右
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
//  	}	

////		
//		if(Usart3_Receive==0x7B) Flag_PID=1;  //指令起始位
//		if(Usart3_Receive==0x7D) Flag_PID=2;  //指令停止位
//		 if(Flag_PID==1)                      //记录参数
//		 {
//			Receive[i]=Usart3_Receive;
//			i++;
//		 }
//		 if(Flag_PID==2)   //执行
//		 {
//			     //Show_Data_Mb=i;
//			     if(Receive[3]==0x50){} 	 //PID_Send=1;  //获取设备参数
//					 else  if(Receive[3]==0x57) 	 Flash_Send=1;   //掉电保存参数
//					 else  if(Receive[1]!=0x23)                    //更新PID参数
//					 {								
//						for(j=i;j>=4;j--)
//						{
//						  Data+=(Receive[j-1]-48)*pow(10,i-j);
//						}
//						switch(Receive[1])
//						 {
//							 case 0x30:  Balance_KP=Data/100;break;
//							 case 0x31:  Balance_KD=Data/100;break;
//							 case 0x32:  Position_KP=Data/100;break;
//							 case 0x33:  Position_KD=Data/100;break;
//							 case 0x34:  break;
//							 case 0x35:  break;
//							 case 0x36:  break;
//							 case 0x37:  break;
//							 case 0x38:  break;
//						 }
//					 }				 
//					 Flag_PID=0;   //相关标志位清零
//					 i=0;
//					 j=0;
//					 Data=0;
//					 memset(Receive, 0, sizeof(u8)*50);
//		 } 
	}  											 
} 
