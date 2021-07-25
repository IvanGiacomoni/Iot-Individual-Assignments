# Iotlab-St-Lrwan1 code

## Introduction
The base code is that of the RIOT [pkg_semtech-loramac test](https://github.com/RIOT-OS/RIOT/tree/master/tests/pkg_semtech-loramac).

## Shell

### Shell initialization
The application starts by initializing a **message queue** for the thread running the shell, and then runs the shell, that will be useful for launching the application on the various St-Lrwan1-nodes:

```c
msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

char line_buf[SHELL_DEFAULT_BUFSIZE];
shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
```

In order to do this, we need to include these **headers** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
#include "shell.h"
#include "msg.h"
```

Also, we need to use these modules in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/Makefile):

```c
USEMODULE += shell
USEMODULE += ps
```

Then we need to define the **message queue** together with its **size** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
```

Then, we need to define the **shell_commands** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
static const shell_command_t shell_commands[] = {
    { "start", "initialize LoraWAN and starts sending data to AWS", cmd_handler},
    { NULL, NULL, NULL}
};
```

### cmd_handler
As we can see, we only have one customized command, that is **start**, which is responsible of starting the application by invoking the *cmd_handler* function, that I report below:

```c
int cmd_handler(int argc, char **argv){
    (void)argc;
    (void)argv;
	
	// Getting device id
	char* device_id = argv[1];
	
	// Getting device eui, app eui and app key 
	uint8_t* dev_eui = argv[2];
    uint8_t* app_eui = argv[3];
    uint8_t* app_key = argv[4];
	
	// Assigning input variables to global variables
	dev_id = device_id;
	deveui = dev_eui;
	appeui = app_eui;
	appkey = app_key;
	
	printf("Initializing Lora...");
    lora_init();
    xtimer_sleep(2);
	printf("done!\n");
	
	// Creating temperature thread 
	thread_create(stackThreadTemp, sizeof(stackThreadTemp), THREAD_PRIORITY_MAIN - 1, 0, 
		threadTemp, NULL, "Thread temperature");
	
	// Creating gas/smoke thread 
	thread_create(stackThreadGasSmoke, sizeof(stackThreadGasSmoke), THREAD_PRIORITY_MAIN - 1, 0, 
		threadGasSmoke, NULL, "Thread gas/smoke");	
		
    return 0;
}
```

The cmd_handler takes as input the **number of arguments** of the program and the **effective arguments**. The arguments are 4:
- the **device id of the St-Lrwan1-node running the application**: this has to be unique for each St-Lrwan1-node
- the **dev eui**: associated to the device
- the **app eui**: associated to the application
- the **app key**: associated to the application

Then I store these informations in some global variables, that we need to define in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
char* dev_id = "";     // device id
uint8_t* deveui = "";
uint8_t* appeui = "";
uint8_t* appkey = "";
```

### Initializing Lora
Then we need to initialize Lora by using the *lora_init* function, and I report it below:

```c
int lora_init(void){
    printf("dev id: %s\n", dev_id);

    semtech_loramac_init(&loramac);
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    semtech_loramac_set_dr(&loramac, 5);

    if(semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED){
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");

    puts("Starting recv thread");
    thread_create(_recv_stack, sizeof(_recv_stack), THREAD_PRIORITY_MAIN - 1, 0, _recv, NULL, "recv thread");
    return 0;
}
```

In particular, we can see the creation of the thread responsible for **receiving** data by LoraWAN, which invokes the *_recv* function, which we will discuss later. More details can be found in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c).

### Threads' creation
Finally, I create **two threads**, one for managing the sampling of **temperature** data and the other one for sampling **gas/smoke** data: the main reason is given by the different sampling periods, so with threads we are able to manage both samplings in parallel. If the actual **mode** of the system is set on "auto", then both threads will start sampling once they are created.

In order to use threads, we need to include this **header** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
#include "thread.h"
```

Also, we need to define the mode in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
// Mode
char* mode = "auto";   // auto || manual
```

## Definition of sampling rates
Given that we need to do **periodical sampling**, we need to use the **xtimer module**. So we need to include it in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/Makefile):

```c
USEMODULE += xtimer
```

Also, we need to include this **header** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#include "xtimer.h"
```

The **sampling period** will be different for the two type of data, so we define **two different constants** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/ThirdAssignment/iotlab-St-Lrwan1_code/main.c):

```c
#define GAS_SMOKE_DELAY             2
#define TEMP_DELAY                  10
```

## Data sampling
The logic of the application has not changed from the first assignment, so for all details you can check the [first assignment](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/FirstAssignment/nucleo_code/README.md). The main differences are the following:
- **there aren't actuators**, so they are 'managed' locally and remotely from the dashboard with some **debug prints**
- **there aren't sensors**, so the value are generated **randomly** with the **generateRandomTemperature** and the **generateRandomPpm** functions

Below I report the two threads:

**Temperature thread**:

```c
static void* threadTemp(void *arg){
	(void)arg;
	
	printf("THREAD TEMPERATURE\n");
	
	srand(time(NULL));
    
    char* temperature_state = "";
    
    // Periodic sampling of temperature
    while(1){
		
		printf("[TEMPERATURE] mode: %s\n", mode);
		
		if(strcmp(mode, "auto") == 0){
			
			int rand_state_temp = rand() % 3;
			
			// Reading temperature values by DHT-22 sensor
			int temperature = generateRandomTemperature(rand_state_temp);
        
			if(temperature < 0){
				continue;
			}
	  
			printf("temperature: %d°C\n", temperature);
		
			// Preprocessing data
		
			if (temperature > TEMP_THRESHOLD_MAX){
				printf("[TEMPERATURE] WARNING!!\n");
				temperature_state = "WARNING!";
			}
		
			else if (temperature > TEMP_THRESHOLD_MIN && temperature <= TEMP_THRESHOLD_MAX){
				printf("[TEMPERATURE] BE CAREFUL TO THE TEMPERATURE!\n");
				temperature_state = "GROWING!";
			}
		
			else if(temperature <= TEMP_THRESHOLD_MIN){
				printf("[TEMPERATURE] ALL OK!\n");
				temperature_state = "ALL OK!";
			}
				
			// Formatting all data into a string for mqtt publishing
			char temp_s[5];
			sprintf(temp_s, "%d", temperature);
		
			char data[64] = "{ ";
			strcat(data, "\"temperature\": ");
			strcat(data, temp_s);
			strcat(data, ", \"device_id\": \"");
			strcat(data, dev_id);
			strcat(data, ", \"temperature_state\": \"");
			strcat(data, temperature_state);
			strcat(data, "\"");
			strcat(data, " }");
        
			// Publishing temperature data towards AWS
			send(data);
			
			xtimer_sleep(TEMP_DELAY);
			printf("\n");
		}
		
		else{
			printf("[TEMPERATURE] Now in manual mode\n");
			xtimer_sleep(5);
		}
		
	}
    
    return NULL;    
}
```

**Gas/smoke thread**:

```c
static void* threadGasSmoke(void* arg){
    (void)arg;
    
    printf("THREAD GAS SMOKE\n");
    
    srand(time(NULL));
    
    char* gas_smoke_state = "";
    
    // Periodical sampling of ppm
    while(1){
		
		printf("[GAS/SMOKE] mode: %s\n", mode);
		
		if(strcmp(mode, "auto") == 0){
			
			int rand_state_gas_smoke = rand() % 2;
			
			// Reading ppm values by MQ-2 sensor
			int ppm = generateRandomPpm(rand_state_gas_smoke);
			printf("ppm: %d\n", ppm);
		
			// Preprocessing data
			if(ppm > PPM_THRESHOLD){
				printf("[GAS/SMOKE] WARNING!!!\n");
				gas_smoke_state = "WARNING!";
			}
		
			else if(ppm <= PPM_THRESHOLD){
				printf("[GAS/SMOKE] ALL OK!\n");
				gas_smoke_state = "ALL_OK!";
			}
				
			// Formatting data into a string for mqtt publishing
			char ppm_s[5];
			sprintf(ppm_s, "%d", ppm);
		
			char data[64] = "{ ";
			strcat(data, "\"ppm\": ");
			strcat(data, ppm_s);
			strcat(data, ", \"device_id\": \"");
			strcat(data, dev_id);
			strcat(data, ", \"gas_smoke_state\": \"");
			strcat(data, gas_smoke_state);
			strcat(data, "\"");
			strcat(data, " }");
        
			// Publishing gas/smoke data towards AWS
			send(data);
		
			xtimer_sleep(GAS_SMOKE_DELAY);
			printf("\n");
		}
		
		else{
			printf("[GAS/SMOKE] Now in manual mode\n");
			xtimer_sleep(5);
		}
	}
    
    return NULL;    
}
```

In order to generate **random** data we need to include this **header** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/SecondAssignment/iotlab-m3_code/main.c):

```c
#include <time.h>
```

Below I report the *generateRandomTemperature* and the *generateRandomPpm* functions:

```c
int generateRandomTemperature(int rand_state_temp){
    
    int rand_temp = 0;
    
    if(rand_state_temp == 0){
	    rand_temp = rand() % TEMP_THRESHOLD_MIN + 1;	
	}
	
	else if(rand_state_temp == 1){
	    rand_temp = (rand() % (TEMP_THRESHOLD_MAX - (TEMP_THRESHOLD_MIN+1) + 1)) + TEMP_THRESHOLD_MIN+1;	
	}
	
	else if(rand_state_temp == 2){
	    rand_temp = (rand() % ((TEMP_THRESHOLD_MAX + 6) - (TEMP_THRESHOLD_MAX+1) + 1)) + TEMP_THRESHOLD_MAX+1;	
	}
    
    return rand_temp;
    
}
```

```c
int generateRandomPpm(int rand_state_gas_smoke){
	
	int rand_ppm = 0;
    
    if(rand_state_gas_smoke == 0){
	    rand_ppm = rand() % PPM_THRESHOLD + 1;	
	}
	
	else if(rand_state_gas_smoke == 1){
	    rand_ppm = (rand() % ((PPM_THRESHOLD + 6) - (PPM_THRESHOLD+1) + 1)) + PPM_THRESHOLD+1;	
	}
    
    return rand_ppm;
	
}
```

As in the previous assignments, data are sent to **AWS**. Obviously, data are formatted in **JSON**, in order to make the things work for storing them into **DynamoDB**, but this time we will use **LoraWAN**, in order to first send the data to [TheThingsNetwork](https://www.thethingsnetwork.org/), and then to **AWS** (by **MQTT**) by using the integration available on [TheThingsNetwork](https://www.thethingsnetwork.org/). A difference is also that I send the **device id** inserted into the shell, because given that we will not use MQTT at the Edge, we can't do an immediate assignment of the device id on **Iot Core**. All this is achieved with the *send* function that I report below:

```c
void send(const char* message){
    printf("Sending message '%s'\n", message);

    size_t inl = strlen(message);
    size_t output;

    char *encoded = base64_encode(message, inl, &output);
   
    uint8_t status = semtech_loramac_send(&loramac, (uint8_t *)message, strlen(message));
  
    if (status != SEMTECH_LORAMAC_TX_DONE) {
        printf("Cannot send message '%s'\n", message);
    } else {
        printf("Successfully sent message: %s\n", message);
    }
}
```

## Switching mode (auto and manual)
When the user sends through the **frontend** the indication to **switch mode**, an indication is sent and then received by our device by **LoraWAN** through the *_recv* function. If we have to switch to **manual** mode, the periodical sampling is blocked and we are ready to receive indications related to actuators, otherwise if we are switching to **auto**, the periodical sampling is resumed. Below I report the code snippet related to the **switching mode management**:

```c
printf("received: %s\n",msg);
		
		char* firstWord = strtok(msg, ";");
		
		// Check if I have to switch mode 
		if(strcmp(firstWord,"manual")==0 || strcmp(firstWord,"auto")==0){
			
			char* rcv_mode = firstWord;
			
			printf("rcv_mode: %s\n", firstWord);
			
			char *device_id = strtok(NULL, ";");
		    printf("device_id: %s\n",device_id);
			
			// Manual
			if(strcmp(rcv_mode, "manual") == 0){
				if(strcmp(mode, "auto") == 0){
					printf("[Device %s] Switching to manual...\n",device_id);
					xtimer_sleep(2);
				
					mode = "manual";
				}
				else if(strcmp(mode, "manual") == 0){
					printf("[Device %s] Already in manual mode!\n", device_id);
				}
			}
		
			// Auto
			else if(strcmp(rcv_mode, "auto") == 0){
			
				if(strcmp(mode, "manual") == 0){
					printf("[Device %s] Switching to auto...\n",device_id);
				
					xtimer_sleep(2);
				
					mode = "auto";
				
				}
				else if(strcmp(mode, "auto") == 0){
					printf("[Device %s] Already in auto mode!\n",device_id);
				}
			}
			printf("\n");
			
		}
```

## Controlling the state of actuators
In order to control the state of actuators, we can receive the indication from the **frontend** by **LoraWAN** through the *_recv* function. What we are going to receive is the **name of the actuator**, the **type of data** it is related to (temperature or gas/smoke) and the **action** we need to do, so ON or OFF. Particularly, the idea is that of only controlling buzzers, the red led and the blue led, because these are the actuators related to dangerous situations. Below I only report the part of code related to temperature, because the remaining part is very similar:

```c
printf("received: %s\n",msg);
		
			char* actuator = strtok(msg, ";");
			printf("actuator: %s\n",actuator);
		
			char *typeData = strtok(NULL, ";");
			printf("typeData: %s\n",typeData);
		
			char *operation = strtok(NULL, ";");
			printf("operation: %s\n", operation);
		
			char *device_id = strtok(NULL, ";");
			printf("device_id: %s\n",device_id);
		
			if(strcmp(typeData, "temperature") == 0){
				
				if(strcmp(actuator, "temp_buzzer") == 0){
				
					if(strcmp(operation, "ON") == 0){
						printf("[Device %s] Turning ON temperature buzzer...\n",device_id);
					}
				
					else if(strcmp(operation, "OFF") == 0){
						printf("[Device %s] Turning OFF temperature buzzer...\n",device_id);
					}
			
				}
			
				else if(strcmp(actuator, "red_led") == 0){
				
					if(strcmp(operation, "ON") == 0){
						printf("[Device %s] Turning ON red led...\n",device_id);
					}
				
					else if(strcmp(operation, "OFF") == 0){
						printf("[Device %s] Turning OFF red led...\n",device_id);
					} 
			
				}
			}
```
