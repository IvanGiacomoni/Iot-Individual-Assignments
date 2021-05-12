# Dashboard

## Introduction
The **Web Dashboard** is composed by two parts:

- the **server**, realized with **Node.js**
- the **frontend**, realized with **HTML, Bootstrap, Javascript, Vue.js**

## Overview
The server will be available at ***http://localhost:8080/***, where we can see the **homepage**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/homepage.png)

If we click on *Temperature* we will see the **temperature values received during the last hour from all environmental stations**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/temperature_last_hour.png)

By clicking on *Last values*, we will see the **latest temperature values** received from each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/temperature_latest.png)

By clicking on *Back* and then on *Last hour aggregated values*, we will see the **temperature aggregated values (average, minimum and maximum) retrieved during the last hour from each device**, and **temperature aggregated values (average, minimum and maximum) retrieved from all sensor devices during the last hour**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/temperature_aggregated_values.png)

All this could be seen also for **gas and smoke values**. So below I show the **gas and smoke values received during the last hour from all environmental stations**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/gas_smoke_last_hour.png)

Below we can see the **gas and smoke latest values** received from each device:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/gas_smoke_latest.png)

Below we can see the **gas and smoke aggregated values (average, minimum and maximum) retrieved during the last hour from each device**, and **gas and smoke aggregated values (average, minimum and maximum) retrieved from all sensor devices during the last hour**:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/gas_smoke_aggregated_values.png)

If we go back to last hour temperature values, by clicking on *Actuators* we have the possibility to **switch mode** for the application **(auto and manual)**, and also to **manage** the red led and the temperature buzzer, for each device. If we consider device 1, by clicking on *Manual*, we will get this message on the website:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/switchToManual.png)

On the console of device_1 we can see these messages:

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

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/images/red_led_ON.png)

On the console of device_1 we can see these messages:

```
### got publication for topic 'manageActuators/device/1' [4] ###
[Device 1] received: red_led;temperature;ON;1
actuator: red_led
typeData: temperature
operation: ON
device_id: 1
[Device 1] Turning ON red led...
```

The same logic is applied for gas/smoke actuators.

## Code
More informations about the code can be found [here](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/dashboard/dashboard_code.md).
