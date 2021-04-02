# Gas-Leaks-And-Fires-Notifier

## Scenario
The problem I took into consideration is that of gas leaks and fires that usually occur in home environments. This problem is a huge problem, because of the serious injuries and deaths it causes, especially from the fires that are generated after the explosion. Iot is very helpful when we can prevent these terrible situations, for example by using a notification system: the main goal of Gas-Leaks-And-Fires-Notifier is that of preventing the outbreak of fires by a constant monitoring of the home environment, in particular those points of the house where it is more probable for a gas leak to occur, such as the kitchen.

## Sensors
The environment will be monitored with the help of two sensors, which will help us to retrieve the data that are useful to our analysis:

- the DHT-22 sensor for retrieving information about the environment temperature
- the MQ-2 sensor for retrieving information about the amount of gas or smoke that we have in the environment

For acting into the environment I used two different types of actuators:

- blink leds, in order to have a visual notification if there is an high probability of an imminent gas leak
- buzzers, in order to have a sound notification if there is an hign probability of an imminent gas leak

The sampling of data from both sensors will be periodic, because if we just think to a real situation where, for example, we are cooking something in the kitchen, but we are not watching what is happening because in the meanwhile we are watching TV, we can't know if we are safe or if we are in danger, and given that every second is important in order to avoid accidents, the sampling must be periodic. In particular, gas and smoke will be sampled more often than temperature data, because even if both are important in our scenario, it is clear that the greater danger will come from smoke and gas: so the sampling period for gas and smoke data will be in a range of [2, 5] seconds, while the sampling period for temperature data will be in a range of [10, 15] seconds.  

Speaking about the actuators, their purposes are strictly connectes to data retrieved from both sensors: in particular, I used three leds and one buzzer for acting with respect to temperature data, and two leds and one buzzer for acting with respect to gas and smoke data.

Temperature actuators:

- green led, used to notify people that actual values of temperature measured by the DHT-22 sensor are not dangerous
- yellow led, used to notify people that actual values of temperature measured by the DHT-22 sensor are above average, so there is no real danger, but we have to pay attention to what we are actually doing
- red led, used to notify people, together with the buzzer, that actual values of temperature measured by the DHT-22 sensor are above or a lot above average
- buzzer, used to notify people, together with the red led, that actual values of temperature measured by the DHT-22 sensor are above or a lot above average

Gas and smoke actuators:

- white led, used to notify people that actual amounts of gas or smoke measured by the MQ-2 sensor are not dangerous
- blue led, used to notify people, together with the buzzer, that actual amounts of gas or smoke measured by the MQ-2 sensor are above or a lot above average
- buzzer, used to notify people, together with the blue led, that actual amounts of gas or smoke measured by the MQ-2 sensor are above or a lot above average

Also, I want to specify the real utilities provided by both leds and buzzers. Leds are very useful in a lot of situations, for example that of a person that is listening to the music on his phone at a very high volume, and cannot hear the buzzer sound, while buzzers are useful in different types of situations, for example situations where people are very distracted and do not notice that the red led is flashing, or when they are sleeping, or also when they are out of home and neighbors could hear the buzzer noising.

## Data collected
