# Dashboard

## Introduction
The **Web Dashboard** is composed by two parts:

- the **server**, realized with **Node.js**
- the **frontend**, realized with **HTML, Bootstrap, Javascript, Vue.js**

## Overview
The server will be available at ***http://localhost:8080/***, where we can see the **homepage**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/homepage.png)

If we click on *Temperature* we will see the **temperature values received during the last hour** from the DHT-22 sensor for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/temp_last_hour_values.png)

By clicking on *Last values*, we will see the **latest values** received from the DHT-22 sensor for each device.

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/temp_latest_values.png)

By clicking on *Back* and then on *Last hour aggregated values*, we will see the **aggregated values (average, minimum and maximum)** for the DHT-22 sensor **retrieved during the last hour** for each device.

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/temp_aggregated.png)

All this could be seen also for **gas and smoke values** retrieved from the MQ-2 sensor. So below I show the **last hour values** for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/gas_smoke_last_hour_values.png)

Below we can see the **latest values** for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/gas_smoke_latest_values.png)

Below we can see the **aggregated values (average, minimum and maximum) retrieved during the last hour** for each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/gas_smoke_aggregated.png)

If we go back to last hour temperature values, by clicking on *Actuators* we have the possibility to **switch mode** for the application **(auto and manual)**, and also to **manage** the red led and the temperature buzzer, for each device. If we consider device 1, by clicking on *Manual*, we will get this message on the website:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/switchToManual.png)

On AWS we will get this message under the **switchMode/device/1 topic**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/switchMode_topic_aws.png)

Notice that we get an error because we are sending a normal string message, but we can ignore it.
Also, on the console we can see these messages:

```
### got publication for topic 'switchMode/device/1' [3] ###
received: manual;1
rcv_mode: manual
device_id: 1
[Device 1] Switching to manual...

[GAS/SMOKE] mode: manual
[GAS/SMOKE] Now in manual mode

[TEMPERATURE] mode: manual
[TEMPERATURE] Now in manual mode
```

At this point, if we click, for example, on *Red led ON*, we get this message on the webpage:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/red_led_ON.png)

On AWS we will get this message under the **manageActuators/device/1** topic, and we got an error for the same reason as before:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/images/manageActuators_topic_aws.png)

Also, on the console we can see these messages:

```
### got publication for topic 'manageActuators/device/1' [4] ###
received: red_led;temperature;ON;1
actuator: red_led
typeData: temperature
operation: ON
device_id: 1
```

The same logic is applied for gas/smoke actuators.

## Code
More informations about the code can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/dashboard/dashboard_code.md)
