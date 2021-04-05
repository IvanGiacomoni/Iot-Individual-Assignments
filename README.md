# Gas-Leaks-And-Fires-Notifier

## Scenario
The problem I took into consideration is that of **gas leaks** and **fires** that usually occur in home environments. This problem is very huge, because of the serious injuries and deaths it causes, especially from the fires that are generated after the explosion. Iot is very helpful when we can prevent these terrible situations, for example by using a notification system: the main **goal** of Gas-Leaks-And-Fires-Notifier is that of **preventing the outbreak of fires** by a constant monitoring of the home environment, in particular those points of the house where it is more probable for a gas leak to occur, such as the kitchen.

## Sensors
The environment will be monitored with the help of **two sensors**, which will help us to retrieve the data that are useful to our analysis:

- the **DHT-22** sensor for retrieving information about the environment **temperature**
- the **MQ-2** sensor for retrieving information about the amount of **gas or smoke** that we have in the environment

For acting into the environment I used **two** different types of **actuators**:

- **blink leds**, in order to have a **visual notification** if there is an high probability of an imminent gas leak
- **buzzers**, in order to have a **sound notification** if there is an hign probability of an imminent gas leak

The **sampling** of data from both sensors will be **periodic**.

## Actuators
Speaking about the actuators, their purposes are strictly connected to data retrieved from both sensors: in particular, I used **three leds** and **one buzzer** for acting with respect to **temperature** data, and **two leds** and **one buzzer** for acting with respect to **gas and smoke** data.

**Temperature actuators:**

- **green led**, used to notify people that actual values of temperature measured by the DHT-22 sensor are **below a medium threshold**, and so not dangerous
- **yellow led**, used to notify people that actual values of temperature measured by the DHT-22 sensor are **above the medium threshold** but **below an high threshold**, so there is no real danger, but we have to pay attention to what we are actually doing
- **red led**, used to notify people, together with the buzzer, that actual values of temperature measured by the DHT-22 sensor are **above the high threshold**
- **buzzer**, used to notify people, together with the red led, that actual values of temperature measured by the DHT-22 sensor are **above the high threshild**

**Gas and smoke actuators:**

- **white led**, used to notify people that actual amounts of gas or smoke measured by the MQ-2 sensor are **below a control threshold**, and so not dangerous
- **blue led**, used to notify people, together with the buzzer, that actual amounts of gas or smoke measured by the MQ-2 sensor are **above the control threshold**
- **buzzer**, used to notify people, together with the blue led, that actual amounts of gas or smoke measured by the MQ-2 sensor are **above the control threshold**

## Data collected
The DHT22 is a **digital** sensor that converts the analog signals to digital so that it can be easily read by the MCU. Features are the following:

- Dimensions: 28mm X 12mm X 10mm
- Unit of measurement: Celsius degrees
- Temperature measurement range: [-40 , 80] ° C
- Periodicity of the measurements: 10 seconds

It is made up of three pins:

- **VCC pin** for the power supply voltage of 5V
- **out pin** for the connection to a digital pin where the signal is taken
- **GND pin** for the connection to ground

Below we can see an image of the DHT-22 sensor:

![img](https://m.media-amazon.com/images/I/61OFHCBgTaL._AC_UL320_.jpg)

The MQ-2 sensor is an analog sensor. Features are the following:

- Unit of measurement: parts per million
- Temperature measurement range: [100, 10000] ppm 
- Periodicity of the measurements: 2 seconds

It is made up of four pins:

- **VCC pin** for the power supply voltage of 5V
- **AOUT pin** for the analog exit
- **DOUT pin** for the digital exit (we will not use it)
- **GND pin** for the connection to ground

Below we can see an image of the MQ-2 sensor:

![img](https://m.media-amazon.com/images/I/71knmA03nPL._AC_UL320_.jpg)

I want to notice that gas and smoke will be sampled more often than temperature data, because even if both are important in our scenario, it is clear that the greater danger will come from smoke and gas.

The system will be capable of determining whether the values retrieved by the sensors are dangerous or not, and these observations will be done for both temperature and gas/smoke data by looking at values with respect to the above cited thredholds.
I want to specify that, based on the values of temperature and concentration of gas and smoke, the system will associate a **state** to that feature in the environment: in particular, for temperature we have 3 states and for gas and smoke 2 states:

**Temperature states:**

- ALL OK! -> green led
- GROWING! -> yellow led
- WARNING! -> red led + buzzer

**Gas/smoke states:**

- ALL OK -> white led
- WARNING -> blue led + buzzer

## Architecture of the system

Our system is thought to be used by several **devices**. For this first assignment we used the **STM32 Nucleo F401RE board**, where all the code needed to retrieve data from sensors and to manage the actuators will be flashed. The next step is that of saving the obtained values into the cloud, in particular using **Amazon Web Services**. In order to do this, we need to use **MQTT**, that is a communication protocol based on **topics**: the key is that every device that is subscribed to a certain topic will receive messages published on that topic. In particular, when we will publish the data from the device, those data will be sent to an **MQTT-SN Broker**, in this case **RSMB**, and then from RSMB they will be sent to the **Mosquitto broker**, which is the **transparent bridge** between RSMB and **Iot-Core**, that is a service provided by AWS that uses MQTT and that we can exploit to receive data coming from our device. So, after Mosquitto redirects our data to Iot-Core, there is the needing to save them in the cloud: my choice was **DynamoDB**, a noSQL database provided by AWS. Once data are stored in the db, we can query them using an appropriate **server** in order to finally show them on the **front-end** side: my choice for the back-end side was **NodeJS**, while for the front-end side I used **HTML, Bootstrap, Javascript and Vue-js**. The frontend and the backend will create the **Web Dashboard**. Finally, the frontend will be used also to **control the state of actuators**. Below I show an image that higlights all the connected components of the application:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/architecture_indiv_assign.png)

## Manual and automatic mode
The system is capable of switching between two modes through the interaction by the front-end:

- **auto**: data from both sensors are periodically sampled and published by MQTT towards Iot-Core, actuators are automatically controlled
- **manual**: the periodical sample of data is stopped in order to allow the user to manually control the actuators

This is done because there could be situations in which the manual mode could be useful, for example when sensors wrongly sample values and detect a danger, so I need to stop the auto mode in order to activate the manual mode, and then deactivate the actuators.

## Topics
Below I list the main topics used by the system:

- **device/+/temperature:** used to publish temperature data from a device with a certain id towards Iot-Core
- **device/+/gas_smoke:** used to publish gas/smoke data from a device with a certain id towards Ior-Core
- **switchMode/device/+:** used to publish towards a device with a certain id the indication of switching to auto/manual mode
- **manageActuators/device/+:** used to publish towards a device with a certain id the indication of turning on/off a certain actuator

## Setup

### The RIOT operating system
We need to install the **RIOT operating system**, [here](https://github.com/RIOT-OS/Tutorials#tutorials-for-riot) you can find a tutorial for the installation.

### The STM32 Nucleo-64 F401RE board
You need to have available an **STM-32 Nucleo-64 F401RE board**, [here](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) you can find info about it.

### Wiring of components
For this application we will use:

- 5 blink leds (red, yellow, green, white and blue)
- 2 buzzers
- a DHT-22 sensor
- an MQ-2 sensor
- 2 100 Ohm resistors
- 5 220 Ohm resistors

For each led, we have to use a **ballast resistor of 220 Ohm**, and we need to connect the **anode** of the led to a digital pin of the board, while the **cathode** will be connected to one side of the resistor, and finally the other side of the resistor will be connected to ground. Below I list the specific digital pin of the board for each led:

- red led -> D4 pin
- green led -> D7 pin
- yellow led -> D8 pin
- blue led -> D10 pin
- white led -> D12 pin

For each buzzer, we have to use a **resistor of 100 Ohm**, and we need to connect the **positive pin** of the buzzer to a digital pin of the board, while the **negative pin** will be connected to one side of the resistor, and finally the other side of the resistor will be connected to ground. Below I list the specific digital pin of the board for each buzzer:

- temperature buzzer -> D13 pin
- gas/smoke buzzer -> D9 pin

For the DHT-22 sensor we need to connect the **GND pin** to ground, the **VCC pin** to 5V on the breadboard and the **out pin** to the D2 pin of the board.
For the MQ-2 sensor we need to connect the **GND pin** to ground, the **VCC pin** to 5V on the breadboard and the **AOUT pin** to the A0 pin of the board.

Below we can see the **wiring of components**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/circuit/circuit_bb.png)

Below I show a **picture of the circuit**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/circuit/circuit_picture.jpeg)

**NOTE!** As you can see from the wiring, I have to point out a few things: the temperature sensor is not the DHT-22 in the picture, but is an RHT sensor, that works with the same logic; the gas sensor is not an MQ-2 sensor in the picture, but also here the logic is the same. For leds they are all red in the picture but, starting from top and going to bottom, we have in the order the red led, the green led, the yellow led, the blue led and the white led; finally for resistors, they are all 220 Ohm resistors in the picture but, as mentioned before, they are clearly distinguished in 220 Ohm for leds and 100 Ohm for buzzers.

### Installing NodeJS and preparing the server
You need to install **NodeJS** in your operating system in order to have the web dashboard working correctly, [here](https://nodejs.org/en/) you can find details.
After this, you have to check if you have **npm** installed, otherwise you can install it from [here](https://www.npmjs.com/get-npm).
You also need to require the **.env** file, that has to placed in the dashboard directory. More info about the .env file will come later.

### Nodemon
After doing all this, you can download the zip file from [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments). Then, you need to open a terminal in the dashboard directory, and launch **npm install**. After this, you can install **nodemon** (not mandatory), a very useful package to run our NodeJS server, with this command: **npm install -g nodemon**, or **sudo npm install -g nodemon** if you are on Linux/Ubuntu.

### Installing RSBM (MQTT-SN broker)
You need to install the **RSBM broker**, and you can follow this [tutorial](https://github.com/RIOT-OS/RIOT/tree/master/examples/emcute_mqttsn). Particularly, at this point the **config.conf** file should look like this.

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

### Installing MQTT-cli
You need also to install this [MQTT-cli](https://www.hivemq.com/blog/mqtt-cli/), in order to do publish/subscribe requests from the command line.

### AWS, Iot-Core and DynamoDB
Then, you need so sign in in [AWS](https://aws.amazon.com/education/awseducate/), in order to get access to **Iot-Core** and **DynamoDB** services. Once you're on Iot-Core, you need to create a **thing** in order to get your certificate, root certificate and private key. Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html). Then you need to add into the **.env** file **the access key id, the secret access key and the session token** provided by AWS, to be able to receive messages from the devices. Then you need to **subscribe** to all **topics** I mentioned before by going in the MQTT test client section.

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/subscribe_aws.png)

Then by going in the Rules section you have to create two **rules**, one for temperature data and one for gas and smoke data, specifying as topics device/+/temperature and device/+/gas_smoke respectively, and as operation you need to choose "Insert a message into a DynamoDB table". Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html).

**Temperature rule:**

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/temperature_rule.png)

**Gas/Smoke rule:**

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/gas_smoke_rule.png)

### Installing and setup Mosquitto broker
Now you have to install the **Mosquitto broker**, more info and details can be found [here](https://aws.amazon.com/it/blogs/iot/how-to-bridge-mosquitto-mqtt-broker-to-aws-iot/). After doing this, you have to modify the previous config.conf file by adding the topics I mentioned before, so at this point the **config.conf** file should look like this:

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
  address 127.0.0.1:1883
  topic device/+/temperature both
  topic device/+/gas_smoke both
  topic switchMode/device/+ both
  topic manageActuators/device/+ both
```

Also, you need to set up a **bridge.conf** file in order to have the bridging between Iot-Core and RSMB:

```
# ============================================================
# Bridge to AWS IOT
# ============================================================

connection awsiot

#<Paste your AWS IoT Core ATS endpoint retrieved from the AWS CLI in the form of xxxxxxxxxxxxxxx-ats.iot.<region>.amazonaws.com:8883

address yourAWSIotCoreEndopoint.com:8883

# Specifying which topics are bridged and in what fashion
topic device/+/temperature both 1
topic device/+/gas_smoke both 1
topic switchMode/device/+ both 1
topic manageActuators/device/+ both 1

# Setting protocol version explicitly
bridge_protocol_version mqttv311
bridge_insecure false

# Bridge connection name and MQTT client Id, enabling the connection automatically when the broker starts.
cleansession true
clientid bridgeawsiot
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

Be sure to use same names as these above for the environment variables.

**NOTE!** The access key id, the secret access key and the session token provided by AWS expire after a while, so you have to manually re-set them in the .env file when this happens.

### Launching the application

- Open a terminal
- Start the mosquitto broker by launching **service mosquitto start**
- Now go into the folder where you have your config.conf file, and start the RSMB broker with **./broker_mqtts config.conf**
- Open another terminal in the dashboard folder
- Launch the nodeJS server with **node index.js** or **nodemon** (if you installed it)
- Open another terminal in the root folder of the project
- Launch the application by flashing the code into the STM-32 board with **make BOARD=nucleo-f401re flash term**

Below I show a picture of the three **consoles** running, on the top left we have the rsbm broker, on the top right we have the nodeJS server, on the bottom we have the STM-32 board:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/consoles.png)

When we flash the code into the board, the application will start in *auto* mode, and on the console we can see these debug prints:

```
Starting the application...
DHT sensor connected

Successfully initialized ADC_LINE(0)

Red led ready!
Green led ready!
Yellow led ready!
Blue led ready!
White led ready!

Gas/Smoke buzzer ready!
Temperature buzzer ready!

Setting up MQTT-SN.
Connecting to MQTT-SN broker fe80::1 port 1885.
Successfully connected to gateway at [fe80::1]:1885

Now subscribed to device/1/temperature

Now subscribed to device/1/gas_smoke

Now subscribed to switchMode/device/1

Now subscribed to manageActuators/device/1

Creating thread for sampling temperature...
No arguments for thread temp
I'm temperature thread with pid 6
Temperature thread created...
Creating thread for sampling ppm...
No arguments for thread temp
I'm gas/smoke thread with pid 7
Gas/smoke thread created...

[GAS/SMOKE] mode: auto
ppm: 65
[GAS/SMOKE] ALL OK!
### got publication for topic 'device/1/gas_smoke' [2] ###
{ "ppm": 65, "gas_smoke_state": "ALL_OK!" }

[TEMPERATURE] mode: auto
temperature: 22°C
[TEMPERATURE] ALL OK!
### got publication for topic 'device/1/temperature' [1] ###
{ "temperature": 22, "temperature_state": "ALL OK!" }

[GAS/SMOKE] mode: auto
ppm: 66
[GAS/SMOKE] WARNING!!!
### got publication for topic 'device/1/gas_smoke' [2] ###
{ "ppm": 66, "gas_smoke_state": "WARNING!" }

[GAS/SMOKE] mode: auto
ppm: 67
[GAS/SMOKE] WARNING!!!
### got publication for topic 'device/1/gas_smoke' [2] ###
{ "ppm": 67, "gas_smoke_state": "WARNING!" }

```
On AWS we can see both temperature and gas/smoke values arriving on topics **device/1/temperature** and **device/1/gas_smoke**.<br/><br/>
**Temperature:**<br/><br/>

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/temp_topic_aws.png)
<br/><br/>

**Gas/Smoke:**<br/><br/>
![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/gas_smoke_topic_aws.png)
<br/><br/>

As said before, data are saved into **DynamoDB** into **two different tables**, one for temperature and the other one for gas and smoke data.<br/><br/>
**Temperature:**<br/><br/>

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/temp_data_dynamoDB.png)
<br/><br/>
**Gas/Smoke:**<br/><br/>

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/gas_smoke_data_dynamoDB.png)<br/><br/>

The server will be available at ***http://localhost:8080/***, where we can see the **homepage**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/homepage.png)

If we click on *Temperature* we will see the **temperature values received during the last hour** from the DHT-22 sensor for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/temp_last_hour_values.png)

By clicking on *Last values*, we will see the **latest values** received from the DHT-22 sensor for each device.

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/temp_latest_values.png)

By clicking on *Back* and then on *Last hour aggregated values*, we will see the **aggregated values (average, minimum and maximum)** for the DHT-22 sensor **retrieved during the last hour** for each device.

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/temp_aggregated.png)

All this could be seen also for **gas and smoke values** retrieved from the MQ-2 sensor. So below I show the **last hour values** for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/gas_smoke_last_hour_values.png)

Below we can see the **latest values** for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/gas_smoke_latest_values.png)

Below we can see the **aggregated values (average, minimum and maximum) retrieved during the last hour** for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/gas_smoke_aggregated.png)

If we go back to last hour temperature values, by clicking on *Actuators* we have the possibility to **switch mode** for the application **(auto and manual)**, and also to **manage** the red led and the temperature buzzer, for each device. If we consider device 1, by clicking on *Manual*, we will get this message on the website:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/switchToManual.png)

On AWS we will get this message under the **switchMode/device/1 topic**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/switchMode_topic_aws.png)

Notice that we get an error because we are sending a normal string message, but we can ignore it.
Also, on the console we can see these messages:

```
### got publication for topic 'switchMode/device/1' [3] ###
received: manual
Switching to manual...

[GAS/SMOKE] mode: manual
[GAS/SMOKE] Now in manual mode

[TEMPERATURE] mode: manual
[TEMPERATURE] Now in manual mode
```

At this point, if we click, for example, on *red led ON*, we get this message on the webpage:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/red_led_ON.png)

On AWS we will get this message under the **manageActuators/device/1** topic, and we got an error for the same reason as before:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/manageActuators_topic_aws.png)

Also, on the console we can see these messages:

```
### got publication for topic 'manageActuators/device/1' [4] ###
received: red_led;temperature;ON
actuator: red_led
typeData: temperature
operation: ON
```

The same logic is applied for gas/smoke actuators.

## The code
All details about the code can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/code.md).

