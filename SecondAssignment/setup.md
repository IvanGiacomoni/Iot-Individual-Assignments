# Setup

## Installing NodeJS and preparing the server
You need to install **NodeJS** in your operating system in order to have the web dashboard working correctly, [here](https://nodejs.org/en/) you can find details. After this, you have to check if you have **npm** installed, otherwise you can install it from [here](https://www.npmjs.com/get-npm). You also need to require the **.env** file, that has to placed in the dashboard directory. More info about the .env file will come later.

## Nodemon (not mandatory)
Now you can install **nodemon**, a very useful package to run our NodeJS server, with this command: **npm install -g nodemon** of you are on Windows, or **sudo npm install -g nodemon** if you are on Linux/Ubuntu.

## AWS, Iot-Core and DynamoDB
Then, you need so sign in in [AWS](https://aws.amazon.com/education/awseducate/), in order to get access to **Iot-Core** and **DynamoDB** services. Once you're on Iot-Core, you need to create a **thing** in order to get your certificate, root certificate and private key. Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html). Then you need to add into the **.env** file the **access key id**, the **secret access key** and the **session token** provided by AWS, to be able to receive messages from the devices. Then you need to **subscribe** to all **topics** I mentioned [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/tree/main/SecondAssignment#mqtt-topics) by going in the MQTT test client section.

## Iot-lab experiment
Then you need to submit an **experiment** on the [Iot-lab testbed](https://www.iot-lab.info/testbed/dashboard) with **one A8-node** at least **2 M3-nodes** on the same **frontend**, between those offered by Iot-lab: for our purposes, the recommended choice is the **Saclay** frontend. In particular, you need to add a **monitoring profile** for each of the M3-nodes, in order to evaluate the performances next. Below I show a picture of the monitoring profile I used:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/iot-lab-monitoring-profile.png)

Below I show a picture of one experiment I submitted, with 4 M3-nodes and one A8-node:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/iot-lab-experiment.png)

## Create the IPv6 network in IoT-LAB with the M3-nodes
Then you need to create the **IPv6 network** in Iot-LAB, and in order to do this you can follow this [tutorial](https://www.iot-lab.info/learn/tutorials/riot/riot-public-ipv6-m3/): as you can see, on the M3-node that will act as you border router you need to flash the firmware related to the RIOT [gnrc_border_router example](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router), while on the other M3-nodes you need to flash the firmware related to our application, and you can find more details [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/README.md).

## A8-node setup for MQTT communication
Then you need to setup the **A8-node** for **MQTT** communication.

### Configuring RSBM (MQTT-SN broker)
For the configuration of the **MQTT-SN broker** you can follow this [tutorial](https://www.iot-lab.info/learn/tutorials/riot/riot-mqtt-sn-a8-m3/) by looking at the **step 9**. Particularly, at this point the config.conf file should look like this:

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
For the configuration of the **Mosquitto broker** we have to copy 
