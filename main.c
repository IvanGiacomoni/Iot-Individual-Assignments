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

#define TEMP_THRESHOLD_MIN          24
#define TEMP_THRESHOLD_MAX          26

#define PPM_THRESHOLD               37

void initializeLeds(gpio_t* red_led, gpio_t* green_led, gpio_t* yellow_led, gpio_t* blue_led){
	// Led red initialization
	*red_led = GPIO_PIN(PORT_B, 5);  // PIN D4
		
	if (gpio_init(*red_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 5);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Red led ready!\n");
	}
	
	// Green red initialization
	*green_led = GPIO_PIN(PORT_A, 8); // PIN D7
		
	if (gpio_init(*green_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 8);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Green led ready!\n");
	}
	
	// Yellow red initialization
	*yellow_led = GPIO_PIN(PORT_A, 9); // PIN D9
		
	if (gpio_init(*yellow_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 9);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Yellow led ready!\n");
	}
	
	// Blue red initialization
	*blue_led = GPIO_PIN(PORT_B, 6); // PIN D10
		
	if (gpio_init(*blue_led, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 6);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Blue led ready!\n");
	}
}

void initializeBuzzer(gpio_t* buzzer){
	*buzzer = GPIO_PIN(PORT_C, 7);  // PIN D9
		
	if (gpio_init(*buzzer, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 7);
		exit(EXIT_FAILURE);
	}
	else{
		printf("Buzzer ready!\n");
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
    xtimer_sleep(2);
    printf("\n");
    
    // Initializing leds
	gpio_t red_led, green_led, yellow_led, blue_led;
	
	initializeLeds(&red_led, &green_led, &yellow_led, &blue_led);
	xtimer_sleep(2);
	printf("\n");
	
	// Initializing the buzzer
	gpio_t buzzer;
	
	initializeBuzzer(&buzzer);
    xtimer_sleep(2);
	printf("\n");
	
    // Initializing DHT_22 parameters and module
    dht_params_t params;
    dht_t dev;

	initializeDHT(&params, &dev);
	xtimer_sleep(2);
	printf("\n");
	
	// Periodical sampling
    while (1) {
		
		// Reading ppm values by MQ-2 sensor
		int ppm = readPpmByMQ2();
		printf("ppm: %d\n", ppm);
        
        
        // Reading temperature values by DHT-22 sensor
        int temperature = readTemperatureByDHT(&dev);
	  
		printf("temperature: %d°C\n", temperature);
		
		// Preprocessing data
		
		if(ppm > PPM_THRESHOLD){
			printf("[GAS/SMOKE] WARNING!!!\n");
			
			led_ON(blue_led);
			buzzer_ON(buzzer);
		}
		
		else if(ppm <= PPM_THRESHOLD){
			printf("[GAS/SMOKE] ALL OK!\n");
			
			led_OFF(blue_led);
			buzzer_OFF(buzzer);
		}
		
		if (temperature > TEMP_THRESHOLD_MAX){
			printf("[TEMPERATURE] WARNING!!\n");
			
			led_OFF(green_led);
			led_OFF(yellow_led);
			
			led_ON(red_led);
			//buzzer_ON(buzzer);
		}
		
		else if (temperature > TEMP_THRESHOLD_MIN && temperature <= TEMP_THRESHOLD_MAX){
			printf("[TEMPERATURE] BE CAREFUL TO THE TEMPERATURE!\n");
			
			led_OFF(green_led);
			led_OFF(red_led);
			buzzer_OFF(buzzer);
			
			led_ON(yellow_led);
		}
		
		else if(temperature <= TEMP_THRESHOLD_MIN){
			printf("[TEMPERATURE] ALL OK!\n");
			
			led_OFF(red_led);
			led_OFF(yellow_led);
			buzzer_OFF(buzzer);
			
			led_ON(green_led);
		}
		
		xtimer_sleep(DELAY);
		buzzer_OFF(buzzer);
		printf("\n");
    }
    
    return 0;
}







