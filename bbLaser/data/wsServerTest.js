//create a websocket server and listen on port 8080
var WebSocketServer = require('ws').Server;
var wss = new WebSocketServer({port: 8080});
const fs = require('fs');

//when a client connects send hello,when a client sends a data, write it to the console and write to a file call test.ild
wss.on('connection', function(ws) {
    ws.on('message', function(message) {
        console.log('received: %s', message);
        fs.writeFileSync('test.ild', message);
    });
    ws.send('hello');
    });
