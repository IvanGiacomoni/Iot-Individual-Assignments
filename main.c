#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net/emcute.h"

#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "analog_util.h"

#include "fmt.h"
#include "dht.h"
#include "dht_params.h"

#define ADC_IN_USE                  ADC_LINE(0)
#define ADC_RES                     ADC_RES_10BIT

#define GAS_SMOKE_DELAY             2
#define TEMP_DELAY                  10

#define TEMP_THRESHOLD_MIN          23
#define TEMP_THRESHOLD_MAX          24

#define PPM_THRESHOLD               54

#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (4U)
#define TOPIC_MAXLEN        (64U)

#ifndef EMCUTE_ID
#define EMCUTE_ID           ("MY_ID")
#endif

static char stack[THREAD_STACKSIZE_DEFAULT];

static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];

// GLOBAL VARIABLES

// DHT-22 sensor
dht_params_t params;
dht_t dev;

// Leds
gpio_t red_led, green_led, yellow_led, blue_led, white_led;

// Buzzers
gpio_t gas_smoke_buzzer, temp_buzzer;

// Mode
char* mode = "auto";


void led_ON(gpio_t led){
	gpio_set(led);
}

void led_OFF(gpio_t led){
	gpio_clear(led);
}

void buzzer_ON(gpio_t buzzer){
	gpio_set(buzzer);
}

void buzzer_OFF(gpio_t buzzer){
	gpio_clear(buzzer);
}

static void *emcute_thread(void *arg){
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

static void on_pub(const emcute_topic_t *topic, void *data, size_t len){
	
	fflush(stdout);
	
	char *in = (char *)data;

	printf("### got publication for topic '%s' [%i] ###\n", topic->name, (int)topic->id);
    
    if(strcmp(topic->name, SWITCH_MODE_DEVICE_1) == 0){
		
		fflush(stdout);
		
		printf("received: %s\n",in);
		
		if(strcmp(in, "manual") == 0){
			if(strcmp(mode, "auto") == 0){
				printf("Switching to manual...\n");
				xtimer_sleep(2);
				
				mode = "manual";
			}
			else if(strcmp(mode, "manual") == 0){
				printf("Already in manual mode!\n");
			}
		}
		
		else if(strcmp(in, "auto") == 0){
			
			if(strcmp(mode, "manual") == 0){
				printf("Switching to auto...\n");
				
				led_OFF(red_led);
				led_OFF(green_led);
				led_OFF(yellow_led);
				led_OFF(white_led);
				led_OFF(blue_led);
				buzzer_OFF(temp_buzzer);
				buzzer_OFF(gas_smoke_buzzer);
				
				xtimer_sleep(2);
				
				mode = "auto";
				
			}
			else if(strcmp(mode, "auto") == 0){
				printf("Already in auto mode!\n");
			}
		}
		printf("\n");
		fflush(stdout);
	}
	
	else if(strcmp(topic->name, MANAGE_ACTUATORS_DEVICE_1) == 0){
		
		fflush(stdout);
		
		printf("received: %s\n",in);
		
		char* actuator = strtok(in, ";");
		printf("actuator: %s\n",actuator);
		
		char *typeData = strtok(NULL, ";");
		printf("typeData: %s\n",typeData);
		
		char *operation = strtok(NULL, ";");
		printf("operation: %s\n", operation);
		
		if(strcmp(typeData, "temperature") == 0){
			
			if(strcmp(actuator, "temp_buzzer") == 0){
				
				if(strcmp(operation, "ON") == 0){
					buzzer_ON(temp_buzzer);
				}
				
				else if(strcmp(operation, "OFF") == 0){
					buzzer_OFF(temp_buzzer);
				}
			
			}
			
			else if(strcmp(actuator, "red_led") == 0){
				
				if(strcmp(operation, "ON") == 0){
					led_ON(red_led);
				}
				
				else if(strcmp(operation, "OFF") == 0){
					led_OFF(red_led);
				} 
			
			}
		}
		
		else if(strcmp(typeData, "gas_smoke") == 0){
			
			if(strcmp(actuator, "gas_smoke_buzzer") == 0){
				
				if(strcmp(operation, "ON") == 0){
					buzzer_ON(gas_smoke_buzzer);
				}
				
				else if(strcmp(operation, "OFF") == 0){
					buzzer_OFF(gas_smoke_buzzer);
				}
				
			}
			
			else if(strcmp(actuator, "blue_led") == 0){
				
				if(strcmp(operation, "ON") == 0){
					led_ON(blue_led);
				}
				
				else if(strcmp(operation, "OFF") == 0){
					led_OFF(blue_led);
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

/**
 * Setup the EMCUTE, open a connection to the MQTT-S broker,
 * subscribe to the default topic and publish a message.
 */
int setup_mqtt(void){
    
    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");
                  
    // connect to MQTT-SN broker
    printf("Connecting to MQTT-SN broker %s port %d.\n",
           SERVER_ADDR, SERVER_PORT);

    sock_udp_ep_t gw = { .family = AF_INET6, .port = SERVER_PORT };
    char *topic = MQTT_TOPIC_TEMP;
    char *message = "connected";
    size_t len = strlen(message);

    /* parse address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, SERVER_ADDR) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", SERVER_ADDR,
               (int)gw.port);
        return 1;
    }

    printf("Successfully connected to gateway at [%s]:%i\n",
           SERVER_ADDR, (int)gw.port);

    return 1;
}

void publishDataForAws(char* data, emcute_topic_t* topic){
	
	if(emcute_pub(topic, data, strlen(data), EMCUTE_QOS_0) != EMCUTE_OK){
		printf("error: unable to publish to %s\n", topic->name);
		exit(EXIT_FAILURE);
	} 	
}

void initializeLeds(void){
	
	// Red led initialization
	red_led = GPIO_PIN(PORT_B, 5);  // PIN D4
		
	if (gpio_init(red_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 5);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Red led ready!\n");
	}
	
	// Green led initialization
	green_led = GPIO_PIN(PORT_A, 8); // PIN D7
		
	if (gpio_init(green_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 8);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Green led ready!\n");
	}
	
	// Yellow led initialization
	yellow_led = GPIO_PIN(PORT_A, 9); // PIN D9
		
	if (gpio_init(yellow_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 9);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Yellow led ready!\n");
	}
	
	// Blue led initialization
	blue_led = GPIO_PIN(PORT_B, 6); // PIN D10
		
	if (gpio_init(blue_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 6);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Blue led ready!\n");
	}
	
	// White led initialization
	white_led = GPIO_PIN(PORT_A, 6); // PIN D12
		
	if (gpio_init(white_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 6);
		exit(EXIT_FAILURE);
	}
	else{
		printf("White led ready!\n");
	}
}

void initializeBuzzers(void){
	
	// Gas/Smoke buzzer initialization
	gas_smoke_buzzer = GPIO_PIN(PORT_C, 7);  // PIN D9
		
	if (gpio_init(gas_smoke_buzzer, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 7);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Gas/Smoke buzzer ready!\n");
	}
	
	// Temperature buzzer initialization
	temp_buzzer = GPIO_PIN(PORT_A, 5);  // PIN D13
		
	if (gpio_init(temp_buzzer, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 5);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Temperature buzzer ready!\n");
	}
}

void initializeADCLine(void){
	if (adc_init(ADC_IN_USE) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_IN_USE);
        exit(EXIT_FAILURE);
    }
    else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_IN_USE);
    }
}

void initializeDHT(void){
	
	// Initializing DHT_22 parameters
	params.pin = GPIO_PIN(PORT_A, 10);
	params.type = DHT22;
	params.in_mode = DHT_PARAM_PULL;
	
	// Initializating DHT_22 module
	if (dht_init(&dev, &params) == DHT_OK) {
		printf("DHT sensor connected\n");
	}
	
	else {
		printf("Failed to connect to DHT sensor\n");
		exit(EXIT_FAILURE);
	}
}

int readPpmByMQ2(void){
	
	int mq2_sample = adc_sample(ADC_IN_USE, ADC_RES);
    int ppm = adc_util_map(mq2_sample, ADC_RES, 10, 100);

    if (mq2_sample < 0) {
        printf("ADC_LINE(%u): selected resolution not applicable\n", ADC_IN_USE);
        return -1;
    }
    
    return ppm;
}

int readTemperatureByDHT(void){
	int16_t temp, hum;
	if (dht_read(&dev, &temp, &hum) != DHT_OK) {
		printf("Error reading values\n");
		return -1;
	}

	// Formatting the temperature into a string
	char temp_s[10];
	size_t n = fmt_s16_dfp(temp_s, temp, -1);
	temp_s[n] = '\0';
		
	// Converting the temp string into an integer
	int temp_int = atoi(temp_s);
	
	return temp_int;
}

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

static char stackThreadTemp[THREAD_STACKSIZE_DEFAULT];
static char stackThreadGasSmoke[THREAD_STACKSIZE_DEFAULT];

static void* threadTemp(void *arg){
	// using arg in order to avoid compilation errors
	void* args = arg;
	args = NULL;
	
	if(args == NULL){
	    printf("No arguments for thread temp\n");	
	}
	
    printf("I'm temperature thread with pid %d\n", thread_getpid());
    
    char* temperature_state = "";
    
    // Periodic sampling of temperature
    while(1){
		
		printf("[TEMPERATURE] mode: %s\n", mode);
		
		if(strcmp(mode, "auto") == 0){
			// Reading temperature values by DHT-22 sensor
			int temperature = readTemperatureByDHT();
        
			if(temperature < 0){
				continue;
			}
	  
			printf("temperature: %d°C\n", temperature);
		
			// Preprocessing data
		
			if (temperature > TEMP_THRESHOLD_MAX){
				printf("[TEMPERATURE] WARNING!!\n");
				temperature_state = "WARNING!";
			
				led_OFF(green_led);
				led_OFF(yellow_led);
			
				led_ON(red_led);
				//buzzer_ON(temp_buzzer);
			}
		
			else if (temperature > TEMP_THRESHOLD_MIN && temperature <= TEMP_THRESHOLD_MAX){
				printf("[TEMPERATURE] BE CAREFUL TO THE TEMPERATURE!\n");
				temperature_state = "GROWING!";
			
				led_OFF(green_led);
				led_OFF(red_led);
			
				led_ON(yellow_led);
			}
		
			else if(temperature <= TEMP_THRESHOLD_MIN){
				printf("[TEMPERATURE] ALL OK!\n");
				temperature_state = "ALL OK!";
			
				led_OFF(red_led);
				led_OFF(yellow_led);
			
				led_ON(green_led);
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
			buzzer_OFF(temp_buzzer);
			printf("\n");
		}
		
		else{
			printf("[TEMPERATURE] Now in manual mode\n");
			xtimer_sleep(5);
		}
		
	}
    
    return NULL;    
}

static void* threadGasSmoke(void* arg){
    // using arg in order to avoid compilation errors
    void* args = arg;
	args = NULL;
	
	if(args == NULL){
	    printf("No arguments for thread temp\n");	
	}
    
    printf("I'm gas/smoke thread with pid %d\n", thread_getpid());
    
    char* gas_smoke_state = "";
    
    // Periodical sampling of ppm
    while(1){
		
		printf("[GAS/SMOKE] mode: %s\n", mode);
		
		if(strcmp(mode, "auto") == 0){
			// Reading ppm values by MQ-2 sensor
			int ppm = readPpmByMQ2();
			printf("ppm: %d\n", ppm);
		
			// Preprocessing data
			if(ppm > PPM_THRESHOLD){
				printf("[GAS/SMOKE] WARNING!!!\n");
				gas_smoke_state = "WARNING!";
			
				led_ON(blue_led);
				led_OFF(white_led);
				//buzzer_ON(gas_smoke_buzzer);
			}
		
			else if(ppm <= PPM_THRESHOLD){
				printf("[GAS/SMOKE] ALL OK!\n");
				gas_smoke_state = "ALL_OK!";
			
				led_ON(white_led);
				led_OFF(blue_led);
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
			buzzer_OFF(gas_smoke_buzzer);
			printf("\n");
		}
		
		else{
			printf("[GAS/SMOKE] Now in manual mode\n");
			xtimer_sleep(5);
		}
		
		
		
	}
    
    return NULL;    
}

int main(void){
    
    printf("\nStarting the application...\n");
	xtimer_sleep(1);
	
	// Initializing DHT_22 parameters and module

	initializeDHT();
	xtimer_sleep(1);
	printf("\n");

    /* Initializing the ADC line */
    initializeADCLine();
    xtimer_sleep(1);
    printf("\n");
    
    // Initializing leds
	initializeLeds();
	xtimer_sleep(1);
	printf("\n");
	
	// Initializing buzzers
	initializeBuzzers();
    xtimer_sleep(1);
	printf("\n");
	
	// setup MQTT-SN connection
    printf("Setting up MQTT-SN.\n");
    setup_mqtt();
    
    // Setup subscription to MQTT_TOPIC_TEMP
    mqttSubscribeTo(MQTT_TOPIC_TEMP, 0);
	
	// Setup subscription to MQTT_TOPIC_GAS_SMOKE
	mqttSubscribeTo(MQTT_TOPIC_GAS_SMOKE, 1);
	
	// Setup subscription to MQTT_TOPIC_GAS_SMOKE
	mqttSubscribeTo(SWITCH_MODE_DEVICE_1, 2);
	
	// Setup subscription to MANAGE_ACTUATORS_DEVICE_1
	mqttSubscribeTo(MANAGE_ACTUATORS_DEVICE_1, 3);
	
	printf("Creating thread for sampling temperature...\n");
	
	// Creating temperature thread 
	thread_create(stackThreadTemp, sizeof(stackThreadTemp), THREAD_PRIORITY_MAIN - 1, 0, 
		threadTemp, NULL, "Thread temperature");
	printf("Temperature thread created...\n");
	
	printf("Creating thread for sampling ppm...\n");
	
	// Creating gas/smoke thread 
	thread_create(stackThreadGasSmoke, sizeof(stackThreadGasSmoke), THREAD_PRIORITY_MAIN - 1, 0, 
		threadGasSmoke, NULL, "Thread gas/smoke");
	printf("Gas/smoke thread created...\n");
	
    // Protecting threads
    while(1){};
    
    return 0;
}







