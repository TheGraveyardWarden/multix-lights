#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "webserver.h"

// ----------------------------------------- DEFINES
#define MAX_QUERY_KEY_LEN	256
#define MAX_BTN_NAME_LEN 	20
#define NR_BTNS			4
#define MAX_BTN_STATUS_LEN	4

// ----------------------------------------- DECLARATIONS
extern const uint8_t index_html[];
__NOINIT_ATTR char btn_names[NR_BTNS][MAX_BTN_NAME_LEN];
__NOINIT_ATTR char btn_status[NR_BTNS][MAX_BTN_STATUS_LEN];

void init_btns(void);

char* str_arr_to_json(const char* str_arr,
		      uint32_t size,
		      uint32_t el_size,
		      uint32_t* res_len);

// ----------------------------------------- HANDLERS
esp_err_t root_get_handler(httpd_req_t* req)
{
	printf("/\n");
	httpd_resp_send(req, (const char*)index_html, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

esp_err_t btn_names_get_handler(httpd_req_t* req)
{
	printf("/btn_names\n");
	char* resp;
	uint32_t total_len;

	resp = str_arr_to_json((const char*)btn_names,
				NR_BTNS,
				MAX_BTN_NAME_LEN,
				&total_len);

	httpd_resp_set_hdr(req, "Content-Type", "application/json");
	httpd_resp_send(req, resp, total_len - 1);

	free(resp);

	return ESP_OK;
}

esp_err_t btn_status_get_handler(httpd_req_t* req)
{
	printf("/btn_status\n");
	char* resp;
	uint32_t total_len;

	resp = str_arr_to_json((const char*)btn_status,
				NR_BTNS,
				MAX_BTN_STATUS_LEN,
				&total_len);

	httpd_resp_set_hdr(req, "Content-Type", "application/json");
	httpd_resp_send(req, resp, total_len - 1);

	free(resp);

	return ESP_OK;
}

esp_err_t switch_get_handler(httpd_req_t* req)
{
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

	if (btn_no < 0 || btn_no > NR_BTNS - 1)
	{
		httpd_resp_set_status(req, HTTPD_400);
		httpd_resp_send(req, NULL, 0);
	} else
	{
		httpd_resp_send(req, "OK", 2);
	}

	return ESP_OK;
}

// ---------------------------------------- URI CONFIGS
const httpd_uri_t root_uri = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = root_get_handler,
	.user_ctx = NULL
};

const httpd_uri_t btn_names_uri = {
	.uri = "/btn_names",
	.method = HTTP_GET,
	.handler = btn_names_get_handler,
	.user_ctx = NULL
};

const httpd_uri_t btn_status_uri = {
	.uri = "/btn_status",
	.method = HTTP_GET,
	.handler = btn_status_get_handler,
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

	init_btns();

	printf("starting webserver\n");
	if (httpd_start(&server, &config) == ESP_OK)
	{
		printf("registering URI handlers\n");
		httpd_register_uri_handler(server, &root_uri);
		httpd_register_uri_handler(server, &btn_names_uri);
		httpd_register_uri_handler(server, &btn_status_uri);
		httpd_register_uri_handler(server, &switch_uri);
		return server;
	}

	printf("error starting webserver\n");
	return NULL;
}

esp_err_t webserver_stop(httpd_handle_t server)
{
	printf("stopping webserver\n");
	return httpd_stop(server);
}

// ---------------------------------------- UTILS
void init_btns(void)
{
	uint8_t i;

	for (i = 0; i < NR_BTNS; i++)
	{
		strncpy(btn_names[i], "Relay #", 7);
		btn_names[i][7] = i+0x31;
		btn_names[i][8] = 0;

		strncpy(btn_status[i], "OFF", 3);
		btn_status[i][3] = 0;
	}

	for (i = 0; i < NR_BTNS; i++)
	{
		printf("btn #%d:\t%s | %s\n", i + 1, btn_names[i], btn_status[i]);
	}
}

char* str_arr_to_json(const char* str_arr,
		      uint32_t size,
		      uint32_t el_size,
		      uint32_t* res_len)
{
	char* resp, *tmp;
	uint8_t i, total_len;
	
	total_len = 2;
	for (i = 0; i < size; i++)
		total_len += strlen(str_arr + (i * el_size)) + 2;
	total_len += size;

	resp = malloc(total_len * sizeof(char));
	tmp = resp;

	*resp++ = '[';
	for (i = 0; i < size; i++)
	{
		*resp++ = '"';
		strncpy(resp, str_arr + (i * el_size), strlen(str_arr + (i * el_size)));
		resp += strlen(str_arr + (i * el_size));
		*resp++ = '"';
		if (i < size - 1) *resp++ = ',';
	}
	*resp++ = ']';
	*resp = 0;

	*res_len = total_len;

	return tmp;
}

