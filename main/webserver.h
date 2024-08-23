#ifndef _WEBSERVER_H
#define _WEBSERVER_H

#include "esp_http_server.h"
#include "esp_err.h"


httpd_handle_t webserver_start(void);

esp_err_t webserver_stop(httpd_handle_t server);

#endif
