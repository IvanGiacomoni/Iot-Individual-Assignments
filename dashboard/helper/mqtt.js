require("dotenv").config();
const path = require("path");
var awsIot = require('aws-iot-device-sdk');


var device = awsIot.device({
   keyPath: path.resolve(__dirname, process.env.PRIVATE_KEY),
  certPath: path.resolve(__dirname, process.env.CERT),
    caPath: path.resolve(__dirname, process.env.ROOT_CA),
  clientId: process.env.CLIENT,
      host: process.env.HOST
});

function publishByMqtt(myTopic, messageObject){
	// Device is an instance returned by mqtt.Client()
	device
		.on('connect', function() {
		console.log('connect');
		device.subscribe(myTopic);
		device.publish(myTopic, JSON.stringify(messageObject));
	});
 
	device
		.on('message', function(topic, payload) {
		console.log('message', topic, payload.toString());
	});
}

exports.publishByMqtt = publishByMqtt;
