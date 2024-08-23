#include "handlers.h"
#include "esp_wifi.h"

void default_wifi_start_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
	esp_wifi_connect();
}
