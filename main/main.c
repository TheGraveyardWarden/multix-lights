#include <stdio.h>
#include <stdint.h>
#include "wifi.h"
#include "delay.h"
#include <string.h>
#include "esp_log.h"
#include "webserver.h"
#include "relay.h"
#include "db.h"

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
	button_t *btns;
	size_t nr_btns;
	int i;
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

	printf("initializing db\n");
	if (db_init() != ESP_OK)
	{
		printf("failed to initialize db\n");
		return;
	}

	ret = wifi_init_sta(&wifi_handle, &config);
	if (ret != ESP_OK)
		goto out;

	ret = relay_init(RELAYS);
	if (ret != ESP_OK)
		goto out;

	ret = db_read_btns(&btns, &nr_btns);
	if (ret != ESP_OK) 
		goto out;

	for (i = 0; i < nr_btns; i++)
		gpio_set_level(relays[i], btns[i].status);

out:
	printf("%s\n", esp_err_to_name(ret));
	return;
}

