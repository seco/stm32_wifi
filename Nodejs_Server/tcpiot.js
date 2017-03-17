var net = require('net')
 
net.createServer(function(socket){
	socket.on('data',function(data){
		console.log('got:',data.toString());

		var text = JSON.parse(data.toString());
		console.log(text);
		console.log(text.tem)
	});
	socket.on('end',function(data){
		console.log('end');
	});
	socket.write('Ready to receive your message!')
	 
}).listen(4001);

