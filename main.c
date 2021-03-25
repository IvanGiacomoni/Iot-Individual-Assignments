#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net/emcute.h"

#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "analog_util.h"

#define ADC_IN_USE                  ADC_LINE(5)
#define ADC_RES                     ADC_RES_12BIT

#define DELAY                       (500LU * US_PER_MS) /* 100 ms */

int main(void){
    
    printf("\nStarting the reading of values...\n");

    /* initialize the ADC line */
    if (adc_init(ADC_IN_USE) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_IN_USE);
        return 1;
    }
    else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_IN_USE);
    }

    xtimer_ticks32_t last = xtimer_now();
    int sample = 0;
    int gas = 0;

    /* Sampling */
    while (1) {
        sample = adc_sample(ADC_IN_USE, ADC_RES);
        gas = adc_util_map(sample, ADC_RES, 10, 100);

        if (sample < 0) {
            printf("ADC_LINE(%u): selected resolution not applicable\n",
                   ADC_IN_USE);
        }
        else {
            printf("ADC_LINE(%u): raw value: %i, gas: %i\n", ADC_IN_USE, sample,
                   gas);
        }

        xtimer_periodic_wakeup(&last, DELAY);
    }
    
    return 0;
}








