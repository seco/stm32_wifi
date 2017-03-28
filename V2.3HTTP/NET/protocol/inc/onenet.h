#ifndef _ONENET_H_
#define _ONENET_H_







typedef struct
{

//    char devID[15];
//    char apiKey[35];
	
	char ip[16];
	char port[8];
	
	unsigned char netWork : 1; //0-局网模式(AP)		1-互联网模式(STA)
	unsigned char sendData : 1;
	unsigned char errCount : 3; //错误计数
	unsigned char reverse : 3;

} ONETNET_INFO;

extern ONETNET_INFO oneNetInfo;


typedef enum
{

	TYPE_BOOL = 0,
	
	TYPE_CHAR,
	TYPE_UCHAR,
	
	TYPE_SHORT,
	TYPE_USHORT,
	
	TYPE_INT,
	TYPE_UINT,
	
	TYPE_LONG,
	TYPE_ULONG,
	
	TYPE_FLOAT,
	TYPE_DOUBLE,
	
	TYPE_GPS,

} DATA_TYPE;

typedef struct
{

	char *name;
	void *data;
	DATA_TYPE dataType;
	_Bool flag;

} DATA_STREAM;

#define SEND_BUF_SIZE  1024


typedef enum
{

    HTTP_TYPE1 = 1,
	HTTP_TYPE3 = 3,
	HTTP_TYPE4,
	HTTP_TYPE5

} HTTP_TYPE;


#define CHECK_CONNECTED			0	//已连接
#define CHECK_CLOSED			1	//已断开
#define CHECK_GOT_IP			2	//已获取到IP
#define CHECK_NO_DEVICE			3	//无设备
#define CHECK_INITIAL			4	//初始化状态
#define CHECK_NO_CARD			5	//没有sim卡
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 //AT命令模式
#define DEVICE_DATA_MODE		1 //平台命令下发模式
#define DEVICE_HEART_MODE		2 //心跳连接模式





_Bool OneNet_SendData(HTTP_TYPE  type, unsigned char len);

void OneNet_Status(void);

void OneNet_Event(unsigned char *dataPtr);


#endif
