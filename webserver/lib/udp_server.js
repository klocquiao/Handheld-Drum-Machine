"use strict";
/*
 * Respond to commands over a websocket to relay UDP commands to a local program
 */

var socketio = require('socket.io');
var io;
var dgram = require('dgram');
var timer = null;

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');

	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});

};

function handleCommand(socket) {
	// Passed string of comamnd to relay
	// Info for connecting to the local process via UDP

	socket.on('drum', function(data) {
		console.log('drum command: ' + data);
		var PORT = 12345;
		var HOST = '127.0.0.1';
		var client = dgram.createSocket('udp4');	

		var buffer = new Buffer(data);
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) {
				throw err;
			}
			console.log('UDP message sent to ' + HOST +':'+ PORT);
			if (data == "uptime") {
				initializeTimeout(socket);
			}
		});

		client.on('listening', function () {
			var address = client.address();
			console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			clearTimeout(timer);
			console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
			var reply = message.toString('utf8')
			var replySplit = reply.split(":");
			
			if (replySplit.length == 2) {
				emitDrumInformation(socket, replySplit);
			}
			else {
				emitDeviceInformation(socket, reply);
			}
			client.close();
		});

		client.on("UDP Client: close", function() {
			console.log("closed");
		});

		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
	});

};

function initializeTimeout (socket) {
	timer = setTimeout(function () {
		socket.emit("error", "No longer connected to the target");
	  }, 1000);
}	

function emitDrumInformation(socket, reply) {
	var parameters = {
		id : reply.at(1),
		value : reply.at(0)
	}
	if (parameters.id == "beat") {
		socket.emit('commandBeat', parameters);
	}
	else if (parameters.id == "volume") {
		socket.emit('commandVolume', parameters);
	}
	else if (parameters.id == 'tempo') {
		socket.emit('commandTempo', parameters);
	}
}

function emitDeviceInformation(socket, reply) {
	socket.emit("fileContents", reply);
}
