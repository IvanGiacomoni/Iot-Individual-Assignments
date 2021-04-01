const express = require("express");
const router = express.Router();
const axios = require("axios");

require("dotenv").config();

const path = require("path");
var awsIot = require('aws-iot-device-sdk');

router.post("/switchMode", async (req, res) => {
	console.log("QUI\n");
	
	const device_id = req.body.device_id;
	const topic_type = req.body.topic_type;  // gas_smoke, temperature
	const mode = req.body.mode;
	
	const myTopic = "switchMode/device/"+device_id+"/"+topic_type;
	
	console.log("topic: "+myTopic+"\n");
	
	
		var device = awsIot.device({
			keyPath: path.resolve(__dirname, process.env.PRIVATE_KEY),
			certPath: path.resolve(__dirname, process.env.CERT),
			caPath: path.resolve(__dirname, process.env.ROOT_CA),
			clientId: process.env.CLIENT,
			host: process.env.HOST
		});	
	
		// Device is an instance returned by mqtt.Client()
		device
		.on('connect', function() {
			console.log('connect');
			device.subscribe(myTopic);
			device.publish(myTopic, JSON.stringify({mode: "manual"}));
		});
		
		device
		.on('message', function(topic, payload) {
			console.log('message', topic, payload.toString());
		})
	
	res.status(200).send("done!");
});

module.exports = router;
