# Setup

## The RIOT operating system
We need to install the **RIOT operating system**, [here](https://github.com/RIOT-OS/Tutorials#tutorials-for-riot) you can find a tutorial for the installation.

## Installing NodeJS and preparing the server
You need to install **NodeJS** in your operating system in order to have the web dashboard working correctly, [here](https://nodejs.org/en/) you can find all the details. After this, you have to check if you have **npm** installed, otherwise you can install it from [here](https://www.npmjs.com/get-npm). You also need to require the **.env** file, that has to be placed in the dashboard directory. More info about the .env file will come later.

## Nodemon (not mandatory)
Now you can install **nodemon**, a very useful package to run our NodeJS server, with this command: **npm install -g nodemon** if you are on Windows, or **sudo npm install -g nodemon** if you are on Linux/Ubuntu.

## AWS, Iot-Core and DynamoDB
Then, you need so sign in [AWS](https://aws.amazon.com/education/awseducate/), in order to get access to **Iot-Core** and **DynamoDB** services. Once you're on Iot-Core, you need to create a **thing** in order to get your **certificate**, **root certificate** and **private key**. Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html). Then you need to add into the **.env** file the **access key id**, the **secret access key** and the **session token** provided by AWS, to be able to receive messages from the devices. At this point the .env file should look like this:

```
AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
AWS_SESSION_TOKEN=YOUR_AWS_SESSION_TOKEN
```

Then by going in the Rules section you have to create two **rules**, one for temperature data and one for gas and smoke data, specifying as topics temperature/device/<device_id> and gas_smoke/device/<device_id> respectively, and as operation you need to choose "Insert a message into a DynamoDB table". Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html).

**Temperature rule:**

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/images/temperature_rule.png)

**Gas/Smoke rule:**

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/images/gas_smoke_rule.png)

So it is clear that we will create **two DynamoDb tables**, one for each rule: particularly, each entry of both tables is made up of the **Unix timestamp** associated to the creation of that entry, that will be useful for quering the last hour values of temperature and gas/smoke, the **device_id** of the device who has sampled that value, and the **device_data**, which consists of the **value** sampled and the **state** associated to that value. 

## TheThingsNetwork
Now we have to setup an **application** on [TheThingsNetwork](https://www.thethingsnetwork.org/), and after that we have to associate at least one **end device** to that application, and we have to make sure that all settings are correcly set, in particular:
- **Activation mode -> Over The Air Activation (OTAA)**
- **LoraWAN version -> MAC V1.0.2**
- **Frequency plan -> Europe 863-870MHz (SF9 for RX2)**
- **Regional Parameters Version -> PHY V1.0.2 REV B**

Then we have to integrate our application with **AWS** in order to store our data into **DynamoDB**, and you can check this [tutorial](https://www.thethingsindustries.com/docs/integrations/cloud-integrations/aws-iot/default/). Finally, you can check this [tutorial](https://www.thethingsindustries.com/docs/integrations/cloud-integrations/aws-iot/default/messages/) for the **handling of messages**.

## Configure an SSH access key
Before starting an [Iot-lab](https://www.iot-lab.info/) experiment, you need to configure an SSH access key, and in order to do this you can follow this [tutorial](https://www.iot-lab.info/docs/getting-started/ssh-access/).

## Iot-lab experiment
Then you need to submit an **experiment** on the [Iot-lab testbed](https://www.iot-lab.info/testbed/dashboard) with at least **1 St-Lrwan1-node** on some of the **frontends** offered by Iot-lab: for our purposes, the recommended choice is the **Saclay** frontend. In general, you can also follow this [tutorial](https://www.iot-lab.info/legacy/tutorials/riot-ttn/index.html).
