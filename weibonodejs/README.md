>想通过微博来定时发送相关数据（由于没认证，所以接口调用一下就到了限制，明天继续。。。）
2017.4.10



# 微博通知
## 1.weibo开发者平台
（1）网址
http://open.weibo.com/
（2）创建应用

![QQ截图20170409215555.png](http://upload-images.jianshu.io/upload_images/2245742-5de9c5de4487a8ea.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



（3）获取App Key 和 App Secret


![QQ截图20170409215631.png](http://upload-images.jianshu.io/upload_images/2245742-ab968a917330c07f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)




（4）了解OAuth2认证流程


![oAuth2_01.gif](http://upload-images.jianshu.io/upload_images/2245742-df64b665d780be8b.gif?imageMogr2/auto-orient/strip)





## 2.Nodejs的nodeweibo模块
（1）github网址
https://github.com/vczero/node-weibo


![pak.png](http://upload-images.jianshu.io/upload_images/2245742-441b42c10f54625f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)




（2）安装
`npm install nodeweibo`


## 3.nodeweibo模块使用
（1）编写setting.json配置文件

![QQ截图20170409220201.png](http://upload-images.jianshu.io/upload_images/2245742-f4c6879a32c6a5af.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)




（2）开启微博认证 ，获取 OAuth2需要的code。会弹出授权页面，同意后会在跳转后的页面url上看到code。
```
var Weibo = require('nodeweibo');
var setting = require('./setting.json');
Weibo.init(setting);
Weibo.authorize();
```

![QQ截图20170409220330.png](http://upload-images.jianshu.io/upload_images/2245742-ea2e12e150f8fe14.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)




（3）接着是通过code获取access_token，注意下面的程序和上面是独立的。access_token每次都是一样的，所以当授权到期，只要进行上面的开启微博认证即可。
```
var Weibo = require('nodeweibo');
var setting = require('./setting.json');
Weibo.init(setting);
var paras={
    code:"上面程序获得到的code",
    grant_type:"authorization_code"
};
Weibo.OAuth2.access_token(paras,function(data){
    console.log(data);//命令行中会打印出access_token
})
```

![QQ截图20170409220811.png](http://upload-images.jianshu.io/upload_images/2245742-9d76d0d2b8e6b66a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)





（4）接下来调用`statuses/update`接口，来发送微博信息
```
var Weibo = require('nodeweibo');
var setting = require('./setting.json');
Weibo.init(setting);
// access_token应该是不变的，status里面试要发送的内容
var para = {
    "access_token":"上面获得到的access_token",
    "status":"微博发送的信息"
}
Weibo.Statuses.update(para,function(data){
    console.log(data);
})
```

![QQ截图20170409221049.png](http://upload-images.jianshu.io/upload_images/2245742-5a6dcfad2e3b812c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)





## 4.nodejs定时发送模块
（1）github网址
https://github.com/node-schedule/node-schedule

（2）安装
`npm install node-schedule`

（3）时间格式

![QQ截图20170409221743.png](http://upload-images.jianshu.io/upload_images/2245742-62452c05b5839dbb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)





## 5.node-schedule模块使用
（1）每分钟的第一秒进行操作
```
var rule = new schedule.RecurrenceRule();
rule.second = 2;
var j = schedule.scheduleJob(rule,function(){
    var para = {
        "access_token":"",
        "status":"测试"
    }
    Weibo.Statuses.update(para,function(data){
        console.log(data);
    })  
});
```

@治电小白菜20170410