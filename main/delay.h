#ifndef _DELAY_H
#define _DELAY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define delay_ms(ms) vTaskDelay((ms) / portTICK_PERIOD_MS)
#define delay_s(s) delay_ms((s)*1000)

#endif

