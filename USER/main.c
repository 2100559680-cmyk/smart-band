#include "main.h"//主函数头文件


/**************************************************************************  
* 函数名: power_pin_init(void)
* 描述  : 一键开机
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void power_pin_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//	 PB12 POWER控制端口
 	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//速度
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_12);	 
}

/**************************************************************************  
* 函数名: display_startup(void)
* 描述  : 开机动画
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void display_startup(void)  //开机动画
{	
	OLED_DrawBMPFast(cartoon1);
	OLED_DrawBMPFast(cartoon2);
	OLED_DrawBMPFast(cartoon3);
	OLED_DrawBMPFast(cartoon1_1);
	OLED_DrawBMPFast(cartoon1_2);
	OLED_DrawBMPFast(cartoon1_3);
	OLED_DrawBMPFast(cartoon1_4);
	OLED_DrawBMPFast(cartoon1_5);
	OLED_DrawBMPFast(cartoon1_6);
	OLED_DrawBMPFast(cartoon1_5);
	OLED_DrawBMPFast(cartoon1_4);
	OLED_DrawBMPFast(cartoon1_3);
	OLED_DrawBMPFast(cartoon1_2);
	OLED_DrawBMPFast(cartoon1_1);
	OLED_DrawBMPFast(cartoon3);
	OLED_DrawBMPFast(cartoon2);
	OLED_DrawBMPFast(cartoon1);	

	delay_ms(300);
	OLED_Clear(); //OLED清屏
}

int main(void)
{
	power_pin_init();//一键开机
	
	/* 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	//延时函数初始化	
	Adc_Init();  	    	//ADC电压采集
	OLED_Init();            //OLED初始化
	OLED_Clear();           //OLED清屏
	sht20_init();  			//SHT20初始化
	LED_Init();				//LED初始化
	RTC_Init();	  			//RTC初始化	
//	usart_Init(115200);	 	//串口初始化为115200
	usart2_Init(9600);	 	//蓝牙初始化为9600
	
	KEY_Init();             //按键初始化
	BEEP_Init();            //蜂鸣器初始化
	
	max30102_init();        //max30102初始化
	max30102_blood();       //max30102数据处理初始化
	
//	PowerDownProtection_Read();//flash读函数
	display_startup();      //开机动画
	
	MPU_Init();             //MPU6050初始化	

	printf("%s\r\n","Start_OK...");   //温度
	while(mpu_dmp_init())
	{
		delay_ms(300);
	}	
	TIM4_Int_Init(199,7199);//mpu6050数据处理用、10Khz的计数频率，计数到200为20ms  
	TIM3_Int_Init(99,7199); //10Khz的计数频率，计数到100为100ms,OLED自动熄屏（用9999,即秒,跑到10s,时间失控，奇怪的bug）
	TIM2_Int_Init(99,7199); //10Khz的计数频率，计数到100为100ms,秒表用	
	TIM5_Int_Init(9999,7199);//10Khz的计数频率，计数到10000为1s，蓝牙发数据用	
	
//	Iwdg_Init();            //看门狗
    
	while(1)
	{
		GUI_Refresh();      //UI界面
	}		
}	



