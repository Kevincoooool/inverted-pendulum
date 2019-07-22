#include "usart3.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Usart3_Receive;
 u8 mode_data[8];
 u8 six_data_stop[3]={0X59,0X59,0X59};  //ֹͣ��������
 u8 six_data_start[3]={0X58,0X58,0X58};  //������������

/**************************************************************************
�������ܣ�����3��ʼ��
��ڲ����� bound:������
����  ֵ����
**************************************************************************/
void uart3_init(u32 bound)
{  	 
	  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��UGPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
	//USART3_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //USART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure);     //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
	{	  
		static	int uart_receive=0;//����������ر���
		static u8 Flag_PID,i,j,Receive[50];
		static float Data;
		uart_receive=USART_ReceiveData(USART3); 
		Usart3_Receive=uart_receive;
//		if(uart_receive==0x59)  Flag_sudu=2;  //���ٵ���Ĭ��ֵ��
//		if(uart_receive==0x58)  Flag_sudu=1;  //���ٵ�
//		
//	  if(uart_receive>10)  //Ĭ��ʹ��app
//    {			
//			if(uart_receive==0x5A)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//			else if(uart_receive==0x41)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ǰ
//			else if(uart_receive==0x45)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////��
//			else if(uart_receive==0x42||uart_receive==0x43||uart_receive==0x44)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //��
//			else if(uart_receive==0x46||uart_receive==0x47||uart_receive==0x48)	    //��
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//  	}
//		if(uart_receive<10)     //����appΪ��MiniBalanceV1.0  ��ΪMiniBalanceV1.0��ң��ָ��ΪA~H ��HEX��С��10
//		{			
//			Flag_sudu=1;//�л������ٵ�
//			if(uart_receive==0x00)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//			else if(uart_receive==0x01)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ǰ
//			else if(uart_receive==0x05)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////��
//			else if(uart_receive==0x02||uart_receive==0x03||uart_receive==0x04)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //��
//			else if(uart_receive==0x06||uart_receive==0x07||uart_receive==0x08)	    //��
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//  	}	

////		
//		if(Usart3_Receive==0x7B) Flag_PID=1;  //ָ����ʼλ
//		if(Usart3_Receive==0x7D) Flag_PID=2;  //ָ��ֹͣλ
//		 if(Flag_PID==1)                      //��¼����
//		 {
//			Receive[i]=Usart3_Receive;
//			i++;
//		 }
//		 if(Flag_PID==2)   //ִ��
//		 {
//			     //Show_Data_Mb=i;
//			     if(Receive[3]==0x50){} 	 //PID_Send=1;  //��ȡ�豸����
//					 else  if(Receive[3]==0x57) 	 Flash_Send=1;   //���籣�����
//					 else  if(Receive[1]!=0x23)                    //����PID����
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
//					 Flag_PID=0;   //��ر�־λ����
//					 i=0;
//					 j=0;
//					 Data=0;
//					 memset(Receive, 0, sizeof(u8)*50);
//		 } 
	}  											 
} 