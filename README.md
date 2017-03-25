>纯新手，慢慢学。。
代码地址：https://github.com/klren0312/stm32_wifi

使用的器件：

 - STM32F103CBT6 单片机
 - ESP8266-01S wifi模块
 - SHT20温湿度

使用的Node.js服务端：

 - net模块
 - express
 - node-mysql


[搭建简易的物联网服务端-STM32（一）](http://www.jianshu.com/p/233ce211446f)
[搭建简易的物联网服务端-Nodejs_net（二）](http://www.jianshu.com/p/289683c96346)
[搭建简易的物联网服务端-Nodejs_mysql（三）](http://www.jianshu.com/p/3ec5b5ec53a5)
[搭建简易的物联网服务端-net+mysql（四）](http://www.jianshu.com/p/80eff97e39b6)
[搭建简易的物联网服务端-第一次融合（五）](http://www.jianshu.com/p/16799e256e0a)
[搭建简易的物联网服务端-Nodejs_express服务（六）](http://www.jianshu.com/p/462144651ee8)
[搭建简易的物联网服务端-ECharts数据显示（七）](http://www.jianshu.com/p/855d7fc25d10)
[搭建简易的物联网服务端-整合（八）](http://www.jianshu.com/p/e8ab20c70b5f)
[搭建简易的物联网服务端-Maibu显示（九）](http://www.jianshu.com/p/54af2d858908)


# 一、单片机相关代码(只提供网络相关代码)

## 1.esp8266相关配置代码

1）esp8266.h(宏定义一些AT指令)

```
\#define AT          "AT\r\n"	//测试esp8266是否工作正常
\#define CWMODE      "AT+CWMODE=3\r\n"		//设置wifi模块的模式，3为STA+AP模式
\#define RST         "AT+RST\r\n"        //重启
\#define CIFSR       "AT+CIFSR\r\n"      //查看ip
\#define CWJAP       "AT+CWJAP=\"nb\",\"123456789\"\r\n"	// 设置要连接的无线路由器的 ssid 和 password
\#define CIPSTART    "AT+CIPSTART=\"TCP\",\"119.29.201.31\",4001\r\n"		//设置服务器IP和端口
\#define CIPMODE0    "AT+CIPMODE=0\r\n"		//非透传模式
\#define CIPMODE1    "AT+CIPMODE=1\r\n"		//透传模式
\#define CIPSEND     "AT+CIPSEND\r\n"        //发送
\#define CIPSTATUS   "AT+CIPSTATUS\r\n"		//网络状态查询

extern void ESP8266_Rst(void);  
extern void ESP8266_Init(void);

```

2）esp8266.c(两个配置函数)

```
/**
  * @brief  ESP8266硬件复位
**/
void ESP8266_Rst(void)
{
	GPIO\_InitTypeDef GPIO\_InitStructure;
    //Pb5--对应ESP8266的reset引脚;
    RCC\_APB2PeriphClockCmd(RCC\_APB2Periph\_GPIOB, ENABLE);
    GPIO\_InitStructure.GPIO\_Pin = GPIO\_Pin\_5;
    GPIO\_InitStructure.GPIO\_Speed = GPIO\_Speed\_50MHz;
    GPIO\_InitStructure.GPIO\_Mode = GPIO\_Mode\_Out\_PP;
    GPIO\_Init(GPIOB, &GPIO\_InitStructure);
	GPIO\_ResetBits(GPIOB,GPIO\_Pin\_5);
	mDelay(100);
	GPIO\_SetBits(GPIOB,GPIO\_Pin\_5);
	mDelay(100);
}
/*
 *  @brief ESP8266模块初始化
 */
void ESP8266_Init(void)
{
	ESP8266_Rst();
\#if 1
    SendCmd(AT, "OK", 1000);		  //模块有效性检查
    SendCmd(CWMODE, "OK", 1000);	//模块工作模式
    SendCmd(RST, "OK", 2000);		  //模块重置
    SendCmd(CIFSR, "OK", 1000);		//查询网络信息
    SendCmd(CWJAP, "OK", 2000);		//配置需要连接的WIFI热点SSID和密码
    SendCmd(CIPSTART, "OK", 2000);	//TCP连接
    SendCmd(CIPMODE1, "OK", 1000);	//配置透传模式
	SendCmd(CIPSEND, ">", 1000);	//进入透传模式
	USART2_Clear();
\#endif
}
```

3)usart2.c中的SendCmd函数（USART2串口发送AT命令使用）
```
void SendCmd(char* cmd, char* result, int timeOut)
{
    while(1)
    {
        USART2_Clear();
        USART2_Write(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
        mDelay(timeOut);
        printf("%s %d cmd:%s,rsp:%s\n", \_\_func\_\_, \_\_LINE\_\_, cmd, usart2\_rcv\_buf);
        if((NULL != strstr((const char *)usart2\_rcv\_buf, result)))	//判断是否有预期的结果
        {
            break;
        }
        else
        {
            mDelay(100);
        }
    }
}
```

## 2.拼接发送字符串代码
1）获取温湿度(sht20代码就不放出来了)
```
        /* 获取温湿度 */
        SHT2x\_MeasureHM(SHT20\_Measurement\_T\_HM, &temp);
        mDelay(500);
        SHT2x\_MeasureHM(SHT20\_Measurement\_RH\_HM, &humi);
        /* 转化为字符串形式 */
        sprintf(tempStr, "%d", temp);
        sprintf(humiStr, "%d", humi);
```

2）拼接代码（拼接成json格式）

```
/**
  * @brief   拼接
  * @param   pkt   缓存指针
  *	@param 	 dsid1  数据流1ID
  *	@param 	 val1   字符串形式的数据点1的值
  *	@param 	 dsid2  数据流2ID
  *	@param 	 val2   字符串形式的数据点2的值
  * @retval  整个包的长度
  */
uint32\_t HTTP\_NewPostPacket(char *pkt, char *dsid1, char *val1, char *dsid2, char *val2)
{
    char dataBuf[50] = {0};
    char lenBuf[10] = {0};
    *pkt = 0;
    sprintf(dataBuf, "{\"%s\":\"%s\",\"%s\":\"%s\"}", dsid1, val1, dsid2, val2);
    sprintf(lenBuf, "%d", strlen(dataBuf));
    strcat(pkt, dataBuf);
    return strlen(pkt);
}

```

# 二、Node.js相关代码

## 1.TCP服务端代码
```
var net = require('net')
net.createServer(function(socket){
	socket.on('data',function(data){//接受处理
		console.log('got:',data.toString());//打印接收到的数据
		socket.write('I have received') //接受后回复
	});
	socket.write('Ready to receive your message!')//客户端连接成功后回复
}).listen(4001);
```

# 三、结果截图

## 1.服务端打印的信息截图

![服务端打印的信息截图](https://leanote.com/api/file/getImage?fileId=58cb384aab64417ac3007b02)

## 2.客户端串口输出的信息截图
![客户端串口输出的信息截图](https://leanote.com/api/file/getImage?fileId=58cb384aab64417ac3007b01)


>服务端慢慢完善中，一步一步记录学习
2017.3.17

# 四、Nodejs TCP 服务端和客户端
>用客户端模拟STM32发送过来的字符串。主要是为了解决对字符串的处理。


## 1.客户端代码

```
const net = require('net')
const client = net.connect({port:4001},() => {//向服务端发送
    console.log("connected to server!");
    client.write("{\"tem\":\"20\",\"hum\":\"80\"}");
});
client.on('data',(data) => {//从服务端接受
    console.log(data.toString());
    client.end();
});
client.on('end',() => {//断开连接
    console.log('disconnected from server');
})
```

发送的字符串是json格式：`"{"tem":"20","hum":"80"}"`

## 3.服务端代码

```
var net = require('net')
net.createServer(function(socket){
    socket.on('data',function(data){//接受服务端数据
        console.log('got:',data.toString());
        var text = JSON.parse(data.toString());//将接收到的字符串转换成json对象
        console.log(text);//获取json对象
        console.log(text.tem);//获取tem的值
    });
    socket.on('end',function(data){
        console.log('end');
    });
    socket.write('Ready to receive your message!')
     
}).listen(4001);

```


>缓慢完善中，一步一步记录学习
2017.3.19

# 五、Nodejs连接数据库，进行增删改查
>目前用来解决，获取STM32发来的数据，将其保存到MYSQL中，方便后面实时显示和统计。

## 1.安装node的mysql模块
>文档：https://www.npmjs.com/package/mysql

`npm install mysql --save`

## 2.数据库和表的创建

```
DROP TABLE IF EXISTS `env`;
CREATE TABLE `env` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tem` int(11) DEFAULT NULL,
  `hum` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
```

## 3.node_mysql模块的使用

```
var mysql = require('mysql');
var conn = mysql.createConnection({
    host:'localhost',
    user:'root',
    password:'root',
    database:'nodemysql',
    port:3306
});
conn.connect();
//查询
conn.query('SELECT * from env',function(err,rows,fields){
    if (err) throw err;
});
//增加
var post = {tem:25,hum:50}
conn.query('INSERT INTO env SET ?', post ,function(error,result,fields){
    if(error) throw error;
});
//更改
conn.query('UPDATE env SET tem=? , hum=? WHERE id = ?',[12,66,1], function(error,result,fields){
    if(error) throw error;
});
//删除
conn.query('DELETE FROM env WHERE tem=?',[12],function(error,result,fields){
    if (error) throw error;
    console.log('delete  '+ result.affectedRows + '  rows');
})
conn.end();
```

## 4.结果截图

![](https://leanote.com/api/file/getImage?fileId=58ce98faab64413217001e4d)


>缓慢完善中，今天任务很简单，将服务端接收到的数据存入mysql，就是前面的（二）和（三）融合
2017.3.20

# 六、TCP服务端接收数据并存入Mysql

## 1.服务端代码
```
var net = require('net')
var mysql = require('mysql');
var conn = mysql.createConnection({
    host:'localhost',
    user:'root',
    password:'root',
    database:'nodemysql',
    port:3306
});
conn.connect();
net.createServer(function(socket){
    socket.on('data',function(data){
        console.log('got:',data.toString());
        var text = JSON.parse(data.toString());
        console.log(text);
        console.log(text.tem);
        console.log(text.hum);
        //将温湿度数据存入mysql
        conn.query('INSERT INTO env SET ?', text, function(error,result,fields){
            if (error) throw error;
        });
    });
    socket.on('end',function(data){
        console.log('end');
    });
    socket.write('Ready to receive your message!')
}).listen(4001);
```

## 2.结果截图
1）客户端截图
![客户端](https://leanote.com/api/file/getImage?fileId=58cf824cab6441359b003026)

2）服务端截图
![](https://leanote.com/api/file/getImage?fileId=58cf824cab6441359b003028)

3）数据库截图
![](https://leanote.com/api/file/getImage?fileId=58cf824cab6441359b003027)



## 3.内网穿透工具ngrok
1）下载地址：https://ngrok.com/download
![](leanote://file/getImage?fileId=58cf843191ab7a40a1000006)

2）在ngrok官网注册账号获得自己的authtoken
![](leanote://file/getImage?fileId=58cf848291ab7a40a1000007)

3）将下载的压缩包解压，进入文件夹，打开命令行，运行`ngrok.exe authtoken 你的authtoken`回车即可

4）运行`ngrok http 80` 将内网80端口映射出去。
![](https://leanote.com/api/file/getImage?fileId=58cfdd06ab6441321700384d)


>通过单片机发送温湿度到服务端，服务端将数据处理后存入mysql数据库
2017.3.20

# 七、融合结果图
## 1.命令行
![QQ截图20170320222648.png](http://upload-images.jianshu.io/upload_images/2245742-3480e4f4826925c9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 2.数据库
![QQ截图20170320222703.png](http://upload-images.jianshu.io/upload_images/2245742-d1049e57ad39951f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


>昨天完成了将STM32采集到的温湿度数据传到服务端，存入Mysql数据库的操作。今天做的是通过express连接数据库，提供接口，让前端页面通过fetch获取数据。其中遇到了跨域问题，在express中解决了。
代码地址：https://github.com/klren0312/stm32_wifi
2017.3.21


# 八、Express服务端与前端获取

## 1.Express服务端代码
1)连接数据库（前面已经有介绍了）
```
var mysql = require('mysql');
//数据库配置
var conn = mysql.createConnection({
    host:'localhost',
    user:'root',
    password:'root',
    database:'nodemysql',
    port:3306
});
//连接数据库
conn.connect();
```

2)Express跨域解决
>解决了前端的`已拦截跨源请求：同源策略禁止读取位于 http://127.0.0.1:3000/mysql 的远程资源。（原因：CORS 头缺少 'Access-Control-Allow-Origin'）。`错误

```
//设置所有路由无限制访问，不需要跨域
app.all('*',function(req,res,next){
    res.header("Access-Control-Allow-Origin","*");
    res.header("Access-Control-Allow-Headers","X-Requested-With");
    res.header("Access-Control-Allow-Methods","PUT,POST,GET,DELETE,OPTIONS");
    res.header("X-Powered-By",'3.2.1');
    res.header("Content-Type","application/json;charset=utf-8");
    next();
})
```

3)路由配置（提供接口）
>两个接口，分别是温度和湿度数据。提供最新的五个数据

```
//温度
app.get('/tem',function(req,res){
    var tem = [];
    conn.query('SELECT * FROM env',function(err,rows,fields){
        var i  = rows.length;
        var i = rows.length;
        var j =i-1;
        var c= 0;
        while(j>=i-5){
            tem[c] = rows[j].tem;
            c++;
            j--;
        }
        res.send(JSON.stringify(tem));
    })
})
//湿度
app.get('/hum',function(req,res) {
    var hum = [];
    conn.query('SELECT * FROM env',function(err,rows,fields){
        var i = rows.length;
        var j =i-1;
        var c= 0;
        while(j>=i-5){
            hum[c] = rows[j].hum;
            c++;
            j--;
        }
        res.send(JSON.stringify(hum));
    })
     
});
```

4)express服务器配置
>设置端口为3000

```
//端口：3000
var server = app.listen(3000,function(){
    var host = server.address().address;
    var port = server.address().port;

    console.log(host + "  " + port);
})
```

## 2.前端代码
>使用fetch取代ajax（fetch与XMLHttpRequest(XHR)类似，fetch()方法允许你发出AJAX请求。区别在于Fetch API使用Promise，因此是一种简洁明了的API，比XMLHttpRequest更加简单易用。）

```
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>jsonptest</title>
</head>
<body>
    <script>
        function status(response){
            if(response.status>=200 && response.status<300){
                return Promise.resolve(response);
            }
            else{
                return Promise.reject(new Error(response.statusText));
            }
        }
        function json(response){
            return response.json();
        }
        //设置接口地址
        fetch("http://127.0.0.1:3000/tem")
        .then(status)
        .then(json)
        .then(function(data){
            console.log("请求成功，JSON解析后的响应数据为:",data);
        })
        .catch(function(err){
            console.log("Fetch错误:"+err);
        });
    </script>
</body>
</html>
```

## 3.结果截图
1)postman请求温度截图
  ![tem.png](http://upload-images.jianshu.io/upload_images/2245742-6c28960573cffd58.png?imageMogr2/auto-orient/strip%7CimageView2

2)postman请求湿度截图
 ![hum.png](http://upload-images.jianshu.io/upload_images/2245742-69628272f0f82952.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

3)前端fetch获取到的温度数据截图
 
![QQ截图20170321205801.png](http://upload-images.jianshu.io/upload_images/2245742-3a51f1e07a336a25.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


>继续缓慢学习。昨天前端获取到了数据，今天我们来通过ECharts来显示数据。
代码地址：https://github.com/klren0312/stm32_wifi
2017.3.22


# 九、ECharts数据显示

## 1.ECharts基本
1)介绍
百度推出的一款纯 Javascript 的图表库。
官方网址：http://echarts.baidu.com/index.html
<br>
2)基本使用
>官方网站有文档，我这里放出最基本结构(我也就会这么多。。。)


![基本结构.png](http://upload-images.jianshu.io/upload_images/2245742-d97edfaf190dce29.png)


<br>

## 2.EChart异步获取数据显示
>百度遇到坑，后来发现官方快速入门就有介绍。。。

![官方异步说明.png](http://upload-images.jianshu.io/upload_images/2245742-524f616e88a8f54f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


1)建立图表代码(body里面要建个id为main的div，记得设置长宽)
两个折线图放在一起，分别显示温度和湿度。
```
//初始化echarts
var myChart = echarts.init(document.getElementById('main'));
//指定图表的配置项和数据
var option ={
    title:{
        text:'温湿度'
    },
    tooltip:{},//提示框
    legend:{//图例组件
        data:['温度','湿度']
    },
    xAxis:{
        data:["1","2","3","4","最新"]
    },
    yAxis:{},
    series: [{
        name: '温度',
        type: 'line',
        data: []
    },{
        name:"湿度",
        type:'line',
        data:[]
    }]
};
//使用设置的配置项和数据显示图表
myChart.setOption(option);
```

3)fetch获取数据并加载到图表中
```
//fetch相关函数
function status(response){
    if(response.status>=200 && response.status<300){
        return Promise.resolve(response);
    }
    else{
        return Promise.reject(new Error(response.statusText));
    }
}
function json(response){
    return response.json();
}
获取温度数据
fetch("http://127.0.0.1:3000/tem")
    .then(status)
    .then(json)
    .then(function(data){
        //将数据加载到对应series name的表中
        myChart.setOption({
            series: [{
                // 根据名字对应到相应的系列
                name: '温度',
                data: data
            }]
        });
    })
    .catch(function(err){
        console.log("Fetch错误:"+err);
    });
//获取湿度数据
fetch("http://127.0.0.1:3000/hum")
    .then(status)
    .then(json)
    .then(function(data){
        //将数据加载到对应series name的表中
        myChart.setOption({
             series: [{
                 // 根据名字对应到相应的系列
                 name: '湿度',
                 data: data
             }]
        });
    })
    .catch(function(err){
        console.log("Fetch错误:"+err);
    });
```

## 3.结果截图

1)温湿度折线图截图

![温湿度图表.png](http://upload-images.jianshu.io/upload_images/2245742-f7a73f6a87d5c2d4.png)


2)温湿度柱状图截图

![温湿度柱状图表.png](http://upload-images.jianshu.io/upload_images/2245742-afbfaea92c3ce1a0.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



>今天主要完成的是表的局部刷新，然后就是整体展示。
代码地址：https://github.com/klren0312/stm32_wifi
2017.3.23



# 十、整合

## 1.EChart的局部刷新
1）setTimeout 和 setInterval
setTimeout 在指定的毫秒数后，将定时任务处理的函数添加到执行队列的队尾（只是延迟执行一次）
setInterval 按照指定的周期（毫秒数计时），将定时任务处理函数添加到执行队列的队尾（周期循环执行）

2）局部刷新
刚开始是准备进行全部页面进行刷新，来更新数据（原谅我的无知）
```
function freshWeb(){
    window.location.reload();
}
setInterval('freshWeb()',3000)
```
但是感到体验很差，于是我把fetch数据的相关放到一个函数中，然后用setInterval对fetch数据的函数循环执行，就可以达到ECharts数据更新的操作。
```
function getData(){
    fetch("http://127.0.0.1:3000/tem")
        .then(status)
        .then(json)
        .then(function(data){
            myChart.setOption({
                series: [{
                // 根据名字对应到相应的系列
                 name: '温度',
                 data: data
                }]
            });
        })
        .catch(function(err){
             console.log("Fetch错误:"+err);
        });
}
//循环执行fetch数据获取，达到ECharts数据更新的操作
setInterval('getData()',10);
```

## 2.整合实战结果图片
1）实时温湿度web显示
![实时温湿度web显示页面](http://upload-images.jianshu.io/upload_images/2245742-2dd78dbe3cc68072.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2）服务端数据存储

![服务端](http://upload-images.jianshu.io/upload_images/2245742-b77f9ff90a76ffb3.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


3）单片机照片


![单片机](http://upload-images.jianshu.io/upload_images/2245742-20c6508919fd9f07.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)




>刚好手边有块Maibu手表，所以，今天我们来让Maibu也来获取显示单片机的温湿度。
代码地址：https://github.com/klren0312/stm32_wifi
2017.3.24

# 十一、Maibu显示

## 1.服务端提供接口
>提供“/watch”接口

```
//手表推送
app.get('/watch',function(req,res){
    var tem = [];
    conn.query('SELECT * FROM env',function(err,rows,fields){
        var tem = "{ \"temhum\" :" + "\"" + rows[rows.length-1].tem + "  |  " 
        + rows[rows.length-1].hum +  "\""  + "}";
        res.send(tem);
    })
})
```

## 2.Maibu手表
1）官方网站
http://www.maibu.cc/

2）官方开发者中心
http://dev.maibu.cc/developer/index_login.do

3）开发者文档
http://maibu.cc/api/welcome.html

## 3.手表代码
>写的比较乱，这里只放出请求数据的代码。大家可以去我的github上看完整代码

1）数据请求函数
```
void data_request_web()
{
    /*拼接url请求地址, 注意url的缓存大小*/
    char url[200] = ""; 
    sprintf(url, "%s", DATA_WEB);       
    /*发送一次*/
    g_comm_id_web = maibu_comm_request_web(url, "temhum", 0);//过滤
}
```

2）界面初始化函数
```
P_Window init_btc_window()
{
    static P_Window p_window;
    P_Layer layer_background = NULL;   
    p_window = app_window_create();
    if (NULL == p_window)
    {
        return NULL;
    }
    /*加入你的代码 begin*/    
    /*创建背景图层*/
    layer_background = get_layer();
    /*添加背景图层到窗口*/
    app_window_add_layer(p_window, layer_background);
    /* 添加时间栏 */
    add_time_bar(p_window);
    /*添加数据提示信息*/
    GRect frame_tem = {{0, 30}, {16, 128}};
    add_text_layer(p_window, &g_layer_id_tem, "温度", &frame_tem, GAlignLeft, U_ASCII_ARIAL_16, GColorWhite);
    /*添加数据提示信息*/
    GRect frame_hum = {{0, 30}, {16, 128}};
    add_text_layer(p_window, &g_layer_id_hum, "湿度", &frame_hum, GAlignRight, U_ASCII_ARIAL_16, GColorWhite);
    /*添加数据提示信息*/
    GRect frame_zzes = {{0,110}, {16, 128}};
    add_text_layer(p_window, &g_layer_id_zzes, "治电科技", &frame_zzes, GAlignBottom, U_ASCII_ARIAL_16, GColorWhite);
    /*添加数据*/
    GRect frame_data = {{0,60}, {16, 128}};
    add_text_layer(p_window, &g_layer_id_data, "waiting", &frame_data, GAlignCenter , U_ASCII_ARIAL_42, GColorWhite);
    return p_window;
}
```

3）更新数据函数
```
static void web_recv_callback(const uint8_t *buff, uint16_t size)
{
    char stock_gid[10];
    char i;
    maibu_get_json_str(buff, "temhum", stock_gid, 10);//过滤
    for (i=0;i<10;i++)
    {
    if (stock_gid[i]=='}')
        {
            stock_gid[i]=0;
        }
    }
    /*添加数据*/
    GRect frame_data = {{0,60}, {25, 128}};
    add_text_layer(h_window, &g_layer_id_data, stock_gid, &frame_data, GAlignCenter , U_ASCII_ARIAL_20, GColorWhite);
    
    app_window_update(h_window);
}
```

## 4.结果展示
1）模拟器
![模拟器](http://upload-images.jianshu.io/upload_images/2245742-6b98ce11645b05e1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2）手表表盘

![maibu手表显示](http://upload-images.jianshu.io/upload_images/2245742-aca208371be9a8bb.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

