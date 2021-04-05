# Code

## Initializing the DHT-22 sensor
The application starts by **initializing the DHT-22 sensor** by using the *initializeDHT* function:

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

We need to include the DHT module in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/Makefile):

```
USEMODULE += dht
```

We need to include the following **headers** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

```
#include "dht.h"
#include "dht_params.h"
```

The function assigns the **D2 pin** for the DHT-22 sensor, and this is done by using the **GPIO peripheral driver**, that maps each pin to a **port** with a certain **port number**, in this case the D2 pin is connected to the **PB port** of the MCU at **pin number 10**. Below we can see all mappings:

![img](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/images/conversions.png)

## Initializing leds and buzzers
By using this picture, we can use the same logic to **initialize the pins for leds and buzzers** (for more info see [this](https://github.com/IvanGiacomoni/Iot-Individual-Assignments#wiring-of-components), respectively with the *initializeLeds* and the *initializeBuzzers* functions:

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

## Initializing the ADC line
After all this, we have also to **initialize the ADC line**: to do this, we need to include in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/Makefile) the **periph_adc module**:

```
FEATURES_REQUIRED += periph_adc
```

Also we need to include these **headers** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

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
The ADC line will be exploited for the **sampling from the MQ-2 sensor**, and given that the **AOUT pin** is connected to the **A0 pin** of the board, in this case the A0 pin is mapped into the **0 line**. Also, the **ADC resolution** is **10 bit**.

```
#define ADC_IN_USE  ADC_LINE(0)
#define ADC_RES     ADC_RES_10BIT
```

## MQTT 
At this point, we have all sensors and actuators ready, so we need to setup **MQTT**, and to do this I used a *setup_mqtt* function, and then we need to **suscribe** to our **4 main topics**, by using the *mqttSubscribeTo* function, that takes as input the name of the topic and the position of the global array of subscriptions where we will save all info about the new subscription. 

```
static emcute_sub_t subscriptions[NUMOFSUBS];
```

In the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/Makefile) we specify our MQTT topics:

```
MQTT_TOPIC_TEMP = device/1/temperature
MQTT_TOPIC_GAS_SMOKE = device/1/gas_smoke
SWITCH_MODE_DEVICE_1 = switchMode/device/1
MANAGE_ACTUATORS_DEVICE_1 = manageActuators/device/1

CFLAGS += -DMQTT_TOPIC_TEMP='"$(MQTT_TOPIC_TEMP)"'
CFLAGS += -DMQTT_TOPIC_GAS_SMOKE='"$(MQTT_TOPIC_GAS_SMOKE)"'
CFLAGS += -DSWITCH_MODE_DEVICE_1='"$(SWITCH_MODE_DEVICE_1)"'
CFLAGS += -DMANAGE_ACTUATORS_DEVICE_1='"$(MANAGE_ACTUATORS_DEVICE_1)"'
```

Obviously we have only one device for now.

For more details you can check the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c) and also this [tutorial](https://github.com/RIOT-OS/RIOT/tree/master/examples/emcute_mqttsn).

## Periodical sampling
Now we are ready to start sampling from sensors, and given that we need to to **periodical sampling**, we need to use the **xtimer module**. So we need to inlude it in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/Makefile):

```
USEMODULE += xtimer
```

Also, we need to include this **header** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

```
#include "xtimer.h"
```

The **sampling period** will be different for the two sensors, so we define **two different constants** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

```
#define GAS_SMOKE_DELAY             2
#define TEMP_DELAY                  10
```

At this point I create **two threads**, one for managing the sampling from the MQ-2 sensor and the other one for sampling from the DHT-22 sensor: the main reason is given by the different sampling periods, so with threads we are able to manage both samplings in parallel:

```
// Creating temperature thread 
thread_create(stackThreadTemp, sizeof(stackThreadTemp), THREAD_PRIORITY_MAIN - 1, 0, 
		threadTemp, NULL, "Thread temperature");

// Creating gas/smoke thread 
thread_create(stackThreadGasSmoke, sizeof(stackThreadGasSmoke), THREAD_PRIORITY_MAIN - 1, 0, 
		threadGasSmoke, NULL, "Thread gas/smoke");
```

If the actual mode of the system is set on "auto", then both threads will start sampling once they are created.

### Temperature
For temperature, we need to setup the **medium and high threshold** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

```
#define TEMP_THRESHOLD_MIN          32
#define TEMP_THRESHOLD_MAX          40
```

The **dht module** encodes the sensor values using **16bit integers**, so we can use the **fmt module** of RIOT in order to format the 16bit integers into **formatted strings**. To do this, we need to specify it in the [Makefile](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/Makefile):

```
USEMODULE += fmt
```

We also need to include this **header** in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

```
#include "fmt.h"
```

Finally, the string value is converted to **integer** by using the *atoi* function of the **stdlib.h** library of C. All is resumed in the *readTemperatureByDHT* function:

```
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

```

- If the value is **below the medium threshold**, the **green led** turns **on** and an "OK" message is sent to AWS by MQTT on topic **device/1/temperature**
- If the value is **over the medium threshold** and **below the high threshold**, the **yellow led** turns **on** and a "TEMP GROWING" message is sent to AWS by MQTT on topic **device/1/temperature**
- If the value is **above the high threshold**, the **red led** and the **temperature buzzer** turn **on** and a "WARNING" message is sent to AWS by MQTT on topic **device/1/temperature**

```
// Reading temperature values by DHT-22 sensor
int temperature = readTemperatureByDHT();

if (temperature > TEMP_THRESHOLD_MAX){
	temperature_state = "WARNING!";
			
	led_OFF(green_led);
        led_OFF(yellow_led);
			
	led_ON(red_led);
	buzzer_ON(temp_buzzer);
}
		
else if (temperature > TEMP_THRESHOLD_MIN && temperature <= TEMP_THRESHOLD_MAX){
	temperature_state = "GROWING!";
			
	led_OFF(green_led);
	led_OFF(red_led);
			
	led_ON(yellow_led);
}
		
else if(temperature <= TEMP_THRESHOLD_MIN){
	temperature_state = "ALL OK!";
			
	led_OFF(red_led);
	led_OFF(yellow_led);
			
	led_ON(green_led);
}
		
```

Data are sent to AWS by using a *publishDataForAws* function, that takes as input the data and the topic where data are published. Obviously, data are formatted in **JSON**, in order to make the things work for storing them into **DynamoDB**.

```
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
```

### Gas and smoke
For gas and smoke, we need to setup only one threshold in the [main.c](https://github.com/IvanGiacomoni/Iot-Individual-Assignments/blob/main/main.c):

```
#define PPM_THRESHOLD               60
```
For gas and smoke, we exploit the **ADC line**, and all is resumed in the *readPpmByMQ2* function:

```
int readPpmByMQ2(void){
	
    int mq2_sample = adc_sample(ADC_IN_USE, ADC_RES);
    int ppm = adc_util_map(mq2_sample, ADC_RES, 10, 100);

    if (mq2_sample < 0) {
        printf("ADC_LINE(%u): selected resolution not applicable\n", ADC_IN_USE);
        return -1;
    }
    
    return ppm;
}
```

- If the value is **below the threshold**, the **white led** turns **on** and an "OK" message is sent to AWS by MQTT on **topic device/1/gas_smoke**
- If the value is **over the threshold**, the **blue led** and the **gas/smoke buzzer** turn **on** and a "WARNING" message is sent to AWS by MQTT on topic **device/1/gas_smoke**

```
// Reading ppm values by MQ-2 sensor
int ppm = readPpmByMQ2();

if(ppm > PPM_THRESHOLD){
	gas_smoke_state = "WARNING!";
			
	led_ON(blue_led);
	led_OFF(white_led);
	//buzzer_ON(gas_smoke_buzzer);
}
		
else if(ppm <= PPM_THRESHOLD){
	gas_smoke_state = "ALL_OK!";		
				
	led_ON(white_led);
	led_OFF(blue_led);
}
```

Data are sent to AWS by using the *publishDataForAws* function, that takes as input the data and the topic where data are published. Obviously, data are formatted in **JSON**, in order to make the things work for storing them into DynamoDB.

```
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
```

Below I show the publishDataForAws function:

```
void publishDataForAws(char* data, emcute_topic_t* topic){
	
	if(emcute_pub(topic, data, strlen(data), EMCUTE_QOS_0) != EMCUTE_OK){
		printf("error: unable to publish to %s\n", topic->name);
		exit(EXIT_FAILURE);
	} 	
}
```

## Turning on and off leds
**Leds** are turned **on** and **off** with these functions (the same can be said for buzzers):

```
void led_ON(gpio_t led){
	gpio_set(led);
}

void led_OFF(gpio_t led){
	gpio_clear(led);
}
```

## Switching mode (auto and manual)
When the user send through the **frontend** the indication to **switch mode**, this indication is published on the **switchMode/device/1** MQTT topic, and then received by our device. If we have to switch to **manual** mode, the periodical sampling is blocked and we are ready to receive indications related to actuators, otherwise if we are switching to **auto**, the periodical sampling is resumed.

## Controlling the state of actuators
In order to control the state of actuators, we can receive the indication from the **frontend** through the **magageActuators/device/1** MQTT topic. What we are going to receive is the **name of the actuator**, the **type of data** it is related to (temperature or gas/smoke) and the **action** we need to do, so ON or OFF. Particularly, the idea is that of only controlling buzzers, the red led and the blue led, because these are the actuators related to dangerous situations. All this code is managed in the *on_pub* function, and below I report the part of code related to temperature:

```
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
```
