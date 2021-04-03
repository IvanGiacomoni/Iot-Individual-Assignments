# Code
The application starts by initializing the DHT-22 sensor by using the *initializeDHT* function:

```
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
```

We need to include the DHT module in the Makefile:

```
USEMODULE += dht
```

We need to include the following headers in the main.c:

```
#include "dht.h"
#include "dht_params.h"
```

The function assigns the D2 pin for the DHT-22 sensor, and this is done by using the GPIO peripheral driver, that maps each pin to a port with a certain port number, in this case the D2 pin is connected to the PB port of the MCU at pin number 10. Below we can see all mappings:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/conversions.png)

By using this picture, we can use the same logic to initialize the pins for leds and buzzers (for more info see [this](https://github.com/IvanGiacomoni/Iot-Individual-Assignments#wiring-of-components), respectively with the *initializeLeds* and the *initializeBuzzers* functions:

```
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
```

```
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
```

After all this, we have also to initialize the ADC line: to do this, we need to include in the Makefile the periph_adc module:

```
FEATURES_REQUIRED += periph_adc
```

Also we need to include these headers in the main.c:

```
#include "periph/adc.h"
#include "periph/gpio.h"
```

Below we can see the *initializeADCLine* function:

```
void initializeADCLine(void){
	if (adc_init(ADC_IN_USE) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_IN_USE);
        exit(EXIT_FAILURE);
    }
    else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_IN_USE);
    }
}
```
The ADC line will be exploited for the sampling from the MQ-2 sensor, and given that the AOUT pin is connected to the A0 pin of the board, in this case the A0 pin is mapped into the 0 line. Also, the ADC resolution is 10 bit.

```
#define ADC_IN_USE  ADC_LINE(0)
#define ADC_RES     ADC_RES_10BIT
```

At this point, we have all sensors and actuators ready, so we need to setup MQTT, and to do this I used a *setup_mqtt* function, and then we need to suscribe to our 4 main topics, by using the *void mqttSubscribeTo* function, that takes as input the name of the topic and the position of the global array of subscriptions where we will save all info about the new subscription. 

```
static emcute_sub_t subscriptions[NUMOFSUBS];
```

For more details you can check the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c) and also this [tutorial](https://github.com/RIOT-OS/RIOT/tree/master/examples/emcute_mqttsn).



