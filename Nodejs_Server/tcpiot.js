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
		console.log(text.tem)
		console.log(text.hum)
		conn.query('INSERT INTO env SET ?', text, function(error,result,fields){
			if (error) throw error;
		});

	});
	socket.on('end',function(data){
		console.log('end');
	});
	socket.write('Ready to receive your message!')
	 
}).listen(4001);

