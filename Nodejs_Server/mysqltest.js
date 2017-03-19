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