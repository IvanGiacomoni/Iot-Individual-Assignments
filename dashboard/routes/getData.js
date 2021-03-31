const express = require("express");
const router = express.Router();
const axios = require("axios");

require("dotenv").config();

const {quickSort} = require("../helper/sortData");

var AWS = require("aws-sdk");
let awsConfig = {
    "region": "us-east-1",
    "endpoint": "http://dynamodb.us-east-1.amazonaws.com",
    "accessKeyId": process.env.AWS_ACCESS_KEY_ID,
     "secretAccessKey": process.env.AWS_SECRET_ACCESS_KEY,
     "sessionToken": process.env.AWS_SESSION_TOKEN
};
AWS.config.update(awsConfig);

var docClient = new AWS.DynamoDB.DocumentClient();

var paramsTemp = {
    TableName: "temp",
};

var paramsGasSmoke = {
    TableName: "gas_smoke",
};

var countTemp = 0;
var countGasSmoke = 0;

router.get("/getTemperatureData", async (req, res) => {
  timestamps = [];
  
  docClient.scan(paramsTemp, (err, data) => {
	  if (err) {
        console.error("Unable to scan temperature table. Error JSON:", JSON.stringify(err, null, 2));
        return res.status(404).send("Unable to scan the table");
    } else {
        
        for(let i=0;i<data.Items.length;i++){
			timestamps.push(data.Items[i].sample_time);
			data.Items[i].sample_time = new Date(data.Items[i].sample_time);
			//console.log("Item :", ++countTemp,JSON.stringify(data.Items[i]));
		}
		
		const sortedTimestamps = quickSort(timestamps, 0, timestamps.length - 1);
		let toReturn = [];

		sortedTimestamps.forEach(t => {
			for(let i=0;i<data.Items.length;i++){
				if(t === (+ data.Items[i].sample_time)){
					toReturn.push(data.Items[i]);
					break;
				}
			}
		});

        // continue scanning if we have more items
        /*if (typeof data.LastEvaluatedKey != "undefined") {
            console.log("Scanning for more...");
            params.ExclusiveStartKey = data.LastEvaluatedKey;
            docClient.scan(paramsTemp, onScan);
        }*/
        return res.status(200).send(toReturn);
    }
  });
});

router.get("/getGasSmokeData", async (req, res) => {
  timestamps = [];
  
  docClient.scan(paramsGasSmoke, (err, data) => {
	  if (err) {
        console.error("Unable to scan gas-smoke table. Error JSON:", JSON.stringify(err, null, 2));
        return res.status(404).send("Unable to scan the table");
    } else {     
        console.log("Scan succeeded.");
        
        for(let i=0;i<data.Items.length;i++){
			timestamps.push(data.Items[i].sample_time);
			data.Items[i].sample_time = new Date(data.Items[i].sample_time);
			//console.log("Item :", ++countTemp,JSON.stringify(data.Items[i]));
		}
		
		const sortedTimestamps = quickSort(timestamps, 0, timestamps.length - 1);
		let toReturn = [];
		
		sortedTimestamps.forEach(t => {
			for(let i=0;i<data.Items.length;i++){
				if(t === (+ data.Items[i].sample_time)){
					toReturn.push(data.Items[i]);
					break;
				}
			}
		});
		
        // continue scanning if we have more items
        /*if (typeof data.LastEvaluatedKey != "undefined") {
            console.log("Scanning for more...");
            params.ExclusiveStartKey = data.LastEvaluatedKey;
            docClient.scan(paramsGasSmoke, onScan);
        }*/
        
        return res.status(200).send(toReturn);
    }
  });
});

router.get("/getDevices/:typeData", async (req, res) => {
	let options = {
		method: 'get',
		url: ''
    };
	
	if(req.params.typeData == "temperature")
		options.url = 'http://localhost:8080/getTemperatureData';
	else if(req.params.typeData == "gasSmoke")
		options.url = 'http://localhost:8080/getGasSmokeData';
	
    
    axios(options)
    .then(response => {
		
		const device_id = req.params.device_id;
		let toReturn = [];
		
		response.data.forEach(elem => {
			if(!toReturn.includes(elem.device_id)){
				toReturn.push(elem.device_id);
			}
		});
		
		return res.status(200).send(toReturn);
	})
	.catch(err => {
		console.log(err);
	});
	
	
});

router.get("/getLatestData/:typeData", async (req, res) => {
  
    let options = {
		method: 'get',
		url: ''
    };
    
    if(req.params.typeData == "temperature")
		options.url = 'http://localhost:8080/getTemperatureData';
	else if(req.params.typeData == "gasSmoke")
		options.url = 'http://localhost:8080/getGasSmokeData';
		
	axios(options)
	.then(response => {
		let toReturn = [];
		
		let devices = [];
		
		response.data.forEach(elem => {
			if(!devices.includes(elem.device_id)){
				devices.push(elem.device_id);
			}
		})
		console.log(devices);
		
		devices.forEach(elem => {
			console.log("\n"+"device "+elem);
			const data = response.data;
			latest = [];
			
			const now = new Date(Date.now());
			
			data.forEach(entry => {
				if(entry.device_id == elem){
					
					const sample_time = Date.parse(entry.sample_time);
					var timeStart = sample_time;
					var timeEnd = now.getTime();
					var hourDiff = timeEnd - timeStart; //in ms
					var secDiff = hourDiff / 1000; //in s
					var minDiff = hourDiff / 60 / 1000; //in minutes
					var hDiff = hourDiff / 3600 / 1000; //in hours
					var humanReadable = {};
					humanReadable.hours = Math.floor(hDiff);
					humanReadable.minutes = minDiff - 60 * humanReadable.hours;
					
					//console.log(now, entry.sample_time, humanReadable);
					
					if(humanReadable.hours == 0 && humanReadable.minutes < 60){
						latest.push(entry);
					}
				}
			});
			
			toReturn.push({
				"device_id": elem,
				latest: latest
			});
		});
		
		return res.status(200).send(toReturn);
	})
	
	.catch(err => {
		console.log(err);
	});
});

router.get("/getTemperatureAggregatedValues", async (req, res) => {
	
	let options = {
		method: 'get',
		url: 'http://localhost:8080/getLatestData/temperature'
    };
    
    axios(options)
    .then(response => {
		
		const temperatureData = response.data;
		
		console.log(temperatureData);
		
		let toReturn = [];
		
		temperatureData.forEach(device => {
			
			if(device.latest.length == 0) toReturn.push({
				"device_id": device.device_id,
				"minTemp": 0,
				"maxTemp": 0,
				"avgTemp": 0.0
			});
			
			else{
				let maxTemp = device.latest[0].device_data.temperature;
				let minTemp = device.latest[0].device_data.temperature;
					
				let sumTempValues = 0;
			
				device.latest.forEach(elem => {
				
					if (elem.device_data.temperature > maxTemp)
						maxTemp = elem.device_data.temperature;
				
					if (elem.device_data.temperature < minTemp)
						minTemp = elem.device_data.temperature;			
				
					sumTempValues += elem.device_data.temperature;
				});
				
				const tempAvgValue = (sumTempValues / device.latest.length).toFixed(2);
				
				toReturn.push({
					"device_id": device.device_id,
					"minTemp": minTemp,
					"maxTemp": maxTemp,
					"avgTemp": tempAvgValue
				});
			}
			
		});
		
		res.status(200).send(toReturn);
		
	})
	.catch(error => {
		console.log(error);
	});
});

router.get("/getGasSmokeAggregatedValues", async (req, res) => {
	
	let options = {
		method: 'get',
		url: 'http://localhost:8080/getLatestData/gasSmoke'
    };
    
    axios(options)
    .then(response => {
		
		const gasSmokeData = response.data;
		
		console.log(gasSmokeData);
		
		let toReturn = [];
		
		gasSmokeData.forEach(device => {
			
			if(device.latest.length == 0) toReturn.push({
				device_id: device.device_id,
				"minPpm": 0,
				"maxPpm": 0,
				"avgPpm": 0.0
			});
			
			else{
				let maxPpm = device.latest[0].device_data.ppm;
				let minPpm = device.latest[0].device_data.ppm;
					
				let sumPpmValues = 0;
			
				device.latest.forEach(elem => {
				
					if (elem.device_data.ppm > maxPpm)
						maxPpm = elem.device_data.ppm;
				
					if (elem.device_data.ppm < minPpm)
						minPpm = elem.device_data.ppm;			
				
					sumPpmValues += elem.device_data.ppm;
				});
				
				const ppmAvgValue = (sumPpmValues / device.latest.length).toFixed(2);
				
				toReturn.push({
					device_id: device.device_id,
					"minPpm": minPpm,
					"maxPpm": maxPpm,
					"avgPpm": ppmAvgValue
				});
			}
			
		});
		
		res.status(200).send(toReturn);
		
	})
	.catch(error => {
		console.log(error);
	});
});

router.get("/getGasSmokeLatestValues", async (req, res) => {
	
	let options = {
		method: 'get',
		url: 'http://localhost:8080/getLatestData/gasSmoke'
    };
    
    axios(options)
    .then(response => {
		
		const gasSmokeData = response.data;
		
		console.log(gasSmokeData);
		
		let toReturn = [];
		
		gasSmokeData.forEach(device => {
			
			if(device.latest.length == 0) toReturn.push({
				device_id: device.device_id,
				"sample_time": "---",
				"value": "---",
				"state": "---"
			});
			
			else{
				console.log(device.latest[device.latest.length - 1]);
				toReturn.push({
					device_id: device.device_id,
					"sample_time": device.latest[device.latest.length - 1].sample_time,
					"value": device.latest[device.latest.length - 1].device_data.ppm,
					"state": device.latest[device.latest.length - 1].device_data.gas_smoke_state
				});
			}
			
		});
		
		res.status(200).send(toReturn);
		
	})
	.catch(error => {
		console.log(error);
	});
});

router.get("/getTemperatureLatestValues", async (req, res) => {
	
	let options = {
		method: 'get',
		url: 'http://localhost:8080/getLatestData/temperature'
    };
    
    axios(options)
    .then(response => {
		
		const temperatureData = response.data;
		
		console.log(temperatureData);
		
		let toReturn = [];
		
		temperatureData.forEach(device => {
			
			if(device.latest.length == 0) toReturn.push({
				device_id: device.device_id,
				"sample_time": "---",
				"value": "---",
				"state": "---"
			});
			
			else{
				console.log(device.latest[device.latest.length - 1]);
				toReturn.push({
					device_id: device.device_id,
					"sample_time": device.latest[device.latest.length - 1].sample_time,
					"value": device.latest[device.latest.length - 1].device_data.temperature,
					"state": device.latest[device.latest.length - 1].device_data.temperature_state
				});
			}
			
		});
		
		res.status(200).send(toReturn);
		
	})
	.catch(error => {
		console.log(error);
	});
});

module.exports = router;
