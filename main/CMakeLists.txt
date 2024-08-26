idf_component_register(SRCS "main.c" "wifi.c" "handlers.c" "webserver.c" "db.c" "button.c" "relay.c"
                    INCLUDE_DIRS "."
		    PRIV_REQUIRES esp_netif esp_wifi esp_event nvs_flash lwip esp_http_server esp_driver_gpio
		    EMBED_TXTFILES index.html
	    )
