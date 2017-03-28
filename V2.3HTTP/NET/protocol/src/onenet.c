/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-12-2
	*
	*	版本： 		V1.0
	*
	*	说明： 		与onenet平台的数据交互，协议层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "sht20.h"
#include "adxl345.h"
#include "gy30.h"
#include "fault.h"
#include "iwdg.h"
#include "hwtimer.h"
#include "selfcheck.h"
#include "oled.h"

//图片数据文件
#include "image_2k.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




ONETNET_INFO oneNetInfo = {"119.29.201.31", "4001", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];


/*
************************************************************
*	函数名称：	OneNet_toString
*
*	函数功能：	将数值转为字符串
*
*	入口参数：	dataStream：数据流
*				buf：转换后的缓存
*				pos：数据流中的哪个数据
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void OneNet_toString(DATA_STREAM *dataStream, char *buf, unsigned short pos)
{
	
	memset(buf, 0, 48);

	switch((unsigned char)dataStream[pos].dataType)
	{
		case TYPE_BOOL:
			snprintf(buf, 48, "%d", *(_Bool *)dataStream[pos].data);
		break;
		
		case TYPE_CHAR:
			snprintf(buf, 48, "%d", *(char *)dataStream[pos].data);
		break;
		
		case TYPE_UCHAR:
			snprintf(buf, 48, "%d", *(unsigned char *)dataStream[pos].data);
		break;
		
		case TYPE_SHORT:
			snprintf(buf, 48, "%d", *(short *)dataStream[pos].data);
		break;
		
		case TYPE_USHORT:
			snprintf(buf, 48, "%d", *(unsigned short *)dataStream[pos].data);
		break;
		
		case TYPE_INT:
			snprintf(buf, 48, "%d", *(int *)dataStream[pos].data);
		break;
		
		case TYPE_UINT:
			snprintf(buf, 48, "%d", *(unsigned int *)dataStream[pos].data);
		break;
		
		case TYPE_LONG:
			snprintf(buf, 48, "%ld", *(long *)dataStream[pos].data);
		break;
		
		case TYPE_ULONG:
			snprintf(buf, 48, "%ld", *(unsigned long *)dataStream[pos].data);
		break;
			
		case TYPE_FLOAT:
			snprintf(buf, 48, "%f", *(float *)dataStream[pos].data);
		break;
		
		case TYPE_DOUBLE:
			snprintf(buf, 48, "%f", *(double *)dataStream[pos].data);
		break;
		
		case TYPE_GPS:
			snprintf(buf, 48, "{\"lon\":%s,\"lat\":%s}", (char *)dataStream[pos].data, (char *)dataStream[pos].data + 16);
		break;
	}

}

/*
************************************************************
*	函数名称：	OneNet_Load_DataStream
*
*	函数功能：	数据流封装
*
*	入口参数：	type：发送数据的格式
*				send_buf：发送缓存指针
*				len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		封装数据流格式
************************************************************
*/
void OneNet_Load_DataStream(unsigned char type, char *send_buf, unsigned char len)
{
	
	unsigned char count = 0;
	char stream_buf[96];
	char data_buf[48];
	char *ptr = send_buf;

	switch(type)
	{
		case HTTP_TYPE1:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE1\r\n");
		
			strncpy(send_buf, "{\"datastreams\":[", strlen("{\"datastreams\":["));
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));

					OneNet_toString(dataStream, data_buf, count);
					snprintf(stream_buf, sizeof(stream_buf), "{\"id\":\"%s\",\"datapoints\":[{\"value\":%s}]},", dataStream[count].name, data_buf);
					
					strncat(send_buf, stream_buf, strlen(stream_buf));
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strncat(send_buf, "]}", 2);
		
		break;
			
		case HTTP_TYPE3:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE3\r\n");
		
			strncpy(send_buf, "{", 1);
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));

					OneNet_toString(dataStream, data_buf, count);
					snprintf(stream_buf, sizeof(stream_buf), "\"%s\":%s,", dataStream[count].name, data_buf);

					strncat(send_buf, stream_buf, strlen(stream_buf));
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strncat(send_buf, "}", 1);
		
		break;
		
		case HTTP_TYPE4:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE4\r\n");
		
			strncpy(send_buf, "{", 1);
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					
					OneNet_toString(dataStream, data_buf, count);
					snprintf(stream_buf, sizeof(stream_buf), "\"%s\":{\"2016-08-10T12:31:17\":%s},", dataStream[count].name, data_buf);
					
					strncat(send_buf, stream_buf, strlen(stream_buf));
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strncat(send_buf, "}", 1);
		
		break;
		
		case HTTP_TYPE5:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE5\r\n");
		
			strncpy(send_buf, ",;", 2);
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					
					OneNet_toString(dataStream, data_buf, count);
					snprintf(stream_buf, sizeof(stream_buf), "%s,%s;", dataStream[count].name, data_buf);
				}
				
				strncat(send_buf, stream_buf, strlen(stream_buf));
			}
		
		break;
	}

}

/*
************************************************************
*	函数名称：	OneNet_SendData
*
*	函数功能：	上传数据到平台
*
*	入口参数：	type：发送数据的格式
*
*	返回参数：	无
*
*	说明：		这里需要更改为自己想要上传的数据，依照格式填写
************************************************************
*/
_Bool OneNet_SendData(HTTP_TYPE  type, unsigned char len)
{
	
	char sendBuf[512];
	char streams[416];
//	unsigned short strLen = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);
	
	memset(sendBuf, 0, sizeof(sendBuf));
	memset(streams, 0, sizeof(streams));
	
	OneNet_Load_DataStream(type, streams, len);
	
	// strLen = strlen(streams);
	
	//这个不改
//	snprintf(sendBuf, sizeof(sendBuf), "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
//					"Content-Length:%d\r\n\r\n",
//	
//					oneNetInfo.devID, type, oneNetInfo.apiKey, strLen);
	
	strncat(sendBuf, streams, strlen(streams));
	
	NET_DEVICE_SendData((unsigned char *)sendBuf, strlen(sendBuf));
	
	faultTypeReport = FAULT_NONE; //发送之后清除标记
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	return 0;
	
}

/*
************************************************************
*	函数名称：	OneNet_Status
*
*	函数功能：	连接状态检查
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void OneNet_Status(void)
{
	
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)	//如果网络为连接 或 不为数据收发模式
		return;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);									//设置为心跳收发模式
	
	errType = NET_DEVICE_Check();
	
	if(errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
	{
		faultTypeReport = faultType = FAULT_EDP;								//标记为协议错误
		
		oneNetInfo.errCount++;
	}
	else if(errType == CHECK_NO_DEVICE)
	{
		faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
		
		oneNetInfo.errCount++;
	}
	else
	{
		faultTypeReport = faultType = FAULT_NONE;								//无错误
	}
	
	NET_DEVICE_ClrData();														//情况缓存
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);									//设置为数据收发模式
	
	

}

/*
************************************************************
*	函数名称：	OneNet_Event
*
*	函数功能：	平台返回数据检测
*
*	入口参数：	dataPtr：平台返回的数据
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void OneNet_Event(unsigned char *dataPtr)
{

	if(strstr((char *)dataPtr, "CLOSED"))
	{
		UsartPrintf(USART_DEBUG, "TCP CLOSED1\r\n");
		
		faultTypeReport = faultType = FAULT_EDP;						//标记为协议错误
		
		oneNetInfo.errCount++;
	}
	else
	{
		//这里用来检测是否发送成功
		if(strstr((char *)dataPtr, "succ"))
		{
			UsartPrintf(USART_DEBUG, "Tips:		Send OK\r\n");
			oneNetInfo.errCount = 0;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:		Send Err\r\n");
			oneNetInfo.errCount++;
		}
	}
	
	NET_DEVICE_ClrData();

}
