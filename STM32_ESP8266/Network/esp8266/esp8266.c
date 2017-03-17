#include "esp8266.h"
#include "usart2.h"
#include "utils.h"
/**
  * @brief  ESP8266硬件复位
**/
void ESP8266_Rst(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    //Pb5--对应ESP8266的reset引脚;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		mDelay(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		mDelay(100);
}


/*
 *  @brief ESP8266模块初始化
 */
void ESP8266_Init(void)
{
		ESP8266_Rst();
#if 1
    SendCmd(AT, "OK", 1000);		  //模块有效性检查
    SendCmd(CWMODE, "OK", 1000);	//模块工作模式
    SendCmd(RST, "OK", 2000);		  //模块重置
    SendCmd(CIFSR, "OK", 1000);		//查询网络信息
    SendCmd(CWJAP, "OK", 2000);		//配置需要连接的WIFI热点SSID和密码
    SendCmd(CIPSTART, "OK", 2000);	//TCP连接
    SendCmd(CIPMODE1, "OK", 1000);	//配置透传模式
		SendCmd(CIPSEND, ">", 1000);	//进入透传模式
		USART2_Clear();
#endif
}
