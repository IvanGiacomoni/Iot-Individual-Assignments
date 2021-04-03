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

The function assigns the D2 pin for the DHT-22, and this is done 



