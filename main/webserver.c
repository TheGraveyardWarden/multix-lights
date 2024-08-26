#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "webserver.h"
#include "db.h"
#include "relay.h"

// ----------------------------------------- DEFINES
#define MAX_QUERY_KEY_LEN	256
#define MAX_REQ_CONTENT_LEN	256

// ----------------------------------------- DECLARATIONS
extern const uint8_t index_html[];

// ----------------------------------------- HANDLERS
esp_err_t root_get_handler(httpd_req_t* req)
{
	printf("GET /\n");

	httpd_resp_send(req, (const char*)index_html, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

esp_err_t switch_get_handler(httpd_req_t* req)
{
	printf("GET /switch\n");

	char* query;
	char query_key[MAX_QUERY_KEY_LEN];
	int btn_no, new_status;
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

	if (btn_no > -1 && btn_no < NUM_RELAYS) {
		new_status = relay_switch(relays[btn_no]);
		db_update_btn_status(new_status, btn_no);
	}
	else {
		httpd_resp_set_status(req, HTTPD_400);
		httpd_resp_send(req, NULL, 0);
		return ESP_OK;
	}

	httpd_resp_send(req, "OK", 2);

	return ESP_OK;
}

esp_err_t buttons_get_handler(httpd_req_t* req)
{
	printf("GET /buttons\n");

	esp_err_t err;
	char* resp;
	button_t* btns;
	size_t nr_btns;

	err = db_read_btns(&btns, &nr_btns);
	if (err != ESP_OK || !nr_btns)
	{
		httpd_resp_set_status(req, HTTPD_400);
		httpd_resp_send(req, NULL, 0);
		goto out;
	}

	button_arr_to_json(btns, nr_btns, &resp);
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	free(resp);

out:
	free(btns);
	return ESP_OK;
}

esp_err_t buttons_post_handler(httpd_req_t* req)
{
	printf("POST /buttons\n");

	esp_err_t err;
	char content[MAX_REQ_CONTENT_LEN], query_key[MAX_QUERY_KEY_LEN];
	char* query, *resp;
	size_t query_len;
	int ret, idx;
	button_t btn;

	if ((ret = httpd_req_recv(req,
			   content,
			   req->content_len < MAX_REQ_CONTENT_LEN
			   ? req->content_len
			   : MAX_REQ_CONTENT_LEN)) <= 0)
		goto out_with_400;

	content[ret] = 0;

	if (button_parse_json(&btn, content) < 0)
	{
		printf("invalid data recvd\n");
		goto out_with_400;
	}

	idx = -1;
	query_len = httpd_req_get_url_query_len(req) + 1;
	if (query_len > 1)
	{
		query = malloc(query_len);
		if (httpd_req_get_url_query_str(req, query, query_len) == ESP_OK)
			if (httpd_query_key_value(query, "idx", query_key, sizeof(query_key)) == ESP_OK)
				idx = atoi(query_key);
		free(query);
	}

	err = db_write_btn(&btn, idx);
	if (err != ESP_OK) goto out_with_400;

	button_to_json(&btn, &resp);

	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

	free(resp);

	return ESP_OK;

out_with_400:
	httpd_resp_set_status(req, HTTPD_400);
	httpd_resp_send(req, NULL, 0);
	return ESP_OK;
}

// ---------------------------------------- URI CONFIGS
const httpd_uri_t root_get_uri = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = root_get_handler,
	.user_ctx = NULL
};

const httpd_uri_t switch_get_uri = {
	.uri = "/switch",
	.method = HTTP_GET,
	.handler = switch_get_handler,
	.user_ctx = NULL
};

const httpd_uri_t buttons_get_uri = {
	.uri = "/buttons",
	.method = HTTP_GET,
	.handler = buttons_get_handler,
	.user_ctx = NULL
};

const httpd_uri_t buttons_post_uri = {
	.uri = "/buttons",
	.method = HTTP_POST,
	.handler = buttons_post_handler,
	.user_ctx = NULL
};

// ---------------------------------------- PUBLIC
httpd_handle_t webserver_start(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	config.lru_purge_enable = true;

	printf("starting webserver\n");
	if (httpd_start(&server, &config) == ESP_OK)
	{
		printf("registering URI handlers\n");
		httpd_register_uri_handler(server, &root_get_uri);
		httpd_register_uri_handler(server, &switch_get_uri);
		httpd_register_uri_handler(server, &buttons_get_uri);
		httpd_register_uri_handler(server, &buttons_post_uri);
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

