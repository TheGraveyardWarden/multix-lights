#include "relay.h"

const int8_t relays[] = {18, 19, 21, 22};

esp_err_t relay_init(uint64_t pins) {
        gpio_config_t conf = {};

        conf.intr_type = GPIO_INTR_DISABLE;
        conf.mode = GPIO_MODE_INPUT_OUTPUT;
        conf.pin_bit_mask = pins;
        conf.pull_down_en = 0;
        conf.pull_up_en = 0;

        return gpio_config(&conf);
}

int relay_switch(int8_t pin) {
	int lvl;

	lvl = gpio_get_level(pin);
	gpio_set_level(pin, !lvl);

	return !lvl;
}

