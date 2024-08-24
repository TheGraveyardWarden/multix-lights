#ifndef _DB_H
#define _DB_H

#include "esp_err.h"


esp_err_t db_init(void);
void db_deinit(void);

esp_err_t db_write_str(const char* key, const char* value);
esp_err_t db_read_str(const char* key, char** value);

#endif
