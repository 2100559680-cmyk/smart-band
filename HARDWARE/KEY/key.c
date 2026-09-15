#include "key.h"

//按键初始化函数
void KEY_Init(void) //IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTC时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;//PA4、PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;//PB4、PB5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC5
}
/*
函数功能：获取键值
参数：无
返回值：返回键值，按键按下(2,3,4,5),无按键按下(0)
*/
u8 Get_KEY_Value(u8 mode)
{
	static u8 Flag=1;//按键按松开标志
	if(mode)Flag=1;  //支持连按
	if((!KEY_UP||!KEY_DOWN||!KEY_LEFT||!KEY_RIGHT)&&Flag==1)
	{
		Flag=0;
		delay_ms(15);//按键消抖
		if(KEY_UP==0)return 2;
		if(KEY_LEFT==0)return 3;
		if(KEY_DOWN==0)return 4;
		if(KEY_RIGHT==0)return 5;
	}
	if((KEY_UP&&KEY_DOWN&&KEY_LEFT&&KEY_RIGHT)&&Flag==0)Flag=1;//检测是否松开
	return 0;
}
