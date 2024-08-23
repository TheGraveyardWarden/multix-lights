#ifndef _WIFI_H
#define _WIFI_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_event.h"
#include "esp_err.h"
#include "handlers.h"
#include "esp_netif_ip_addr.h"
#include "lwip/ip_addr.h"

#define check_err(ret, msg, label)	\
	if (ret != ESP_OK) {	\
		printf("%s: %s\n", msg, esp_err_to_name(ret));	\
		goto label;	\
	}

struct event_loop_config {
	esp_event_handler_t on_start;
	esp_event_handler_t on_stop;
	esp_event_handler_t on_connect;
	esp_event_handler_t on_disconnect;
	esp_event_handler_t on_got_ip;
	esp_event_handler_t on_lost_ip;
};

#define EVENT_LOOP_CONFIG_DEFAULT() \
	(struct event_loop_config) {	\
		.on_start = default_wifi_start_handler,	\
		.on_stop = NULL,			\
		.on_disconnect = NULL, 			\
		.on_connect = NULL,			\
		.on_got_ip = NULL,			\
		.on_lost_ip = NULL,			\
	}

#define WIFI_CONFIG_STA_DEFAULT(_ssid, _password) (wifi_config_t) {.sta = {.ssid = _ssid, .password = _password}}

struct static_ip {
	esp_netif_ip_info_t ip_info;
};

static inline void static_ip_set_ip(struct static_ip* static_ip, const unsigned char ip[4] ) {
	IP4_ADDR(&static_ip->ip_info.ip, ip[0], ip[1], ip[2], ip[3]);
}

static inline void static_ip_set_gw(struct static_ip* static_ip, const unsigned char gw[4] ) {
	IP4_ADDR(&static_ip->ip_info.gw, gw[0], gw[1], gw[2], gw[3]);
}

static inline void static_ip_set_netmask(struct static_ip* static_ip, const unsigned char netmask[4] ) {
	IP4_ADDR(&static_ip->ip_info.netmask, netmask[0], netmask[1], netmask[2], netmask[3]);
}

struct wifi_init_config {
	struct event_loop_config event_loop_config;
	wifi_init_config_t wifi_init_config;
	wifi_config_t wifi_config;
	struct static_ip* static_ip;
};

#define _WIFI_INIT_CONFIG_DEFAULT(ssid, password) {			\
		.event_loop_config = EVENT_LOOP_CONFIG_DEFAULT(),	\
		.wifi_init_config = WIFI_INIT_CONFIG_DEFAULT(),		\
		.wifi_config = WIFI_CONFIG_STA_DEFAULT(ssid, password),	\
		.static_ip = NULL,					\
	}

inline void wifi_set_on_start(struct wifi_init_config* config, esp_event_handler_t handler) {
	config->event_loop_config.on_start = handler;
}

inline void wifi_set_on_stop(struct wifi_init_config* config, esp_event_handler_t handler) {
	config->event_loop_config.on_stop = handler;
}

inline void wifi_set_on_disconnect(struct wifi_init_config* config, esp_event_handler_t handler) {
	config->event_loop_config.on_disconnect = handler;
}

inline void wifi_set_on_connect(struct wifi_init_config* config, esp_event_handler_t handler) {
	config->event_loop_config.on_connect = handler;
}

inline void wifi_set_on_got_ip(struct wifi_init_config* config, esp_event_handler_t handler) {
	config->event_loop_config.on_got_ip = handler;
}

inline void wifi_set_on_lost_ip(struct wifi_init_config* config, esp_event_handler_t handler) {
	config->event_loop_config.on_lost_ip = handler;
}

inline void wifi_set_static_ip(struct wifi_init_config* config, struct static_ip* s_ip) {
	config->static_ip = s_ip;
}

struct wifi_handle {
	esp_netif_t* netif;
	bool init;
};

#define DEFINE_WIFI_HANDLE(name) struct wifi_handle name = {.netif = NULL, .init = false};

esp_err_t wifi_init_sta(struct wifi_handle*, struct wifi_init_config*);
esp_err_t wifi_deinit_sta(struct wifi_handle*);

#endif
