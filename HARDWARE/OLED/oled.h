#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	    
////////////////////////////////////////////////////////////////////////////////// 	  
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    			    						  
//-----------------OLED IIC端口定义---------------- 
#define SCL				PAout(6)
#define SDA_W			PAout(7)
#define SDA_R			PAin(7)
 		     
#define OLED_CMD  0		//写命令
#define OLED_DATA 1		//写数据

//OLED控制用函数
void OLED_Refresh(void);
void OLED_ClearPoint(u8 x,u8 y);
u8 OLED_IsPoint(u8 x,u8 y);
void OLED_Refresh_Clear(void);

void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Clear_Line(uint8_t line);  
//void OLED_DrawPoint(uint8_t x,uint8_t y);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void dis_DrawCurve(u32* data,u8 x);//max30102数据画点处理
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
//void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
//void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]);// 快速绘制图像
void Delay_50ms(unsigned int Del_50ms);
void Delay_1ms(unsigned int Del_1ms);
void fill_picture(unsigned char fill_Data);
void Picture(void);
void i2c_start(void);
void i2c_stop(void);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
uint32_t i2c_wait_ack(void);

/******************2048游戏相关*******************************/
void OLED_DrawLine(u8 x,u8 y,u8 wide,u8 high,u8 dat);
void OLED_ShowChar_2048(u8 x,u8 y,u8 chr,u8 Size);
void OLED_ShowNum_2048(u8 x,u8 y,u32 num,u8 len,u8 size);
extern void draw_grid(void);
#endif  
	 



