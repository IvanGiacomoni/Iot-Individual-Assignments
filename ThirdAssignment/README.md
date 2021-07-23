# Third assignment
The goal of this assignment is to replace the wireless mesh network with a **long-range low-power network**: in particular we will focus on the development of **IoT Edge Analytics**.

## Motivations
These networks are used because, at some point, we have to connect a lot of devices to the Internet and so, even if we have Bluetooth or Wifi, we need some other better technologies with a **wider range** for the networking. **Endpoints costs** need to be low, and also we need to send a **small amount of data**. The **energy consumption** must be low.
In a **LPWAN**, the data rate is selected by a **tradeoff** between the **communication range** and the **duration of the message**. What is very important is the **battery life**, and also the **range**, which has to be about **2-5 km** of coverage range in **urban perimeters** and about **45 km** in **rural areas**. We also have to consider the **latency** and the **throughput** (data rates between **290 bps** and **50 kbps**).

## How is the long-range low-power wide are network going to affect the IoT platform?
For my system, given that one of the main constraints is that of having a **real time** evaluation of a possible gas leak, LoraWAN is not so useful, due to its internal latency caused by the **low data rate**. Also for the **battery life**, my system does not have problems, because Gas-Leak-and-Fire-Notifier is thought to be useful in a home environment, especially in the kitchen, and so we will always have the power supply needed for the device. A possible scenario where LoraWAN could be useful is into a house where we don't have any Wifi (and so also MQTT) or Bluetooth available, but we know that within a certain range we will find a LoraWAN gateway that could be exploited for our purposes.

## What are the connected components, the protocols to connect them and the overall IoT architecture?
Below we can see a picture that highlights all the connected components:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/images/architecture-hw-3.png)

The long-range low-power wide area network will be simulated on [Iot-lab](https://www.iot-lab.info/), where we need to setup at least one **B-L072Z-LRWAN1-node** for running our **application**. From each device on Iot-lab we will send by the data by **LoraWAN** to [TheThingsNetwork](https://www.thethingsnetwork.org/), which is a public Lora network, that can be exploited to connect the devices on Iot-lab to a gateway (potentially owned by another person) on this network. From the gateway, data will be sent by **MQTT** to **Iot-Core**, that is a service provided by AWS that uses MQTT and that we can exploit to receive data coming from our devices. Then there is the needing to save them in the cloud: my choice was **DynamoDB**, a noSQL database provided by AWS. Once data are stored in the db, we can query them using an appropriate server in order to finally show them on the **front-end** side. My choice for the back-end side was **NodeJS**, while for the front-end side I used **HTML, Bootstrap, Javascript and Vue-js**. The frontend and the backend will create the **Web Dashboard**. Finally, the frontend will be used also to **control the state of actuators**.

## Performance evaluation
It was not possible to do a performance evaluation because of several issues affecting the **B-L072Z-LRWAN1 nodes** in the [IoT-Lab](https://www.iot-lab.info/) system.

## Manual and automatic mode
The system is capable of **switching** between two **modes** through the interaction by the front-end:

- **auto**: data from both sensors are periodically sampled and published by MQTT towards Iot-Core, actuators are automatically controlled
- **manual**: the periodical sample of data is stopped in order to allow the user to manually control the actuators

This is done because there could be situations in which the manual mode could be useful, for example when sensors wrongly sample values and detect a danger, so I need to stop the auto mode in order to activate the manual mode, and then deactivate the actuators.

## Setup
More informations about the **setup** of this assignment can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/setup.md).

## Iotlab-St-Lrwan1 code
More informations about the **iotlab-St-Lrwan1 code** of this assignment can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/README.md).

## Web dashboard
More informations about the **dashboard** can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/dashboard/README.md).
