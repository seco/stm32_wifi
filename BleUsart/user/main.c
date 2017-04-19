/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-01-09
	*
	*	版本： 		V1.0
	*
	*	说明： 		USART1打印
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "led.h"
//C库
#include <string.h>


//全局变量
unsigned char usart3Len = 0;	//usart3接收的数据长度
char usart3Buf[64];	//usart3接收缓存



/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断控制器分组设置

	Delay_Init();										//Systick初始化，用于普通的延时
	
	ledGpioInit();
	
	Usart3_Init(9600);								//初始化串口 3 9600

}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	USART3打印数据
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{

	Hardware_Init();									//硬件初始化
	
	UsartPrintf(USART3, "\r\n治电科技\r\n");		//打印
 
 
		DelayXms(100);
 
	while(1)
	{
		
		if(usart3Len > 0)
		{
			 
			if(strcmp(usart3Buf, "666") == 0){
				UsartPrintf(USART3, "输入的命令是：\r\n%s\r\n", usart3Buf);
				ledOn(LED1);
				
			}
			else if(strcmp(usart3Buf, "233") == 0){
				UsartPrintf(USART3, "输入的命令是：\r\n%s\r\n", usart3Buf);
				ledOff(LED1);
			}
			
			
			memset(usart3Buf, 0, sizeof(usart3Buf));
			usart3Len = 0;
		}
		
		DelayXms(100);
	
	}

}
