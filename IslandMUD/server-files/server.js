var app = require('http').createServer(handler),
  io = require('/home/pi/node_modules/socket.io').listen(app),
  parser = new require('/home/pi/node_modules/xml2json'),
  fs = require('fs');

app.listen(8000);
console.log('server listening on localhost:8000');

// load webpage
function handler(req, res) {
  fs.readFile(__dirname + '/client.html', function(err, data) {
    if (err) {
      console.log(err);
      res.writeHead(500);
      return res.end('error loading webpage');
    }
    res.writeHead(200);
    res.end(data);
  });
}

// create a websocket
io.sockets.on('connection', function(socket) {
  // add a watcher to the XML file
  fs.watchFile(__dirname + '/example.xml', function(curr, prev) {
    // when the file is updated
    fs.readFile(__dirname + '/example.xml', function(err, data) {
      if (err) throw err;
      // convert XML to JSON
      var json = parser.toJson(data);
	  // send
      socket.volatile.emit('notification', json);
    });
  });  
});
