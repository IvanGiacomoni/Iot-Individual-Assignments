const express = require("express");
const router = express.Router();

require("dotenv").config();

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
  docClient.scan(paramsTemp, (err, data) => {
	  if (err) {
        console.error("Unable to scan temperature table. Error JSON:", JSON.stringify(err, null, 2));
        return res.status(404).send("Unable to scan the table");
    } else {
        
        for(let i=0;i<data.Items.length;i++){
			
			data.Items[i].sample_time = new Date(data.Items[i].sample_time);
			console.log("Item :", ++countTemp,JSON.stringify(data.Items[i]));
		}

        // continue scanning if we have more items
        /*if (typeof data.LastEvaluatedKey != "undefined") {
            console.log("Scanning for more...");
            params.ExclusiveStartKey = data.LastEvaluatedKey;
            docClient.scan(paramsTemp, onScan);
        }*/
        
        return res.status(200).send(data.Items);
    }
  });
});

router.get("/getGasSmokeData", async (req, res) => {
  docClient.scan(paramsGasSmoke, (err, data) => {
	  if (err) {
        console.error("Unable to scan gas-smoke table. Error JSON:", JSON.stringify(err, null, 2));
        return res.status(404).send("Unable to scan the table");
    } else {     
        console.log("Scan succeeded.");
        
        for(let i=0;i<data.Items.length;i++){
			data.Items[i].sample_time = new Date(data.Items[i].sample_time);
			console.log("Item :", ++countTemp,JSON.stringify(data.Items[i]));
		}

        // continue scanning if we have more items
        /*if (typeof data.LastEvaluatedKey != "undefined") {
            console.log("Scanning for more...");
            params.ExclusiveStartKey = data.LastEvaluatedKey;
            docClient.scan(paramsGasSmoke, onScan);
        }*/
        
        return res.status(200).send(data.Items);
    }
  });
});

module.exports = router;
