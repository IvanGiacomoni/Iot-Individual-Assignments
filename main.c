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

#define DELAY                       2

#define TEMP_THRESHOLD_MIN          23
#define TEMP_THRESHOLD_MAX          24

#define PPM_THRESHOLD               48

#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (1U)
#define TOPIC_MAXLEN        (64U)

#ifndef EMCUTE_ID
#define EMCUTE_ID           ("MY_ID")
#endif

static char stack[THREAD_STACKSIZE_DEFAULT];

static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];

static void *emcute_thread(void *arg){
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

static void on_pub(const emcute_topic_t *topic, void *data, size_t len){
    char *in = (char *)data;

    printf("### got publication for topic '%s' [%i] ###\n", topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");
}

/**
 * Setup the EMCUTE, open a connection to the MQTT-S broker,
 * subscribe to the default topic and publish a message.
 */
int setup_mqtt(void){
    
    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));
    printf("prima della thread create\n");
    
    printf("sizeof(stack) = %d\n",sizeof(stack));
    printf("sizeof(stack) = %d\n", THREAD_STACKSIZE_DEFAULT);

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");
    printf("dopo la thread create\n");

    // connect to MQTT-SN broker
    printf("Connecting to MQTT-SN broker %s port %d.\n",
           SERVER_ADDR, SERVER_PORT);

    sock_udp_ep_t gw = { .family = AF_INET6, .port = SERVER_PORT };
    char *topic = MQTT_TOPIC;
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

    /* setup subscription to topic*/
    unsigned flags = EMCUTE_QOS_0;
    subscriptions[0].cb = on_pub;
    strcpy(topics[0], MQTT_TOPIC);
    subscriptions[0].topic.name = MQTT_TOPIC;

    if (emcute_sub(&subscriptions[0], flags) != EMCUTE_OK) {
        printf("error: unable to subscribe to %s\n", MQTT_TOPIC);
        return 1;
    }

    printf("Now subscribed to %s\n", MQTT_TOPIC);

    return 1;
}

void publishDataForAws(char* data){
	
	if(emcute_pub(&subscriptions[0].topic, data, strlen(data), EMCUTE_QOS_0) != EMCUTE_OK){
		printf("error: unable to publish to %s\n", MQTT_TOPIC);
		exit(EXIT_FAILURE);
	} 	
}

void initializeLeds(gpio_t* red_led, gpio_t* green_led, gpio_t* yellow_led,
    gpio_t* blue_led, gpio_t* white_led){
	
	// Red led initialization
	*red_led = GPIO_PIN(PORT_B, 5);  // PIN D4
		
	if (gpio_init(*red_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 5);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Red led ready!\n");
	}
	
	// Green led initialization
	*green_led = GPIO_PIN(PORT_A, 8); // PIN D7
		
	if (gpio_init(*green_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 8);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Green led ready!\n");
	}
	
	// Yellow led initialization
	*yellow_led = GPIO_PIN(PORT_A, 9); // PIN D9
		
	if (gpio_init(*yellow_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 9);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Yellow led ready!\n");
	}
	
	// Blue led initialization
	*blue_led = GPIO_PIN(PORT_B, 6); // PIN D10
		
	if (gpio_init(*blue_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 6);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Blue led ready!\n");
	}
	
	// White led initialization
	*white_led = GPIO_PIN(PORT_A, 6); // PIN D12
		
	if (gpio_init(*white_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 6);
		exit(EXIT_FAILURE);
	}
	else{
		printf("White led ready!\n");
	}
}

void initializeBuzzers(gpio_t* gas_smoke_buzzer, gpio_t* temp_buzzer){
	
	// Gas/Smoke buzzer initialization
	*gas_smoke_buzzer = GPIO_PIN(PORT_C, 7);  // PIN D9
		
	if (gpio_init(*gas_smoke_buzzer, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 7);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Gas/Smoke buzzer ready!\n");
	}
	
	// Temperature buzzer initialization
	*temp_buzzer = GPIO_PIN(PORT_A, 5);  // PIN D13
		
	if (gpio_init(*temp_buzzer, GPIO_OUT)) {
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

void initializeDHT(dht_params_t* params, dht_t* dev){
	
	// Initializing DHT_22 parameters
	params -> pin = GPIO_PIN(PORT_A, 10);
	params -> type = DHT22;
	params -> in_mode = DHT_PARAM_PULL;
	
	// Initializating DHT_22 module
	if (dht_init(dev, params) == DHT_OK) {
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

int readTemperatureByDHT(dht_t* dev){
	int16_t temp, hum;
	if (dht_read(dev, &temp, &hum) != DHT_OK) {
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

int main(void){
    
    printf("\nStarting the application...\n");
	xtimer_sleep(2);

    /* Initializing the ADC line */
    initializeADCLine();
    xtimer_sleep(DELAY);
    printf("\n");
    
    // Initializing leds
	gpio_t red_led, green_led, yellow_led, blue_led, white_led;
	
	initializeLeds(&red_led, &green_led, &yellow_led, &blue_led, &white_led);
	xtimer_sleep(DELAY);
	printf("\n");
	
	// Initializing buzzers
	gpio_t gas_smoke_buzzer, temp_buzzer;
	
	initializeBuzzers(&gas_smoke_buzzer, &temp_buzzer);
    xtimer_sleep(DELAY);
	printf("\n");
	
    // Initializing DHT_22 parameters and module
    dht_params_t params;
    dht_t dev;

	initializeDHT(&params, &dev);
	xtimer_sleep(DELAY);
	printf("\n");
	
	// setup MQTT-SN connection
    printf("Setting up MQTT-SN.\n");
    setup_mqtt();
    xtimer_sleep(DELAY);
	printf("\n");
	
	// Periodical sampling
    while (1) {
		
		// Reading ppm values by MQ-2 sensor
		int ppm = readPpmByMQ2();
		printf("ppm: %d\n", ppm);
        
        
        // Reading temperature values by DHT-22 sensor
        int temperature = readTemperatureByDHT(&dev);
	  
		printf("temperature: %d°C\n", temperature);
		
		char* data = "test message for now";
        
        publishDataForAws(data);
		
		// Preprocessing data
		
		if(ppm > PPM_THRESHOLD){
			printf("[GAS/SMOKE] WARNING!!!\n");
			
			led_ON(blue_led);
			led_OFF(white_led);
			//buzzer_ON(gas_smoke_buzzer);
		}
		
		else if(ppm <= PPM_THRESHOLD){
			printf("[GAS/SMOKE] ALL OK!\n");
			led_ON(white_led);
			led_OFF(blue_led);
		}
		
		if (temperature > TEMP_THRESHOLD_MAX){
			printf("[TEMPERATURE] WARNING!!\n");
			
			led_OFF(green_led);
			led_OFF(yellow_led);
			
			led_ON(red_led);
			//buzzer_ON(temp_buzzer);
		}
		
		else if (temperature > TEMP_THRESHOLD_MIN && temperature <= TEMP_THRESHOLD_MAX){
			printf("[TEMPERATURE] BE CAREFUL TO THE TEMPERATURE!\n");
			
			led_OFF(green_led);
			led_OFF(red_led);
			
			led_ON(yellow_led);
		}
		
		else if(temperature <= TEMP_THRESHOLD_MIN){
			printf("[TEMPERATURE] ALL OK!\n");
			
			led_OFF(red_led);
			led_OFF(yellow_led);
			
			led_ON(green_led);
		}
		
		xtimer_sleep(DELAY);
		buzzer_OFF(gas_smoke_buzzer);
		buzzer_OFF(temp_buzzer);
		printf("\n");
    }
    
    return 0;
}







