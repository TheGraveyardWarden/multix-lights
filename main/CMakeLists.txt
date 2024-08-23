idf_component_register(SRCS "main.c" "wifi.c" "handlers.c"
                    INCLUDE_DIRS "."
		    PRIV_REQUIRES esp_netif esp_wifi esp_event nvs_flash lwip
	    )
