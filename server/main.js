// const io = require('socket.io')(3000);
var express = require('express');
var app = express();
var server = require('http').Server(app);
var io = require('socket.io')(server);

/*
const io = require('socket.io')(server, {
  cors: {
      origin: "*:*",
      methods: ["GET", "POST"],
      //transports: ['websocket', 'polling'],
      credentials: true
  },
  allowEIO3: true
});
*/

app.get('/', function(req, res){
  // respone html from public folder
  res.sendFile(__dirname + '/public/index.html');
});


app.use(express.static('public'));

io.on('connection', socket => {
  console.log("connected " + socket.id);
  socket.on('status', (data) => {
    console.log(data);
    socket.broadcast.emit('broadcast', data);
  });

});

server.listen(5000, function(){
	console.log('Server running on port 5000.');
	//console.log(consulta("Josue","mcjosue"));
	//if(consulta() == "Josue"){console.log("correcto")};
});