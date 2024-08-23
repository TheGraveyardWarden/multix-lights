#include "wifi.h"
#include "nvs_flash.h"
#include <stdio.h>


esp_err_t enable_nvs(void) {
	esp_err_t ret;

	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ret = nvs_flash_erase();
		check_err(ret, "nvs flash erase", out);

		ret = nvs_flash_init();
	}

out:
	return ret;
}

#define REGISTER_HANDLER(handler, base, id, ret)	\
	if (handler != NULL) {	\
		ret = esp_event_handler_register(base, id, handler, NULL);\
	}

esp_err_t wifi_init_event_loop(struct event_loop_config* config) {
	esp_err_t ret;

	ret = esp_event_loop_create_default();
	if (ret != ESP_ERR_INVALID_STATE && ret != ESP_OK) {
		printf("event loop create default: %s\n", esp_err_to_name(ret));
		return ret;
	}

	ret = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, config->on_start, NULL);
	check_err(ret, "register sta start", del_loop);

	REGISTER_HANDLER(config->on_stop, WIFI_EVENT, WIFI_EVENT_STA_STOP, ret);
	check_err(ret, "register sta stop", del_loop);

	REGISTER_HANDLER(config->on_connect, WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, ret);
	check_err(ret, "register sta connected", del_loop);

	REGISTER_HANDLER(config->on_disconnect, WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, ret);
	check_err(ret, "register sta disconnected", del_loop);

	REGISTER_HANDLER(config->on_got_ip, IP_EVENT, IP_EVENT_STA_GOT_IP, ret);
	check_err(ret, "register sta got ip", del_loop);

	REGISTER_HANDLER(config->on_lost_ip, IP_EVENT, IP_EVENT_STA_LOST_IP, ret);
	check_err(ret, "register sta lost ip", del_loop);

	return ret;

del_loop:
	esp_event_loop_delete_default();
	return ret;
}

esp_err_t wifi_init_sta(struct wifi_handle* handle, struct wifi_init_config* config) {
	esp_err_t ret;

	if (handle->init) {
		return ESP_ERR_INVALID_STATE;
	}

	ret = wifi_init_event_loop(&config->event_loop_config);
	check_err(ret, "wifi event loop", out);

	ret = esp_netif_init();
	check_err(ret, "esp_netif_init", del_loop);

	handle->netif = esp_netif_create_default_wifi_sta();

	if (config->static_ip != NULL) {
		esp_netif_dhcpc_stop(handle->netif);
		esp_netif_set_ip_info(handle->netif, &config->static_ip->ip_info);
	}

	if (config->wifi_init_config.nvs_enable) {
		ret = enable_nvs();
		check_err(ret, "nvs enable", destroy_wifi);
	}

	ret = esp_wifi_init(&config->wifi_init_config);
	check_err(ret, "esp_wifi_init", nvs_deinit);

	ret = esp_wifi_set_mode(WIFI_MODE_STA);
	check_err(ret, "wifi_set_mode", wifi_deinit);

	ret = esp_wifi_set_config(WIFI_IF_STA, &config->wifi_config);
	check_err(ret, "wifi_set_config", wifi_deinit);

	ret = esp_wifi_start();
	check_err(ret, "wifi_start", wifi_deinit)
	
	handle->init = true;
	return ret;

wifi_deinit:
	esp_wifi_deinit();
nvs_deinit:
	if (config->wifi_init_config.nvs_enable)
		nvs_flash_deinit();
destroy_wifi:
	esp_netif_destroy_default_wifi((void*)handle->netif);
	esp_netif_deinit();
del_loop:
	esp_event_loop_delete_default();
out:
	return ret;
}

esp_err_t wifi_deinit_sta(struct wifi_handle* handle) {
	esp_err_t ret;

	if (!handle->init) {
		return ESP_ERR_INVALID_STATE;
	}

	ret = esp_wifi_disconnect();
	check_err(ret, "wifi disconnect", out);

	esp_wifi_stop();
	ret = esp_wifi_deinit();
	check_err(ret, "wifi deinit", out);

	ret = nvs_flash_deinit();
	check_err(ret, "nvs flash deinit", out)

	esp_netif_destroy_default_wifi((void*)handle->netif);

	esp_netif_deinit();

	esp_event_loop_delete_default();

	handle->init = false;

out:
	return ret;
}



