const express = require("express");
const router = express.Router();
const axios = require("axios");
const {publishByMqtt} = require("../helper/mqtt");

//publishByMqtt(process.env.TEMPERATURE_TOPIC, {mex: "ciao", numero: 2});



module.exports = router;
