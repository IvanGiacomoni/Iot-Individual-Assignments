# First Assignment

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
The **DHT22** sensor is a **digital** sensor that converts the analog signals to digital so that it can be easily read by the MCU. Features are the following:

- Dimensions: 28mm X 12mm X 10mm
- Unit of measurement: Celsius degrees
- Temperature measurement range: [-40 , 80] ?? C
- Periodicity of the measurements: 10 seconds

More info can be found [here](https://learn.adafruit.com/dht).

It is made up of **three pins**:

- **VCC pin** for the power supply voltage of 5V
- **out pin** for the connection to a digital pin where the signal is taken
- **GND pin** for the connection to ground

Below we can see an image of the DHT-22 sensor:

![img](https://m.media-amazon.com/images/I/61OFHCBgTaL._AC_UL320_.jpg)

The **MQ-2** sensor is an **analog** sensor. Features are the following:

- Unit of measurement: parts per million
- Temperature measurement range: [100, 10000] ppm 
- Periodicity of the measurements: 2 seconds

More info can be found [here](https://www.az-delivery.de/en/products/gas-sensor-modul).

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

- ALL OK! -> white led
- WARNING! -> blue led + buzzer

## Architecture of the system

Our system is thought to be used by several **devices**. For this first assignment we used the **STM32 Nucleo F401RE board**, where all the code needed to retrieve data from sensors and to manage the actuators will be flashed. The next step is that of saving the obtained values into the cloud, in particular using **Amazon Web Services**. In order to do this, we need to use **MQTT**, that is a communication protocol based on **topics**: the key is that every device that is subscribed to a certain topic will receive messages published on that topic. In particular, when we will publish the data from the device, those data will be sent to an **MQTT-SN Broker**, in this case **RSMB**, and then from RSMB they will be sent to the **Mosquitto broker**, which is the **transparent bridge** between RSMB and **Iot-Core**, that is a service provided by AWS that uses MQTT and that we can exploit to receive data coming from our device. So, after Mosquitto redirects our data to Iot-Core, there is the needing to save them in the cloud: my choice was **DynamoDB**, a noSQL database provided by AWS. Once data are stored in the db, we can query them using an appropriate **server** in order to finally show them on the **front-end** side: my choice for the back-end side was **NodeJS**, while for the front-end side I used **HTML, Bootstrap, Javascript and Vue-js**. The frontend and the backend will create the **Web Dashboard**. Finally, the frontend will be used also to **control the state of actuators**. Below I show an image that higlights all the connected components of the application:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/architecture_indiv_assign.png)

## Manual and automatic mode
The system is capable of switching between two modes through the interaction by the front-end:

- **auto**: data from both sensors are periodically sampled and published by MQTT towards Iot-Core, actuators are automatically controlled
- **manual**: the periodical sample of data is stopped in order to allow the user to manually control the actuators

This is done because there could be situations in which the manual mode could be useful, for example when sensors wrongly sample values and detect a danger, so I need to stop the auto mode in order to activate the manual mode, and then deactivate the actuators.

## Topics
Below I list the main topics used by the system:

- **device/<device_id>/temperature:** used to publish temperature data from a device with a certain id towards Iot-Core
- **device/<device_id>/gas_smoke:** used to publish gas/smoke data from a device with a certain id towards Ior-Core
- **switchMode/device/<device_id>:** used to publish towards a device with a certain id the indication of switching to auto/manual mode
- **manageActuators/device/<device_id>:** used to publish towards a device with a certain id the indication of turning on/off a certain actuator

## Setup
More informations about the **setup** of this assignment can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/setup.md).

## Nucleo code
More informations about the **iotlab-m3 code** of this assignment can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/nucleo_code/README.md).

## Web dashboard
More informations about the **dashboard** can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/dashboard/README.md).
