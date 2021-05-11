# Second Assignment

## Wireless Sensor Network
For the second assignment, we need to develop a **Wireless Sensor Network**: the idea is that of setupping an environment with multiple devices, where each one is able to generate the same type of data from the [previous assignment](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/README.md) and send them to the [AWS](https://aws.amazon.com/education/awseducate/) cloud platform.

## Benefits and disadvantages
The main idea of a Wireless Sensor Network is that of **helping human beings** to retrieve data where it is hard for them to do this manually: so, as a consequence, there is the possibility to retrieve data from **different locations** that leads to a better **data quality**. Also, they are very easy to **install**, as well as they are very **flexible and scalable**, in the sense that they are very easy to modify and to adapt for new incoming technologies. Speaking about the **fault tolerance**, obviously if a node (device) of our network crashes or in general stops working, this could lead to a less accurate solution for the problem taken into consideration, but in general a good network is able to reduce at minimum faults in the system. The **manteinance** is related to the **energy efficiency**, so if we have a lot of devices, obviously we have less energy efficiency and manteinance will be harder. By looking at **costs**, those for installation will be higher, while those for changes into the network will be lower. There are also technical limitations in terms of **throughput** and **end-to-end** delay, because these networks are made up of **low-energy** and **low-range** devices, so depending of the network conditions, throughput can increase as well as end-to-end delay can increase.

## Architecture of the system
The Wireless Sensor Network will be simulated on [Iot-lab](https://www.iot-lab.info/), where we need to setup one **m3-node** as a **border router**, at least one **m3-node** for running our **application**, and an **A8-node** where we will configure an **MQTT-SN broker (RSMB)** and the **Mosquitto broker**. The **border router** will receive, by **IPV6 (6LoWPAN/RPL)**, the data generated on the other **M3-nodes** where the application is running. Then the data are sent by **IPV6** to the **MQTT-SN broker** on the **A8-node**, and then they will be sent by **MQTT** to the **Mosquitto broker**, which is the **transparent bridge** between RSMB and **Iot-Core**, that is a service provided by AWS that uses MQTT and that we can exploit to receive data coming from our devices. So, after Mosquitto redirects our data to Iot-Core, there is the needing to save them in the cloud: my choice was **DynamoDB**, a noSQL database provided by AWS. Once data are stored in the db, we can query them using an appropriate server in order to finally show them on the **front-end** side: my choice for the back-end side was **NodeJS**, while for the front-end side I used **HTML, Bootstrap, Javascript and Vue-js**. The frontend and the backend will create the **Web Dashboard**. Finally, the frontend will be used also to **control the state of actuators**. Below I show an image that higlights all the connected components of the application:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/architecture_hw2.png)

## Performance of the system
The performance of the system will be carried out with the tools provided by [Iot-lab](https://www.iot-lab.info/). In particular, we will measure and evaluate:

- the performance of the wireless sensor networ in terms of **end-to-end latency** and **throughput**
- the performance of the system as the **number of wireless elements** increases
- the **energy consumption** and **duty cycling** of the nodes

## More details

### Manual and automatic mode
The system is capable of **switching** between two **modes** through the interaction by the front-end:

- **auto**: data from both sensors are periodically sampled and published by MQTT towards Iot-Core, actuators are automatically controlled
- **manual**: the periodical sample of data is stopped in order to allow the user to manually control the actuators

This is done because there could be situations in which the manual mode could be useful, for example when sensors wrongly sample values and detect a danger, so I need to stop the auto mode in order to activate the manual mode, and then deactivate the actuators.

### MQTT topics
Below I list the main **MQTT topics** used by the system:

- **temperature/device/'device_id'**: used to publish temperature data from a device with a certain id towards Iot-Core
- **gas_smoke/device/'device_id'**: used to publish gas/smoke data from a device with a certain id towards Ior-Core
- **switchMode/device/'device_id'**: used to publish towards a device with a certain id the indication of switching to auto/manual mode
- **manageActuators/device/'device_id**: used to publish towards a device with a certain id the indication of turning on/off a certain actuator

## Setup
More informations about the **setup** of this assignment can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/setup.md).

## Performance evaluation
More informations about the **performance evaluation** conducted can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/performance_evaluation.md).

## Iotlab-m3 code
More informations about the **iotlab-m3 code** of this assignment can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/README.md).

## Web dashboard
More informations about the **dashboard** can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/dashboard/README.md).
