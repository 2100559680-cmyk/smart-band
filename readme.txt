实验器材:
	STM32F103C8T6+SHT20温湿度+无源蜂鸣器+MPU6050+0.96寸OLED(IIC)显示屏+HC-05蓝牙模块+USB转TTL电平工具
	
实验目的:智能手环的设计与实现(毕业设计)

	
硬件资源:
	BEEP:            PB8
	ADC:             PA0
	按键：           KEY1接PA4      KEY2接PA5        KEY3接PB4         KEY4接PB5
	SHT20:           SCL接PA11      SDA接PA12
	MPU6050：        SCL接PB6       SDA接PB7
	MAX30102:        SCL接PB10      SDA接PB11        INT接PB9
	OLED：           SCL接PA6       SDA接PA7
	指示灯：         LED0接PA8      LED1接P
	串口1：          RX接PA9        TX接PA10
	HC-05:           RX接PA2        TX接PA3
实验现象:
	通过蓝牙将温湿度、心率血氧、运动数据上报，OLED实时显示数据。
	********************************************************
	设定时间:TIME-"时"-"分"-"秒"
	设定日期:DAYS-"年"-"月"-"日"
	设定闹钟1:ALARM1-"0/1(开/关)"-"时"-"分"
	设定闹钟2:ALARM2-"0/1(开/关)"-"时"-"分"
	关闭发送:OneRxOFF 					
	温度:Temperature 				
	湿度:Humidness 					
	心率:HeartRate 					
	血氧:BloodOxygen 				
	步数:StepNumber 					
	运动时间:ExerciseTime 				
	消耗能量:ConsumeEnergy 				
	运动温度:KineticTemperature			
	俯仰角:AngleOfPitch				
	横滚角:RollAngle 	
	定时发送数据开:RXON
	定时发送数据关:RXOFF
	***注：字符串结尾+“#”校准***
	********************************************************
注意事项:
	注意接线，硬件平台选择和适配。

该工程借鉴了很多平台的程序设计，本人能力有限，设计难免有问题和漏洞，欢迎大家和我交流。
	
			桂林电子科技大学 
			容之武设计
			QQ:3011678262
			2022年11月11日
			