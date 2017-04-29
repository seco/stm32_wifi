var email = require("emailjs");
var mysql = require('mysql');
var schedule = require('node-schedule');
//数据库配置
var conn = mysql.createConnection({
	host:'localhost',
	user:'root',
	password:'root',
	database:'nodemysql',
	port:3306
})
//连接数据库
conn.connect();

//配置邮箱
var server = email.server.connect({
	user: "xxx@xxx.com",//用户名
	password:"xxx",//密码
	host:"smtp.xxx.com",//smtp邮箱服务器
	ssl:true
});
//每分钟第十秒的时候检测
var rule = new schedule.RecurrenceRule();
rule.second = 10;
var j = schedule.scheduleJob(rule,function(){
    conn.query('SELECT * FROM pet',function(err,rows,fields){
	 	var tem = rows[rows.length-1].tem;
	 	var temmsg = {
	 		text:"tempreture is " + tem + ",please  be careful",
	 		from: "xxxxxxx@xxxx.com",//发送方
	 		to:"xxx@xxx.com",//接收方
	 		subject:"PetHose tem"
	 	};	 		
	 	if(tem>=30){
	 		server.send(temmsg,function(err,message){
	 			console.log(err || "ok");
	 		});
	 	}
	});
});