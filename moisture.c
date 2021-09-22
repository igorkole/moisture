#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"

#include "ws2812.pio.h"

// How often perform a conversion
static uint const conv_freq_ms = 500;

// ADC used
static uint const adc = 0;

// PIO and state machine for running the LED.
static PIO const pio = pio0;
static uint const sm = 0;

// ADC raw value to voltage conversion factor.
static float const conv_factor = 3.3f / (1 << 12);

enum {
	// LED
	PIN_LED = 21,
	// ADC power
	PIN_POWER = 22,
	// ADC signal
	PIN_SIG = 26,
};

static void put_pixel(uint32_t grb) {
	pio_sm_put_blocking(pio, sm, grb << 8);
}

static uint32_t ugrb(uint8_t r, uint8_t g, uint8_t b) {
	return
		((uint32_t)r << 8) |
		((uint32_t)g << 16) |
		((uint32_t)b);
}

static int map(int x,
		int xmin, int xmax,
		int ymin, int ymax) {
	return (x - xmin) * (ymax - ymin) / (xmax - xmin) + ymin;
}

// Initial min/max conversion values.
// These get updated if ADC returns smaller/bigger value.
static int conv_min = 2400;
static int conv_max = 3100;

static void conv(void) {
	uint i;

	// Perform a conversion
	uint const conv_count = 11;
	uint16_t val = 0;
	for (i = 0; i < conv_count; ++i) {
		val += adc_read();
	}
	val /= i;
	printf("raw: %u, voltage: %gV\n", val, val * conv_factor);

	// Re-calibrate a bit if needed
	if (val < conv_min) {
		conv_min = val;
	} else if (val > conv_max) {
		conv_max = val;
	}

	// Update LED
	uint const pixel_count = 12;
	uint16_t const xmin = 0;
	uint16_t const xmax = conv_max - conv_min;
	uint16_t const ymin = 5;
	uint16_t const ymax = 60;
	uint16_t r = map(conv_max - val, xmin, xmax, ymin, ymax);
	uint16_t g = map(val - conv_min, xmin, xmax, ymin, ymax);
	uint16_t b = 0;
	for (i = 0; i < pixel_count; ++i) {
		put_pixel(ugrb(r, g, b));
	}
}

int main(void) {
	// Standard input/output for debugging
	stdio_init_all();

	// LED pin
	gpio_init(PIN_LED);
	gpio_set_dir(PIN_LED, GPIO_OUT);

	// ADC power pin
	gpio_init(PIN_POWER);
	gpio_set_dir(PIN_POWER, GPIO_OUT);

	// Init ADC
	adc_init();

	// ADC signal pin
	adc_gpio_init(PIN_SIG);
	adc_select_input(adc);

	// Set up PIO for LED
	uint offset = pio_add_program(pio, &ws2812_program);
	ws2812_program_init(pio, sm, offset, PIN_LED, 800000, false);

	// Power on ADC
	gpio_put(PIN_POWER, 1);
	sleep_ms(10);

	for (;;) {
		conv();
		sleep_ms(conv_freq_ms);
	}
}
