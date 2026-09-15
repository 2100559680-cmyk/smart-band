#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"

extern void delay_us(u32 nus);
extern void delay_ms(u16 nms);

static GPIO_InitTypeDef   			GPIO_InitStructure;

u8 OLED_GRAM[144][8];

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 

//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
void OLED_ShowNum_2048(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar_2048(x+(size/2)*t,y,' ',size);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar_2048(x+(size/2)*t,y,temp+'0',size); 
	}
} 

void OLED_ShowChar_2048(u8 x,u8 y,u8 chr,u8 Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>Max_Column-1){x=0;y=y+2;}
	if(Size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		{
			//OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_GRAM[x+i][y] = F8X16[c*16+i];
		}
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		{
			//OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			OLED_GRAM[x+i][y+1] = F8X16[c*16+i+8];
		}
	}
		
	else 
	{	
		OLED_Set_Pos(x,y+1);
		for(i=0;i<6;i++)
		OLED_WR_Byte(F6x8[c][i],OLED_DATA);
		
	}
}

void draw_grid(void)	//先显示数字，在调用该函数画线
{   
	uint8_t x,y;
	for(y=0;y<64;)
	{
		for(x=0;x<128;x++)
		{
			OLED_DrawPoint(x,y,0);
		}
		
		y += 16;
		if(y==64)
			y=63;
	}
	
	for(y=0;y<8;y++)
	{
		for(x=0;x<128;)
		{
			OLED_GRAM[x][y] =0xff;
			x += 32;
			if(x==128)
				x=127;
		}		
	}
	
	OLED_Refresh();	
}
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	OLED_GRAM[x][i]|=n;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
}

u8 OLED_IsPoint(u8 x,u8 y)
{
	u8 i,m,n,temp;
	i=y/8;
	m=y%8;
	n=1<<m;
	temp=OLED_GRAM[x][i];
	if(temp & n)	temp = 1;
	else	temp = 0;
	return temp;
}
//更新显存到LCD	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		for(n=0;n<128;n++)
		OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}

void OLED_Refresh_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}

//void OLED_DrawPoint(u8 x,u8 y)
//{
//	u8 i,m,n;
//	i=y/8;
//	m=y%8;
//	n=1<<m;
//	OLED_GRAM[x][i]|=n;
//}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}


void dis_DrawCurve(u32* data,u8 x)    //max30102数据画点处理
{
	u16 i;
	u32 max=0,min=262144;
	u32 temp;
	u32 compress;
	
	for(i=0;i<128*2;i++)
	{
		if(data[i]>max)
		{
			max = data[i];
		}
		if(data[i]<min)
		{
			min = data[i];
		}
	}
	
	compress = (max-min)/20;
	
	for(i=0;i<128;i++)
	{
		temp = data[i*2] + data[i*2+1];
		temp/=2;
		temp -= min;
		temp/=compress;
		if(temp>20)temp=20;
		OLED_DrawPoint(i,63-x-temp,1);
	}
}


void i2c_sda_mode(GPIOMode_TypeDef mode)
{
	/* 配置PA7引脚为输出模式 */	  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = mode; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

void i2c_start(void)
{

	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_Out_PP);
	
	SCL=1;
	SDA_W=1;
	
	//延时1us
	delay_us(1);
	
	SDA_W=0;
	
	//延时1us
	delay_us(1);	
	
	
	//占用时钟线，告诉其他从机这是忙的状态
	SCL=0;
	
	//延时1us
	delay_us(1);
}

/**********************************************
//IIC Stop
**********************************************/
void i2c_stop(void)
{
	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_Out_PP);
	
	SCL=0;
	SDA_W=0;

	//延时1us
	delay_us(1);
	
	SCL=1;	
	
	//延时1us
	delay_us(1);	
	
	SDA_W=1;

	//延时1us
	delay_us(1);	
}

uint32_t i2c_wait_ack(void)
{

	uint32_t ack=0;
	
	//保证SDA引脚为输入模式
	i2c_sda_mode(GPIO_Mode_AIN);
	
	SCL=1;
	
	//延时1us
	delay_us(1);
	
	//判断SDA引脚的电平
	if(SDA_R)
	{
		ack=1;
		
		//中止整个i2c通信
		i2c_stop();
	}
	
	SCL=0;
	
	//延时1us
	delay_us(1);
	
	return ack;
}

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	uint32_t i=0;
	
	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_Out_PP);
	
	SCL=0;
	SDA_W=0;

	//延时1us
	delay_us(1);
	
	for(i=0; i<8; i++)
	{
		//以MSB最高有效位进行数据的发送
		if(IIC_Byte & (1<<(7-i)))
			SDA_W=1;
		else
			SDA_W=0;
	
		//延时1us
		delay_us(1);
		
		//设置时钟线为高电平，告诉从机现在可以读取SDA引脚的电平
		SCL=1;	
		
		//延时1us
		delay_us(1);	

		//设置时钟线为低电平，告诉从机现在不可以读取SDA引脚的电平，因为主机现在要更改SDA引脚的电平
		SCL=0;	
		
		//延时1us
		delay_us(1);			
	}
}

void Write_IIC_Command(unsigned char IIC_Command)
{
	i2c_start();
	Write_IIC_Byte(0x78);         
	i2c_wait_ack();	
	Write_IIC_Byte(0x00);		
	i2c_wait_ack();	
	Write_IIC_Byte(IIC_Command); 
	i2c_wait_ack();	
	i2c_stop();
}

void Write_IIC_Data(unsigned char IIC_Data)
{
	i2c_start();
	Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	i2c_wait_ack();	
	Write_IIC_Byte(0x40);			//write data
	i2c_wait_ack();	
	Write_IIC_Byte(IIC_Data);
	i2c_wait_ack();	
	i2c_stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
			{

   Write_IIC_Data(dat);
   
		}
	else {
   Write_IIC_Command(dat);
		
	}


}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1);
			}
	}
}

//坐标设置

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

void OLED_Clear_Line(uint8_t line)  
{  
	uint8_t n;		    

	OLED_WR_Byte (0xb0+line,OLED_CMD);    //设置页地址（0~7）
	OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
	OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址  
	
	for(n=0;n<128;n++)
		OLED_WR_Byte(0,OLED_DATA); 

}


void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>Max_Column-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else {	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WR_Byte(F6x8[c][i],OLED_DATA);
		
	}
}
			  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
//显示汉字
//void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
//{      			    
//	uint8_t t,adder=0;
//	OLED_Set_Pos(x,y);	
//    for(t=0;t<16;t++)
//		{
//				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
//				adder+=1;
//     }	
//		OLED_Set_Pos(x,y+1);	
//    for(t=0;t<16;t++)
//			{	
//				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
//				adder+=1;
//      }					
//}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
//void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
//{ 	
//	unsigned int j=0;
//	unsigned char x,y;

//	if(y1%8==0) 
//		y=y1/8;      
//	else 
//		y=y1/8+1;
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//		for(x=x0;x<x1;x++)
//		{      
//			
//			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
//		}
//	}
//} 

// 快速绘制图像
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y);
		i2c_start();
		Write_IIC_Byte(0x78);
		i2c_wait_ack();
		Write_IIC_Byte(0x40);
		i2c_wait_ack();
		for (x = x0; x < x1; x++)
		{
			Write_IIC_Byte(BMP[j++]);
			i2c_wait_ack();
		}
		i2c_stop();
	}
}


//初始化OLED				    
void OLED_Init(void)
{ 	
 	GPIO_InitTypeDef  GPIO_InitStructure;
  	//使能端口A的时钟 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );
	/* 配置PA6 PA7引脚为输出模式 */	  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

//	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);
	delay_ms(200);

	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  

//每调用一次OLED_WR_Byte()就向屏幕填写一列(每列8个点)，dat为1的位即点亮对应的一个像素点
//		OLED_DrawLine(8,0,3,8,0xff);//把每一行的x=8到x=10的点都点亮，即从x=8,y=0开始画一条宽等于3，高等于8的线
//							  0xf0为在一列像素点中的下面4个像素点点亮
//会改变所有像素点原来的状态
void OLED_DrawLine(u8 x,u8 y,u8 wide,u8 high,u8 dat)
{
	u8 i,j;
	if(y>=8 || y+high>8)
		return;
	if(x>=128 || x+wide>128)
		return;
	
	for(i=y;i<y+high;i++)
	{	
		OLED_Set_Pos(x, i); 
		for(j=x;j<x+wide;j++)
			OLED_WR_Byte(dat,OLED_DATA);
	}
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh();//更新显示
}

