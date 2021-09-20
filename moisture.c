#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define MOISTURE_READ_FREQ_MS 2000

#define MOISTURE_PIN_POWER 22
#define MOISTURE_PIN_SIG 26

#define MOISTURE_ADC_IN 0

void moisture_init(void);
uint16_t moisture_read(void);
void moisture_print(uint16_t val);
void moisture_power_on(void);
void moisture_power_off(void);
uint16_t moisture_read_sig(void);

int main(void) {
	uint16_t val;

	moisture_init();
	for (;;) {
		val = moisture_read();
		moisture_print(val);
		sleep_ms(MOISTURE_READ_FREQ_MS);
	}
}

void moisture_init(void) {
	stdio_init_all();
	adc_init();
	gpio_init(MOISTURE_PIN_POWER);
	gpio_set_dir(MOISTURE_PIN_POWER, GPIO_OUT);
	adc_gpio_init(MOISTURE_PIN_SIG);
	adc_select_input(MOISTURE_ADC_IN);
}

/*
 * How many conversions do for a single measurement.
 */
#define CONVERTION_COUNT 11

/*
 * Turns the ADC power on.
 * Performs a number of conversions.
 * Turns the power off.
 * Returns the moisture measurement as an average of the conversions performed.
 */
uint16_t moisture_read(void) {
	uint32_t val;
	size_t i;

	moisture_power_on();
	sleep_ms(10);
	val = 0;
	for (i = 0; i < CONVERTION_COUNT; ++i) {
		val += moisture_read_sig();
	}
	moisture_power_off();
	return (uint16_t)(val / i);
}

void moisture_power_on(void) {
	gpio_put(MOISTURE_PIN_POWER, 1);
}

void moisture_power_off(void) {
	gpio_put(MOISTURE_PIN_POWER, 0);
}

uint16_t moisture_read_sig(void) {
	uint16_t val;

	val = adc_read();
	return val;
}

#define CONVERSION_FACTOR (3.3f / (1 << 12))

void moisture_print(uint16_t val) {
	printf("raw: %d, voltage: %gV\n",
			val, val * CONVERSION_FACTOR);
}
