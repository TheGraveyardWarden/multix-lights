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

	*value = malloc(required_size * sizeof(char));
	err = nvs_get_str(my_handle, key, *value, &required_size);
	if (err != ESP_OK) goto cleanup;

out:
	return err;
cleanup:
	free(*value);
	return err;
}

