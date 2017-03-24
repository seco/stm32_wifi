var express = require('express');
var mysql = require('mysql');
app = express();

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

//设置所有路由无限制访问，不需要跨域
app.all('*',function(req,res,next){
 	res.header("Access-Control-Allow-Origin","*");
 	res.header("Access-Control-Allow-Headers","X-Requested-With");
 	res.header("Access-Control-Allow-Methods","PUT,POST,GET,DELETE,OPTIONS");
 	res.header("X-Powered-By",'3.2.1');
 	res.header("Content-Type","application/json;charset=utf-8");
 	next();
})
//温度
app.get('/watch',function(req,res){
	var tem = [];
	conn.query('SELECT * FROM env',function(err,rows,fields){
		var tem = "{ \"temhum\" :" + "\"" + rows[rows.length-1].tem + "  |  " 
		+ rows[rows.length-1].hum +  "\""  + "}";
		res.send(tem);
	})
})
//湿度
app.get('/hum',function(req,res) {
	var hum = [];
	conn.query('SELECT * FROM env',function(err,rows,fields){
		var i = rows.length;
		var j =i-5;
		var c= 0;
		while(j<i){
			hum[c] = rows[j].hum;
			c++;
			j++;
		}
		res.send(JSON.stringify(hum));
	})
	 
});
//端口：3000
var server = app.listen(3000,function(){
	var host = server.address().address;
	var port = server.address().port;

	console.log(host + "  " + port);
})
