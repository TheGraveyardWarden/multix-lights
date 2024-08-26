#include "db.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <stdint.h>

#define STORAGE_NAMESPACE "global"

nvs_handle my_handle;

esp_err_t db_init(void)
{
	esp_err_t err;

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);

	return err;
}

void db_deinit(void)
{
	nvs_close(my_handle);
}

esp_err_t db_write_str(const char* key, const char* value)
{
	esp_err_t err;

	err = nvs_set_str(my_handle, key, value);
	if (err != ESP_OK) goto out;

	err = nvs_commit(my_handle);

out:
	return err;
}

esp_err_t db_read_str(const char* key, char** value)
{
	esp_err_t err;
	size_t required_size;

	err = nvs_get_str(my_handle, key, NULL, &required_size);
	if (err != ESP_OK) goto out;

	*value = malloc(required_size);
	err = nvs_get_str(my_handle, key, *value, &required_size);
	if (err != ESP_OK) goto cleanup;

out:
	return err;
cleanup:
	free(*value);
	return err;
}

esp_err_t db_read_btns(button_t** btns, size_t* size)
{
	esp_err_t err;
	size_t required_size;

	*size = 0;

	err = nvs_get_blob(my_handle, "btns", NULL, &required_size);
	if (err != ESP_OK) goto out;

	*btns = malloc(required_size);
	err = nvs_get_blob(my_handle, "btns", *btns, &required_size);
	if (err != ESP_OK) goto cleanup;

	*size = required_size / sizeof(struct button);

out:
	return err;
cleanup:
	free(*btns);
	return err;
}

esp_err_t db_write_btns(button_t* btns, size_t size)
{
	esp_err_t err;

	err = nvs_set_blob(my_handle, "btns", btns, size * sizeof(struct button));
	if (err != ESP_OK) goto err;

	err = nvs_commit(my_handle);

err:
	return err;
}

esp_err_t db_write_btn(button_t* btn, int idx)
{
	esp_err_t err;
	size_t nr_btns;

	button_t* prev;
	err = db_read_btns(&prev, &nr_btns);
	if (err != ESP_OK) goto out;

	if (idx < (int)nr_btns) {
		button_clone(&prev[idx], btn);
		err = db_write_btns(prev, nr_btns);
		if (err != ESP_OK) goto cleanup;
	} else {
		err = ESP_FAIL;
		goto cleanup;
	}

	free(prev);

out:
	return err;
cleanup:
	free(prev);
	return err;
}

esp_err_t
db_update_btn_status(enum button_status status, int idx)
{
	esp_err_t err;
	size_t nr_btns;

	button_t* prev;
	err = db_read_btns(&prev, &nr_btns);
	if (err != ESP_OK) goto out;

	if (idx < (int)nr_btns) {
		prev[idx].status = status;
		err = db_write_btns(prev, nr_btns);
		if (err != ESP_OK) goto cleanup;
	} else {
		err = ESP_FAIL;
		goto cleanup;
	}

	free(prev);

out:
	return err;
cleanup:
	free(prev);
	return err;
}

