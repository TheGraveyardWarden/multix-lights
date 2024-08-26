#include <stdio.h>
#include <stdint.h>
#include "wifi.h"
#include "delay.h"
#include <string.h>
#include "esp_log.h"
#include "webserver.h"

#define SSID "T5"
#define PASSWORD "S3SHT1LL1R3ST"

static httpd_handle_t server;
extern const uint8_t index_html[] asm("_binary_index_html_start");

void on_connect(void* arg,
		esp_event_base_t base,
		int32_t id,
		void* data)
{
	printf("connected to wifi\n");
	server = webserver_start();
}

void on_disconnect(void* arg,
		   esp_event_base_t base,
		   int32_t id,
		   void* data)
{
	printf("disconnected from wifi\n");
	webserver_stop(server);
	printf("trying again...\n");
	esp_wifi_connect();
}

void app_main(void) {
	esp_err_t ret;
	DEFINE_WIFI_HANDLE(wifi_handle);
	struct wifi_init_config config = _WIFI_INIT_CONFIG_DEFAULT(SSID, PASSWORD);
	struct static_ip static_ip;
	const unsigned char IP[] = {192, 168, 1, 30};
	const unsigned char GW[] = {192, 168, 1, 1};
	const unsigned char NM[] = {255, 255, 255, 0};

	static_ip_set_ip(&static_ip, IP);
	static_ip_set_gw(&static_ip, GW);
	static_ip_set_netmask(&static_ip, NM);

	wifi_set_on_connect(&config, on_connect);
	wifi_set_on_disconnect(&config, on_disconnect);
	wifi_set_static_ip(&config, &static_ip);

	ret = wifi_init_sta(&wifi_handle, &config);
	if (ret != ESP_OK) {
		printf("%s\n", esp_err_to_name(ret));
	}
}

