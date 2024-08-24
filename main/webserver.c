#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "webserver.h"
#include "db.h"

// ----------------------------------------- DEFINES
#define MAX_QUERY_KEY_LEN	256

// ----------------------------------------- DECLARATIONS
extern const uint8_t index_html[];

// ----------------------------------------- HANDLERS
esp_err_t root_get_handler(httpd_req_t* req)
{
	printf("/\n");

	httpd_resp_send(req, (const char*)index_html, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

esp_err_t switch_get_handler(httpd_req_t* req)
{
	printf("/switch\n");

	char* query;
	char query_key[MAX_QUERY_KEY_LEN];
	int btn_no;
	size_t query_len;

	btn_no = -1;
	query_len = httpd_req_get_url_query_len(req) + 1;
	if (query_len > 1)
	{
		query = malloc(query_len);
		if (httpd_req_get_url_query_str(req, query, query_len) == ESP_OK)
			if (httpd_query_key_value(query, "btn_no", query_key, sizeof(query_key)) == ESP_OK)
				btn_no = atoi(query_key);
		free(query);
	}

	printf("%d\n", btn_no);

	httpd_resp_send(req, "OK", 2);

	return ESP_OK;
}

// ---------------------------------------- URI CONFIGS
const httpd_uri_t root_uri = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = root_get_handler,
	.user_ctx = NULL
};

const httpd_uri_t switch_uri = {
	.uri = "/switch",
	.method = HTTP_GET,
	.handler = switch_get_handler,
	.user_ctx = NULL
};

// ---------------------------------------- PUBLIC
httpd_handle_t webserver_start(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	config.lru_purge_enable = true;

	printf("initializing db\n");
	if (db_init() != ESP_OK)
	{
		printf("failed to initialize db\n");
		return NULL;
	}

	printf("starting webserver\n");
	if (httpd_start(&server, &config) == ESP_OK)
	{
		printf("registering URI handlers\n");
		httpd_register_uri_handler(server, &root_uri);
		httpd_register_uri_handler(server, &switch_uri);
		return server;
	}

	printf("error starting webserver\n");
	return NULL;
}

esp_err_t webserver_stop(httpd_handle_t server)
{
	printf("closing db\n");
	db_deinit();
	printf("stopping webserver\n");
	return httpd_stop(server);
}

