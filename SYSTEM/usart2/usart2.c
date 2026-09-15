#include "usart2.h"

extern uint32_t 	g_usart2_cnt;				//串口1接收数据计数
extern uint32_t		g_usart2_event;			//串口1接收数据的事件
extern uint8_t 		g_usart2_buf[128];		//串口1接收数据缓冲区


//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	USART2->DR = (u8) ch;   
	
	//清空发送完成标志位
	USART2->SR = (uint16_t)~USART_FLAG_TC;
	return ch;
}
  

void usart2_Init(uint32_t baud)
{	
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);
  
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART2_RX	  GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = baud;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式	

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  

	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口1 
}

//AT指令配置模块
void usart2_set_name(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/************EN引脚电平拉高，进入AT命令模式。退出AT模式，EN引脚电平拉低。*************/
	//打开PA硬件时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;//		   //串口输出PA4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//输出高电平
	
	//发送AT的测试指令
	usart2_send_str("AT\r\n");
	delay_ms(500);
	
	//配置蓝牙模块的名字,名字不能太长（需要复位生效，如果发送AT+RESET搜索蓝牙模块名字没有变更，那么请重新对蓝牙模块上电）
	usart2_send_str("AT+NAME=wfj15\r\n");	
	delay_ms(500);
	 
	 //发送复位模块的指令
	usart2_send_str("AT+RESET\r\n");
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4);  //输出低电平
	delay_ms(2000);
}


void usart2_send_str(char *pstr)
{
	char *p = pstr;	
	while(*p!='\0')
	{	
		USART_SendData(USART2,*p);
		//指针偏移
		p++;
		//等待数据发送成功
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	}	
}


void USART2_IRQHandler(void)
{
	uint8_t d = 0;
	//检测标志位
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//接收数据
		d=USART_ReceiveData(USART2);
		
		g_usart2_buf[g_usart2_cnt] = d;
		
		g_usart2_cnt++;
		
		if(d == '#'&& g_usart2_cnt>= sizeof(g_usart2_buf))
		{
			g_usart2_event=1;
		}	
		//清空串口接收中断标志位
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	} 
}


