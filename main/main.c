#include <stdio.h>
#include "wifi.h"
#include "delay.h"
#include <string.h>
#include "esp_log.h"

#define SSID "DragonHunters"
#define PASSWORD "IreliaMain"

void on_connect(void* arg, esp_event_base_t base, int32_t id, void*data) {
	ip_event_got_ip_t ip_data = *(ip_event_got_ip_t*)data;

	ESP_LOGI("WIFI_STATION", "got ip:" IPSTR, IP2STR(&ip_data.ip_info.ip));
}

void app_main(void) {
	esp_err_t ret;
	DEFINE_WIFI_HANDLE(wifi_handle);
	struct wifi_init_config config = _WIFI_INIT_CONFIG_DEFAULT(SSID, PASSWORD);
	struct static_ip static_ip;
	const unsigned char IP[] = {192, 168, 43, 30};
	const unsigned char GW[] = {192, 168, 43, 1};
	const unsigned char NM[] = {255, 255, 255, 0};

	static_ip_set_ip(&static_ip, IP);
	static_ip_set_gw(&static_ip, GW);
	static_ip_set_netmask(&static_ip, NM);

	wifi_set_on_got_ip(&config, on_connect);
	wifi_set_static_ip(&config, &static_ip);

	ret = wifi_init_sta(&wifi_handle, &config);
	if (ret != ESP_OK) {
		printf("%s\n", esp_err_to_name(ret));
	}

	delay_s(10);

	wifi_deinit_sta(&wifi_handle);
}



