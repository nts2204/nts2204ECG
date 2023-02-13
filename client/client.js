const io = require('socket.io-client');
const socket = io.connect('http://192.168.1.10:3000');

socket.on('connect', () => {
  console.log('Connected to the server!');
});

socket.on('disconnect', () => {
  console.log('Disconnected from the server');
});

socket.on('broadcast', (data) => {
  console.log(data);
});

socket.on('from_server', (data) => {
  console.log(`Received message: ${data}`);
});

// Emit a message to the server
// socket.emit('status', 'Hello, server!');

// send a message to the server each a second
setInterval(() => {
  // get a random number between 0 and 100
  const number = Math.floor(Math.random() * 100);
  console.log(`Emitting number: ${number}`);
  socket.emit('status', number);
}
, 100);
