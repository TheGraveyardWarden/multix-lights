#ifndef _RELAY_H
#define _RELAY_H

#include "esp_err.h"
#include "driver/gpio.h"
#include <stdint.h>

#define RELAY_1 18ULL
#define RELAY_2 19ULL
#define RELAY_3 21ULL
#define RELAY_4 22ULL

// pin to mask
#define PTM(p) (1ULL << (p))

#define RELAYS (PTM(RELAY_1) | PTM(RELAY_2) | PTM(RELAY_3) | PTM(RELAY_4))
#define NUM_RELAYS 4

extern const int8_t relays[NUM_RELAYS];

esp_err_t relay_init(uint64_t pins);
int relay_switch(int8_t pin);

#endif
