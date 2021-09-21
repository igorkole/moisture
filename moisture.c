#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/regs/intctrl.h"
#include "pico/stdlib.h"

#define MOISTURE_ADC_IN 0

#define MOISTURE_PIN_POWER 22
#define MOISTURE_PIN_SIG 26

static void moisture_init(void);
static void moisture_run(void);
static void moisture_irs_adc(void);
static void moisture_power_on(void);

int main(void) {
	/*
	 * Initialization
	 */
	moisture_init();
	/*
	 * Run
	 */
	moisture_run();
	/*
	 * The conversions are processing in the ADC interrupt handler,
	 * so just sleeping here.
	 */
	for (;;) {
		sleep_ms(1000);
	}
}

static void moisture_init(void) {
	/*
	 * Standard input/output for debugging.
	 */
	stdio_init_all();
	/*
	 * ADC
	 */
	adc_init();
	/*
	 * Power pin
	 */
	gpio_init(MOISTURE_PIN_POWER);
	gpio_set_dir(MOISTURE_PIN_POWER, GPIO_OUT);
	/*
	 * Signal pin
	 */
	adc_gpio_init(MOISTURE_PIN_SIG);
	adc_select_input(MOISTURE_ADC_IN);
	/*
	 * FIFO
	 */
	adc_fifo_setup(true, false, 8, true, false);
	/*
	 * IRQ handler
	 */
	irq_set_exclusive_handler(ADC_IRQ_FIFO, moisture_irs_adc);
	irq_set_enabled(ADC_IRQ_FIFO, true);
	/*
	 * All set, enable ADC interrupts.
	 */
	adc_irq_set_enabled(true);
}

static void moisture_irs_adc(void) {
	uint32_t sum = 0;
	uint16_t conv;
	uint8_t level;
	uint8_t i;

	/*
	 * How many conversion the FIFO holds.
	 */
	level = adc_fifo_get_level();
	/*
	 * Average of them.
	 */
	for (i = 0; i < level; ++i) {
		conv = adc_fifo_get();
		if (conv & 0x1000) {
			/*
			 * Conversion error occured; skip it.
			 */
			continue;
		}
		sum += conv;
	}
	conv = (uint16_t)(sum / level);
	/*
	 * Display the result.
	 */
	printf("%u\n", conv);
}

static void moisture_run(void) {
	/*
	 * Turn the ADC's power on.
	 */
	moisture_power_on();
	/*
	 * Start conversions
	 */
	adc_run(true);
}

static void moisture_power_on(void) {
	gpio_put(MOISTURE_PIN_POWER, 1);
	sleep_ms(10);
}
