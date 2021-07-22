#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "net/loramac.h"
#include "semtech_loramac.h"

#include "msg.h"
#include "shell.h"
#include "thread.h"
#include "xtimer.h"

#define RECV_MSG_QUEUE             (4U)

#define MAIN_QUEUE_SIZE            (8)
#define GAS_SMOKE_DELAY             2
#define TEMP_DELAY                  10

#define TEMP_THRESHOLD_MIN          23
#define TEMP_THRESHOLD_MAX          24

#define PPM_THRESHOLD               65

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static char stackThreadTemp[THREAD_STACKSIZE_DEFAULT];
static char stackThreadGasSmoke[THREAD_STACKSIZE_DEFAULT];


static msg_t _recv_queue[RECV_MSG_QUEUE];
static char _recv_stack[THREAD_STACKSIZE_DEFAULT];

// Mode
char* mode = "auto";   // auto || manual
char* dev_id = "";     // device id

static semtech_loramac_t loramac;

uint8_t* deveui = "";
uint8_t* appeui = "";
uint8_t* appkey = "";

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[] = {0, 2, 1};

static void *_recv(void *arg) {
    msg_init_queue(_recv_queue, RECV_MSG_QUEUE);
    (void)arg;
    while (1) {

        semtech_loramac_recv(&loramac);
        loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
        printf("Data received: %s, port: %d", (char *)loramac.rx_data.payload, loramac.rx_data.port);
        
        char * msg = (char *)loramac.rx_data.payload;
		
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
					
		else{
			
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
			
		}

       
    }
    return NULL;
}

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

char *base64_encode(const char *data, size_t input_length, size_t *output_length){
    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if(encoded_data == NULL) return NULL;

    for(size_t i = 0, j = 0; i < input_length;){
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++){
        encoded_data[*output_length - 1 - i] = '=';
    }
    return encoded_data;
}

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
			
			// Sending to the cloud only if the current situation is not ok 
			if(strcmp(temperature_state, "ALL OK!") != 0){
				
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
			
			}
			
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
			
			// Sending to the cloud only if the current situation is not ok 
			if(strcmp(gas_smoke_state, "ALL OK!") != 0){
				
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
			
			}
		
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

static const shell_command_t shell_commands[] = {
	{ "start", "initialize LoraWAN and starts sending data to AWS", cmd_handler},
    { NULL, NULL, NULL}
};

int main(void){
	
	msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
	
    return 0;
}