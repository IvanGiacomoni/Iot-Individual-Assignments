# Iotlab-m3 code

## Introduction
The base code is that of the RIOT [gnrc_networking example](https://github.com/RIOT-OS/RIOT/blob/master/examples/gnrc_networking/main.c).

## Shell

### Shell initialization
The application starts by initializing a **message queue** for the thread running the shell, and then runs the shell, that will be useful for launching the application on the various M3-nodes:

```c
msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

char line_buf[SHELL_DEFAULT_BUFSIZE];
shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
```

In order to do this, we need to include these **headers** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#include "shell.h"
#include "msg.h"
```

Then we need to define the **message queue** together with its **size** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
```

Then, we need to define the **shell_commands** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
static const shell_command_t shell_commands[] = {
    { "start", "connect to the RSMB broker and starts sending data to AWS", cmd_handler},
    { NULL, NULL, NULL }
};
```
### cmd_handler
As we can see, we only have one command, that is **start**, which is responsible of starting the application by invoking the *cmd_handler* function, that I report below:

```c
int cmd_handler(int argc, char **argv){
    (void)argc;
    (void)argv;
    
    char* server_addr = argv[1];
    
    // SERVER_PORT
    
    int server_port = SERVER_PORT;
    
    char* device_id = argv[2];
    
    // Saving device_id into global variable
    device_idd = device_id;
    
    char* mqtt_topic_temp = getTopic(MQTT_TOPIC_TEMP, device_id);
    char* mqtt_topic_gas_smoke = getTopic(MQTT_TOPIC_GAS_SMOKE, device_id);
    char* switch_mode = getTopic(SWITCH_MODE, device_id);
    char* manage_actuators = getTopic(MANAGE_ACTUATORS, device_id);
    
    // Saving topics into global variables
    topic_temp = mqtt_topic_temp;
    topic_gas_smoke = mqtt_topic_gas_smoke;
    topic_switch_mode = switch_mode;
    topic_manage_actuators = manage_actuators;
    
    // Setup mqtt -> connecting to the RSMB broker
    setup_mqtt(server_addr, server_port);
    
    // Setup subscription to MQTT_TOPIC_TEMP
    mqttSubscribeTo(mqtt_topic_temp, 0);
	
	  // Setup subscription to MQTT_TOPIC_GAS_SMOKE
	  mqttSubscribeTo(mqtt_topic_gas_smoke, 1);
	
	  // Setup subscription to SWITCH_MODE
	  mqttSubscribeTo(switch_mode, 2);
	
	  // Setup subscription to MANAGE_ACTUATORS
	  mqttSubscribeTo(manage_actuators, 3);
	
	  // Creating temperature thread 
	  thread_create(stackThreadTemp, sizeof(stackThreadTemp), THREAD_PRIORITY_MAIN - 1, 0, 
		  threadTemp, NULL, "Thread temperature");
	
	  // Creating gas/smoke thread 
	  thread_create(stackThreadGasSmoke, sizeof(stackThreadGasSmoke), THREAD_PRIORITY_MAIN - 1, 0, 
		  threadGasSmoke, NULL, "Thread gas/smoke");	
		
    return 0;
}
```

The cmd_handler takes as input the **number of arguments** of the program and the **effective arguments**. The arguments are only 2:
- the **MQTT-SN broker address**, because it changes every time the Iot-lab experiment finishes
- the **device id of the M3-node running the application**: this has to be unique for each M3-node, because we need to have unique **MQTT topics** as well as a unique **MQTT id** for the communication with the MQTT-SN broker.

The **port** for the MQTT communication with the MQTT-SN broker is stored locally in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#define SERVER_PORT                 1885
```

### MQTT topics
Topics are evaluated by the *getTopic* function, which simply concatenates the **base of the topic** with the device id. So, in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/Makefile), we need to define the **base** fo all **topics**:

```c
MQTT_TOPIC_TEMP = temperature/device/
MQTT_TOPIC_GAS_SMOKE = gas_smoke/device/
SWITCH_MODE = switchMode/device/
MANAGE_ACTUATORS = manageActuators/device/

CFLAGS += -DMQTT_TOPIC_TEMP='"$(MQTT_TOPIC_TEMP)"'
CFLAGS += -DMQTT_TOPIC_GAS_SMOKE='"$(MQTT_TOPIC_GAS_SMOKE)"'
CFLAGS += -DSWITCH_MODE='"$(SWITCH_MODE)"'
CFLAGS += -DMANAGE_ACTUATORS='"$(MANAGE_ACTUATORS)"'
```

Below I report the *getTopic* function:

```c
char* getTopic(char* typeTopic, char* device_idd){
    
  char id_device[1];
	strcpy(id_device, device_idd);
	
	char t[50];
	
	char tmp[64] = "";
	strcat(tmp, typeTopic);
	strcat(tmp, id_device);
	
	strcpy(t, tmp);
	
  char* topic = (char*)malloc(strlen(t)+1);
  strcpy(topic, t);
	
	return topic;
}
```

The built topics are also saved in some global variables defined in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
char* topic_temp = "";
char* topic_gas_smoke = "";
char* topic_switch_mode = "";
char* topic_manage_actuators = "";
```

### MQTT connection setup
At this point, we need to setup the MQTT connection with the MQTT-SN broker, and we do this by using the *setup_mqtt* function, that takes as input the **MQTT-SN broker address** and the **server port**. For more details you can check the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c) and also the RIOT [emcute_mqttsn](https://github.com/RIOT-OS/RIOT/tree/master/examples/emcute_mqttsn) example. I only want to show the *emcute_thread* function, because here we need to build the unique **MQTT id** for the communication with the MQTT-SN broker by using the *getEmcuteId* function, that simply concatenates the **base** of the **MQTT id** with the device id:

```c
// Emcute thread
static void *emcute_thread(void *arg){
	printf("EMCUTE THREAD\n");
	
    (void)arg;
    
    char* mqtt_id_base = EMCUTE_ID_BASE;
    
    char* emcute_id = getEmcuteId(mqtt_id_base, device_idd);
    
    printf("emcute_id = %s\n",emcute_id);
    
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, emcute_id);
    return NULL;    // should never be reached 
}
```

The **base** of the mqtt_id needs to be defined in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#define EMCUTE_ID_BASE              "MQTT_ID_"
```

Below I show the *getEmcuteId* function:

```c
char* getEmcuteId(char* mqtt_id_base, char* device_idd){
    char id_device[1];
	strcpy(id_device, device_idd);
	
	char t[50];
	
	char tmp[64] = "";
	strcat(tmp, mqtt_id_base);
	strcat(tmp, id_device);
	
	strcpy(t, tmp);
	
    char* emcute_id = (char*)malloc(strlen(t)+1);
    strcpy(emcute_id, t);
	
	return emcute_id;
}
```

### MQTT topics' subscriptions
At this point we need to **subscribe** to our **4 main topics**, by using the *mqttSubscribeTo* function, that takes as input the name of the topic and the position of the global array of subscriptions where we will save all info about the new subscription. We need to define the **subscriptions array** as well as the **number of subscriptions** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#define NUMOFSUBS           (4U)
static emcute_sub_t subscriptions[NUMOFSUBS];
```

Below I report the *mqttSubscribeTo* function:

```c
// Subsribes to a topic
void mqttSubscribeTo(char* topic_name, int pos_into_subscriptions){
	  unsigned flags = EMCUTE_QOS_0;
    subscriptions[pos_into_subscriptions].cb = on_pub;
    strcpy(topics[pos_into_subscriptions], topic_name);
    subscriptions[pos_into_subscriptions].topic.name = topic_name;

    if (emcute_sub(&subscriptions[pos_into_subscriptions], flags) != EMCUTE_OK) {
        printf("error: unable to subscribe to %s\n", topic_name);
        exit(EXIT_FAILURE);
    }

    printf("Now subscribed to %s\n", topic_name);
    xtimer_sleep(1);
	  printf("\n");
}
```

### Threads' creation
Finally, I create **two threads**, one for managing the sampling from the MQ-2 sensor and the other one for sampling from the DHT-22 sensor: the main reason is given by the different sampling periods, so with threads we are able to manage both samplings in parallel. If the actual **mode** of the system is set on "auto", then both threads will start sampling once they are created.

In order to use threads, we need to include this **header** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#include "thread.h"
```

Also, we need to define the mode in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
// Mode
char* mode = "auto";   // auto || manual
```

