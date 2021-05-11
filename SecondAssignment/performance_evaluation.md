# Performance evaluation

## Introduction
In order to do a performance evaluation, we need to create a **monitoring profile** when creating the experiment on [Iot-lab](https://www.iot-lab.info/). All **results** reported in this file have been obtained by following this [tutorial](https://iot-lab.github.io/docs/tools/consumption-monitoring/), and the whole **experiment** with the **monitoring profile** that are related to these reults are shown in the pictures below:

**Experiment**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/iot-lab-experiment.png)

**Monitoring profile**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/iot-lab-monitoring-profile.png)

As we can see, for the experiment I used the **Saclay** frontend, where I reserved **4 M3-nodes** (**M3-6** as the **border router**, **M3-7**, **M3-8** and **M3-11** as nodes for my **application**).

The monitoring profile will allow us to measure consumption in terms of **power, voltage and current**, and the performance of the network in terms of **throughput** and **end-to-end latency**. In particular, the metrics used are the following:

- **Power consumption: Watt (W)**
- **Voltage consumption: Volt (V)**
- **Current consumption: Ampere (A)**
- **Latency: Received signal strength indication (rssi)**

## Power, voltage and current consumption

### M3-node: the application
I analyzed the power, voltage and current consumption of the single **M3-nodes** where my application was running. Below I report the results of the **M3-11** node of the Saclay frontend that I was testing as a node for my application:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/m3-node-power_consumption.png)

As we can see, in some points we have some consumption **peaks**, corresponding to the **sampling** of values, while in other points we have some **flat lines**, corresponding to the delay **periods**. Obviously, as the number of wireless node increases, the performance of one of this nodes remain almost **similar**, because they are executing the same code application.

I don't report the plots for the other nodes of the application because they are very similar.

### M3-node: the border router

I analyzed the power, voltage and current consumption of the **border router**. Below I report the results of the **M3-6** node of the Saclay frontend that I was testing as my border router:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/border_router_power_consumption.png)

As we can see, in some points we have some consumption **peaks**, corresponding to the arrival of **sampled values** that the border router has to redirect towards the MQTT-Sn broker, while in other points we have some **flat lines**, corresponding to the periods where the border router is not receiving anything from no one of the nodes. Obviously, as the number of wireless node increases, the performance of the border router tend to get **worse**, because it has to manage more data.

## Throughput and end-to-end latency
I analyzed the throughput and end-to-end latency of the **network**. It is obvious to observe that, as the number of wireless node increases, the **end-to-end latency** and so the **end-to-end delay** increases, as well as the **throughput** decreases, because the border router has more data to manage, and so the delay values really depends of how the border router is internally configured to manage packets arriving.
