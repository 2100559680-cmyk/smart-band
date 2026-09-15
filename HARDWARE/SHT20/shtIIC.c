#include "shtIIC.h"

/*起止信号：SCL为高电平期间，SDA由高电平向低电平的跳变。*/
void IIC_Startsht()
{
	IIC_SHTSDA_SHTOUT();//输出模式
	IIC_SHTSCL=1;//保持数据稳点
	IIC_SHTSDA=1; 
	delay_us(5);
	IIC_SHTSCL=1;
	IIC_SHTSDA=0; //SDA由高电平向低电平的跳变,起始信号
	delay_us(5);
	IIC_SHTSCL=0;//发送数据做准备
  delay_us(5);
}
/*终止信号：SCL为高电平期间，SDA由低电平向高电平的跳变。*/
void IIC_Stopsht(void)
{
	IIC_SHTSCL=0;//保正数据上一个是传输数据状态
	IIC_SHTSDA_SHTOUT();//输出模式
	IIC_SHTSDA=0; //SDA低电平作为起始状态
	delay_us(5);
	IIC_SHTSCL=1;//保持时钟线为高
	IIC_SHTSDA=1;//拉高数据线结束发送
	delay_us(5);
}
/*应答信号：每个字节（8bit）传输完成后的下一个时钟信号，*/
/*在SCL为高电平期间，SDA为低，则表示一个应答信号（ACK）。*/
void IIC_ACKsht(void)
{	
	IIC_SHTSDA_SHTOUT();//输出模式
	IIC_SHTSCL = 0;	//SCL为低电平时才能改变SDA的状态
	IIC_SHTSDA=0;//为1时发送应答第9个时钟（释放数据线）
	delay_us(5);
	IIC_SHTSCL=1;//先保持高电平
	delay_us(5);
	IIC_SHTSCL=0;//由高变低电平
	IIC_SHTSDA=0;//表示应答
	delay_us(5);
	IIC_SHTSDA=1;
	
}
/*非应答信号：每个字节（8bit）传输完成后的下一个时钟信号，*/
/*在SCL为高电平期间，SDA为高，则表示一个非应答信号（NACK）。*/
void IIC_NACKsht(void)
{
	IIC_SHTSCL = 0;	//SCL为低电平时才能改变SDA的状态
	IIC_SHTSDA_SHTOUT();//输出模式
	IIC_SHTSDA=1;//为1时发送应答第9个时钟（释放数据线）
	IIC_SHTSCL=1;//先保持高电平
	delay_us(5);
	IIC_SHTSDA=1;//表示非应答
	delay_us(5);
}
/*从机给应答：返回0：应答，返回1：非应答*/
unsigned char IIC_Wait_ACKsht(void)
{
	IIC_SHTSDA_IN();	//输入模式
	IIC_SHTSCL = 1;	//拉高SCL，SDA保持稳定
	delay_us(5);
	if(IIC_SHTSDAIN)	//如果数据线为高，那么表示非应答信号
	{
		IIC_Stopsht();
		return 1;//表示非应答
	}
//如果输入进来的是0
	IIC_SHTSCL = 0;	//拉低SCL，准备下一个数据的接收
	return 0;//应答
}
/*MSB（高位在前）*/
void IIC_WriteBytesht(unsigned char data)
{
	unsigned char count = 0;
	IIC_SHTSDA_SHTOUT();	//输出模式
	for(count=0;count<8;count++)
	{
		IIC_SHTSCL = 0;	//SCL为低电平时才能改变SDA的状态
		if(data&0x80)	//1000 0000
			IIC_SHTSDA = 1;
		else			
			IIC_SHTSDA = 0;
		data <<= 1;
		delay_us(5);
		IIC_SHTSCL = 1;	//数据已准备好，通知从机读取
		delay_us(5);
	}
	IIC_SHTSCL = 0;
	delay_us(5);
}
/*读取一个字节，ack = 1，发送应答信号，ack = 0，发送非应答信号*/
unsigned char IIC_ReadBytesht(unsigned char ack)
{
	unsigned char count = 0, data = 0;
	IIC_SHTSDA_IN();	//输入模式
	for(count=0;count<8;count++)
	{
		IIC_SHTSCL = 0;	//通知从机准备数据
		delay_us(5);
		IIC_SHTSCL = 1;	//从机已经准备好数据了
		data <<= 1;		//准备一个空位
		data |= !!IIC_SHTSDAIN;	//二值化，确保结果只有1和0
		delay_us(5);
	}
	if(ack)	
		IIC_ACKsht();		//根据形参决定发送应答和非应答信号
	else	
		IIC_NACKsht();	
	return data;
}
