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
// conn.query('SELECT * from pet',function(err,rows,fields){
// 	if (err) throw err;
// 	var i = rows.length;
// 	var j =i-1;
// 	console.log(i);
// 	while(j>=i-5){
// 		console.log(rows[j].hum);
// 		j--;
// 	}
// 	console.log(rows[0].tem);
// 	console.log(rows[1].tem);
// 	console.log(rows[2]);
// });


conn.query('DELETE FROM pet WHERE id NOT IN ( SELECT id FROM ( SELECT id FROM pet ORDER BY id DESC LIMIT 5 ) foo )',function(err,rows,fields){
    if (err) throw err;      
});

// //增加
// var post = {tem:25,hum:50}
// conn.query('INSERT INTO env SET ?', post ,function(error,result,fields){
// 	if(error) throw error;
// });

// //更改
// conn.query('UPDATE env SET tem=? , hum=? WHERE id = ?',[12,66,1], function(error,result,fields){
// 	if(error) throw error;
// });

// //删除
// conn.query('DELETE FROM pet LIMIT ?',[3],function(error,result,fields){
// 	if (error) throw error;
// 	console.log('delete  '+ result.affectedRows + '  rows');
// })

 
conn.end();