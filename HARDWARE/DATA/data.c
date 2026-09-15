#include "data.h"



void Delay(__IO u32 nCount)	//简单的延时函数
{
	for(; nCount != 0; nCount--);
} 

void myitoc(int myint, char * myascall)//int转ascall函数
{
    int len = 0, tmpe = myint;
    
    while(tmpe)//计算int型数据位数
    {
        len++;
        tmpe /= 10;
    }
    
    while(myint)//循环转换每一位
    {
        tmpe = myint%10;//取出最低位
        myascall[len-1] = (tmpe+48);//0的ascall码加偏移量

        myint /= 10;//去掉最低位
        len--;
    }
}


 

