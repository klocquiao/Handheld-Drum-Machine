"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {
	// Setup a repeating function (every 1s)
	window.setInterval(function() {sendDrumCommand('uptime')}, 1000);
	
	window.setInterval(function() {sendDrumCommand('beat')}, 1000);
	window.setInterval(function() {sendDrumCommand('volume')}, 1000);
	window.setInterval(function() {sendDrumCommand('tempo')}, 1000);

	//BEAT
	$('#modeNone').click(function(){
		sendDrumCommand("none");
	});
	$('#modeRock').click(function(){
		sendDrumCommand("rock");
	});
	$('#modeAlternative').click(function(){
		sendDrumCommand("alternative");
	});

	//SOUND
	$('#snare').click(function(){
		sendDrumCommand("snare");
	});
	$('#bassDrum').click(function(){
		sendDrumCommand("bass");
	});
	$('#hiHat').click(function(){
		sendDrumCommand("hihat");
	});

	//VOLUME
	$('#volumeUp').click(function(){
		sendDrumCommand("vup");
	});
	$('#volumeDown').click(function(){
		sendDrumCommand("vdown");
	});

	//TEMPO
	$('#tempoUp').click(function(){
		sendDrumCommand("tup");
	});
	$('#tempoDown').click(function(){
		sendDrumCommand("tdown");
	});


	socket.on('commandBeat', function(result) {
		$('#modeid').text(result.value);
	});

	socket.on('commandVolume', function(result) {
		$('#volumeid').attr("value", result.value);
	});

	socket.on('commandTempo', function(result) {
		$('#tempoid').attr("value", result.value);
	});

	socket.on('fileContents', function(result) {
		$('#status').text("Device up for: " + result);
	});

	socket.on('error', function(result) {
		$('#error-text').text(result);
		$('#error-box').show();

		var timer = setTimeout(function() {
			$('#error-box').hide();
			clearTimeout(timer);
		}, 10000);
	});
});

function sendDrumCommand(message) {
	socket.emit('drum', message);
};
