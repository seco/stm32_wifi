var net = require('net')
 

net.createServer(function(socket){
	socket.on('data',function(data){
		console.log('got:',data.toString());
 

	});
	socket.on('end',function(data){
		console.log('end');
	});
	socket.write('HIGH')
	 
}).listen(4040);

