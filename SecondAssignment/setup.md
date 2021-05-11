# Setup

## The RIOT operating system
We need to install the **RIOT operating system**, [here](https://github.com/RIOT-OS/Tutorials#tutorials-for-riot) you can find a tutorial for the installation.

## Installing NodeJS and preparing the server
You need to install **NodeJS** in your operating system in order to have the web dashboard working correctly, [here](https://nodejs.org/en/) you can find details. After this, you have to check if you have **npm** installed, otherwise you can install it from [here](https://www.npmjs.com/get-npm). You also need to require the **.env** file, that has to be placed in the dashboard directory. More info about the .env file will come later.

## Nodemon (not mandatory)
Now you can install **nodemon**, a very useful package to run our NodeJS server, with this command: **npm install -g nodemon** if you are on Windows, or **sudo npm install -g nodemon** if you are on Linux/Ubuntu.

## Installing MQTT-cli (not mandatory)
You can also install this [MQTT-cli](https://www.hivemq.com/blog/mqtt-cli/), in order to do publish/subscribe requests from the command line.

## AWS, Iot-Core and DynamoDB
Then, you need so sign in in [AWS](https://aws.amazon.com/education/awseducate/), in order to get access to **Iot-Core** and **DynamoDB** services. Once you're on Iot-Core, you need to create a **thing** in order to get your **certificate**, **root certificate** and **private key**. Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html). Then you need to add into the **.env** file the **access key id**, the **secret access key** and the **session token** provided by AWS, to be able to receive messages from the devices. At this point the .env file sholud look like this:

```
AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
AWS_SESSION_TOKEN=YOUR_AWS_SESSION_TOKEN
```

Then by going in the Rules section you have to create two **rules**, one for temperature data and one for gas and smoke data, specifying as topics temperature/device/<device_id> and gas_smoke/device/<device_id> respectively, and as operation you need to choose "Insert a message into a DynamoDB table". Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html).

**Temperature rule:**

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/temperature_rule.png)

**Gas/Smoke rule:**

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/gas_smoke_rule.png)

So it is clear that we will create **two DynamoDb tables**, one for each rule: particularly, each entry of both tables is made up of the **Unix timestamp** associated to the creation of that entry, that will be useful for quering the last hour values of temperature and gas/smoke, the **device_id** of the device who has sampled that value, and the **device_data**, which consists of the **value** sampled and the **state** associated to that value. 

## Iot-lab experiment
Then you need to submit an **experiment** on the [Iot-lab testbed](https://www.iot-lab.info/testbed/dashboard) with **one A8-node** and at least **2 M3-nodes** on the same **frontend**, between those offered by Iot-lab: for our purposes, the recommended choice is the **Saclay** frontend. In particular, you need to add a **monitoring profile** for each of the M3-nodes, in order to evaluate the performances next. Below I show a picture of the monitoring profile I used:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/iot-lab-monitoring-profile.png)

Below I show a picture of one experiment I submitted, with 4 M3-nodes and one A8-node:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/iot-lab-experiment.png)

## Create the IPv6 network in IoT-LAB with the M3-nodes
Then you need to create the **IPv6 network** in Iot-LAB, and in order to do this you can follow this [tutorial](https://www.iot-lab.info/learn/tutorials/riot/riot-public-ipv6-m3/): as you can see, on the M3-node that will act as you border router you need to flash the firmware related to the RIOT [gnrc_border_router example](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router), while on the other M3-nodes you need to flash the firmware related to our application, and you can find more details [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/README.md).

## A8-node setup for MQTT communication
Then you need to setup the **A8-node** for **MQTT** communication.

### Configuring RSBM (MQTT-SN broker)
For the configuration of the **MQTT-SN broker** you can follow this [tutorial](https://www.iot-lab.info/learn/tutorials/riot/riot-mqtt-sn-a8-m3/) by looking at the **step 9**. Particularly, at this point the **config.conf** file should look like this:

```
# add some debug output
trace_output protocol

# listen for MQTT-SN traffic on UDP port 1885
listener 1885 INADDR_ANY mqtts
  ipv6 true

# listen to MQTT connections on tcp port 1886
listener 1886 INADDR_ANY
  ipv6 true
```

### Configuring Mosquitto
For the configuration of the **Mosquitto broker** we have to copy the **certificate**, **root certificate** and **private key** from our local machine into the A8 folder of the A8-node: particularly, if the you followed correctly the steps from the previous assignment, you can do this by executing the command **scp -r /etc/mosquitto/certs/ 'username'@saclay.iot-lab.info:~/A8/** in a terminal opened in the root folder of your local machine, where 'username' is your Iot-lab username.
Then, by going in the A8 folder of the A8 node with **cd A8**, you need to copy them in the /etc/mosquitto directory of the A8-node with the command **cp -r certs/ /etc/mosquitto**. Then you need to modify the **config.conf** file:

```
# add some debug output
trace_output protocol

# listen for MQTT-SN traffic on UDP port 1885
listener 1885 INADDR_ANY mqtts
  ipv6 true

# listen to MQTT connections on tcp port 1886
listener 1886 INADDR_ANY
  ipv6 true

# MQTT-S outgoing local bridge
connection local_bridge_to_mosquitto
  address [MQTT-SN-broker-address]:1883
  topic temperature/device/+ both
  topic gas_smoke/device/+ both
  topic switchMode/device/+ both
  topic manageActuators/device/+ both
```

**NOTE**: '+' stands for 'at least one character' by the syntax of regular expressions, but in our case it will be filled with the device id.
Be sure to insert the correct address of the MQTT-SN broker.

Finally, you need to setup a **bridge.conf** file in the A8 directory of the node in order to have the bridging between Iot-Core and RSMB:

```
# ============================================================
# Bridge to AWS IOT
# ============================================================

connection awsiot

#<Paste your AWS IoT Core ATS endpoint retrieved from the AWS CLI in the form of xxxxxxxxxxxxxxx-ats.iot.<region>.amazonaws.com:8883

address yourAWSIotCoreEndopoint.com:8883

# Specifying which topics are bridged and in what fashion
topic temperature/device/+ both 1
topic gas_smoke/device/+ both 1
topic switchMode/device/+ both 1
topic manageActuators/device/+ both 1

# Setting protocol version explicitly
bridge_protocol_version mqttv311
bridge_insecure false

# Bridge connection name and MQTT client Id, enabling the connection automatically when the broker starts.
cleansession true
clientid myclientid
start_type automatic
notifications false
log_type all

# ============================================================
# Certificate based SSL/TLS support
# ============================================================

#Path to the rootCA
bridge_cafile yourPath/root-CA.crt

# Path to the PEM encoded client certificate
bridge_certfile yourPath/mqtt-bridge.cert.pem

# Path to the PEM encoded client private key
bridge_keyfile /etc/mosquitto/certs/mqtt-bridge.private.key

#END of bridge.conf
```

Be sure to have the right path and files for the private key, the certificate and the root certificate.

Finally you need to add into the **.env** file **a unique client id, the host address provided on Iot-Core, and the paths for the certificate and the root-certificate**, in order to be able to send messagges from the frontend to manage the actuators. At this point the **.env** file should look like this:

```
AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
AWS_SESSION_TOKEN=YOUR_AWS_SESSION_TOKEN
CERT=path/certificate.cert.pem
ROOT_CA=path/root-CA.crt
CLIENT_ID=YOUR_UNIQUE_CLIENT_ID
HOST=YOUR_AWS_HOST
```

Be sure to use same names as these above for the environment variables, and be sure to change the clientid.

**NOTE!** The access key id, the secret access key and the session token provided by AWS expire after a while, so you have to manually re-set them in the .env file when this happens.

## Launching the application
At this point you should already have a terminal on the Saclay frontend with the border router running, and below I show a picture of it:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/border_router.png)

Now you have to do the following steps:
- Download the repository from [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/archive/refs/heads/main.zip), or clone it with **git clone https://github.com/IvanGiacomoni/Iot-Individual-Assignments.git**
- Open a terminal in the SecondAssignment/dashboard folder
- Launch npm install
- Launch the nodeJS server with **node index.js** or **nodemon** (if you installed it)
- Open a terminal in the A8-node of your experiment
- Start the RSMB broker with **broker_mqtts config.conf**
- Open another terminal in the A8-node of your experiment, and then go in the A8 folder with **cd A8**
- Start the Mosquitto broker with **mosquitto -c bridge.conf**
- In the Saclay frontend, open a number of terminal equal to the **number of M3-nodes of your experiment - 1**
- In each one, launch the application with **nc m3-'node_id' 20000**, where 'node_id' is the id of the M3-node, and then, in the shell that you have in each terminal, type the command **start 'MQTT-SN-broker-address' 'device_id'**, where 'MQTT-SN-broker-address' is the address of the MQTT-SN broker, and 'device_id' must be **unique** for each node.

The server will be available at http://localhost:8080/.

## Consoles

### Mosquitto broker

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/mosquitto.png)

### RSMB broker

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/rsmb.png)

### Application on three M3-nodes

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/m3-nodes.png)

### NodeJS server

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/server.png)
