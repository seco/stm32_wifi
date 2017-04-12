var express = require('express');
var Weibo = require('nodeweibo');
var setting = require('./setting.json');
var schedule = require('node-schedule');
var mysql = require('mysql');
var app = express();
Weibo.init(setting);
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

app.get('/',function(req,res){
	var user = req.originalUrl;
	var codes = user.split("=");
	console.log(codes[1]);
	res.send(codes[1])
})

conn.query('SELECT * FROM pet',function(err,rows,fields){
	var indoor;
	if (rows[rows.length-1].indoor == 1) {
		indoor = "on the home";
	}else{
		indoor = "not on the home";
	}

	var tem = "  temperature:"  + rows[rows.length-1].tem +"'C  ||  Pet:"+indoor 
	+"  ||  humidity:"+ rows[rows.length-1].hum + "%  ||  by ZZES_IOT";
	var para = {
		"access_token":"2.00IlvKmF05EX2B7c5e17d246vKldGE",
		"status":tem
	}
	Weibo.Statuses.update(para,function(data){
		console.log(data);
	})
})

var server = app.listen(3000,function(){
	console.log("listening for http://127.0.0.1:3000");
})