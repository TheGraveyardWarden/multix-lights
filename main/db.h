#ifndef _DB_H
#define _DB_H

#include "esp_err.h"
#include "button.h"
#include <stdint.h>


esp_err_t db_init(void);
void db_deinit(void);

esp_err_t db_write_str(const char* key, const char* value);
esp_err_t db_read_str(const char* key, char** value);

esp_err_t db_read_btns(button_t** btns, size_t* size);
esp_err_t db_write_btns(button_t* btns, size_t size);
esp_err_t db_write_btn(button_t* btn, int idx);

#endif
