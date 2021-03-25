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

#define ADC_IN_USE                  ADC_LINE(5)
#define ADC_RES                     ADC_RES_10BIT

#define DELAY                       (200LU * US_PER_MS) /* 500 ms */

int main(void){
    
    printf("\nStarting the application...\n");

    /* initialize the ADC line */
    if (adc_init(ADC_IN_USE) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_IN_USE);
        return 1;
    }
    else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_IN_USE);
    }
    
    // Initialization of DHT_22 parameters
    dht_params_t my_params;
	my_params.pin = GPIO_PIN(PORT_A, 10);
	my_params.type = DHT22;
	my_params.in_mode = DHT_PARAM_PULL;
	
	// Initialization of DHT_22 module
	dht_t dev;

	if (dht_init(&dev, &my_params) == DHT_OK) {
		printf("DHT sensor connected\n");
	}
	else {
		printf("Failed to connect to DHT sensor\n");
		return 1;
	}

    xtimer_ticks32_t last = xtimer_now();
    int mq2_sample = 0;
    int ppm = 0;
    
    // Red led initialization
    gpio_t pin_out = GPIO_PIN(PORT_B, 5);
		
	if (gpio_init(pin_out, GPIO_OUT)) {
		printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 5);
		return -1;
	}
	
    /* Sampling values from the MQ-2 sensor*/
    while (1) {
		printf("ciao\n");
        mq2_sample = adc_sample(ADC_IN_USE, ADC_RES);
        ppm = adc_util_map(mq2_sample, ADC_RES, 10, 100);

        if (mq2_sample < 0) {
            printf("ADC_LINE(%u): selected resolution not applicable\n", ADC_IN_USE);
        }
        else {
            printf("ADC_LINE(%u): raw value: %i, ppm: %i\n", ADC_IN_USE, mq2_sample, ppm);
        }
		
		/*printf("Set pin to HIGH\n");
		gpio_set(pin_out);
	
		xtimer_sleep(2);

		printf("Set pin to LOW\n");
		gpio_clear(pin_out);

		xtimer_sleep(2);*/
        
        /* Retrieval of data by DHT sensor */
		/*int16_t temp, hum;
		if (dht_read(&dev, &temp, &hum) != DHT_OK) {
			printf("Error reading values\n");
		}*/

		/* Formatting the temperature into a string*/
		/*char temp_s[10];
		size_t n = fmt_s16_dfp(temp_s, temp, -1);
		temp_s[n] = '\0';
		
	    // Converting the temp string into an integer
	    int temp_int = atoi(temp_s);
	  
		printf("temperature: %d°C\n", temp_int);*/
		
		
		/* [TODO] HERE THERE WILL BE SOME PREPROCESSING STUFF WITH RETRIEVED TEMPERATURE AND PPM... */

        xtimer_periodic_wakeup(&last, DELAY);
    }
    
    return 0;
}








