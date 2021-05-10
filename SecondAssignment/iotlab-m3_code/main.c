/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "thread.h"
#include "shell.h"
#include "msg.h"
#include "xtimer.h"
#include "net/emcute.h"

#define MAIN_QUEUE_SIZE     (8)

#define NUMOFSUBS           (4U)
#define TOPIC_MAXLEN        (64U)

#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define EMCUTE_ID_BASE              "MQTT_ID_"

#define SERVER_PORT                 1885

#define GAS_SMOKE_DELAY             2
#define TEMP_DELAY                  10

#define TEMP_THRESHOLD_MIN          23
#define TEMP_THRESHOLD_MAX          24

#define PPM_THRESHOLD               65

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static char stack[THREAD_STACKSIZE_DEFAULT];
static char stackThreadTemp[THREAD_STACKSIZE_DEFAULT];
static char stackThreadGasSmoke[THREAD_STACKSIZE_DEFAULT];

static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];

// Mode
char* mode = "auto";   // auto || manual

// Topics variables after taking device_id
char* topic_temp = "";
char* topic_gas_smoke = "";
char* topic_switch_mode = "";
char* topic_manage_actuators = "";

char* device_idd = "";

// Callback function for MQTT publishing 
static void on_pub(const emcute_topic_t *topic, void *data, size_t len){
	
	fflush(stdout);
	
	char *in = (char *)data;

	printf("### got publication for topic '%s' [%i] ###\n", topic->name, (int)topic->id);
   
   
    // Switch mode
    if(strcmp(topic->name, topic_switch_mode) == 0){
		
		fflush(stdout);
		
		printf("received: %s\n",in);
		
		char* rcv_mode = strtok(in, ";");
		printf("rcv_mode: %s\n", rcv_mode);
		
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
		fflush(stdout);
	}
	
	// Control state of actuators
	else if(strcmp(topic->name, topic_manage_actuators) == 0){
		
		fflush(stdout);
		
		printf("received: %s\n",in);
		
		char* actuator = strtok(in, ";");
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
		
		else if(strcmp(typeData, "gas_smoke") == 0){
			
			if(strcmp(actuator, "gas_smoke_buzzer") == 0){
				
				if(strcmp(operation, "ON") == 0){
					printf("[Device %s] Turning ON gas/smoke buzzer...\n",device_id);
				}
				
				else if(strcmp(operation, "OFF") == 0){
					printf("[Device %s] Turning OFF gas/smoke buzzer...\n",device_id);
				}
				
			}
			
			else if(strcmp(actuator, "blue_led") == 0){
				
				if(strcmp(operation, "ON") == 0){
					printf("[Device %s] Turning ON blue led...\n",device_id);
				}
				
				else if(strcmp(operation, "OFF") == 0){
					printf("[Device %s] Turning OFF blue led...\n",device_id);
				}
				
			}
			
		}
		
		
		printf("\n");
		fflush(stdout);
	}
	
	else{
		fflush(stdout);
		
		for (size_t i = 0; i < len; i++) {
			printf("%c", in[i]);
		}
		
		puts("");
	}

}


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


int setup_mqtt(char* server_addr, int server_port){
    
    // initialize our subscription buffers 
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    // start the emcute thread 
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");             
                  
    // connect to MQTT-SN broker
    printf("Connecting to MQTT-SN broker %s port %d.\n",
           server_addr, server_port);

    sock_udp_ep_t gw = { .family = AF_INET6, .port = server_port };
    
    char *message = "connected";
    size_t len = strlen(message);

    // parse address 
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, server_addr) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (emcute_con(&gw, true, NULL, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", server_addr,
               (int)gw.port);
        return 1;
    }

    printf("Successfully connected to gateway at [%s]:%i\n",
           server_addr, (int)gw.port);

    return 1;
}

// Publish data towards AWS
void publishDataForAws(char* data, emcute_topic_t* topic){
	
	if(emcute_pub(topic, data, strlen(data), EMCUTE_QOS_0) != EMCUTE_OK){
		printf("error: unable to publish to %s\n", topic->name);
		exit(EXIT_FAILURE);
	} 	
}

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

// Temperature thread
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
			strcat(data, ", \"temperature_state\": \"");
			strcat(data, temperature_state);
			strcat(data, "\"");
			strcat(data, " }");
        
			// Publishing temperature data to MQTT_TOPIC_TEMP
			publishDataForAws(data, &subscriptions[0].topic);
			
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


// Gas-smoke thread
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
			strcat(data, ", \"gas_smoke_state\": \"");
			strcat(data, gas_smoke_state);
			strcat(data, "\"");
			strcat(data, " }");
        
			// Publishing temperature data to MQTT_TOPIC_GAS_SMOKE
			publishDataForAws(data, &subscriptions[1].topic);
		
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

static const shell_command_t shell_commands[] = {
    { "start", "connect to the RSMB broker and starts sending data to AWS", cmd_handler},
    { NULL, NULL, NULL }
};



int main(void)
{
    
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
