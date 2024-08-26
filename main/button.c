#include "button.h"
#include <string.h>
#include <stdio.h>


void button_create(button_t* btn, const char* name, enum button_status status)
{
	strncpy(btn->name, name, MAX_BUTTON_NAME_LEN);
	if (strlen(name) > MAX_BUTTON_NAME_LEN - 1)
		btn->name[MAX_BUTTON_NAME_LEN - 1] = 0;
	btn->status = status;
}

void button_print(button_t* btn)
{
	printf("button { name: %s, status: %s }\n",
		btn->name,
		btn->status == button_status_off ? "off" : "on");
}

void button_to_json(button_t* btn, char** json)
{
	size_t total_size, btn_name_len;
	char* buff;

	btn_name_len = strlen(btn->name);

	// {"name":"shit","status":1}
	total_size = 23 + btn_name_len;

	*json = malloc(total_size);
	buff = *json;

	strncpy(buff, "{\"name\":\"", 9);
	buff += 9;

	strncpy(buff, btn->name, btn_name_len);
	buff += btn_name_len;
	*buff++ = '"';

	strncpy(buff, ",\"status\":", 10);
	buff += 10;

	*buff++ = btn->status == button_status_off ? '0' : '1';

	*buff++ = '}';
	*buff = 0;
}

void button_arr_to_json(button_t* btns, size_t size, char** json)
{
	// [{"name":"shit","status":0},{"name":"btn2","status":1}]
	char* items[size];
	size_t total_size, i;
	char* buff;

	for (i = 0; i < size; i++)
		button_to_json(&btns[i], &items[i]);

	total_size = 2 + size;
	for (i = 0; i < size; i++)
		total_size += strlen(items[i]);

	*json = malloc(total_size);
	buff = *json;

	*buff++ = '[';
	for (i = 0; i < size; i++) {
		strncpy(buff, items[i], strlen(items[i]));
		buff += strlen(items[i]);
		if (i < size - 1) *buff++ = ',';
	}
	*buff++ = ']';
	*buff = 0;

	for (i = 0; i < size; i++)
		free(items[i]);
}

int button_parse_json(button_t* btn, char* json)
{
	int json_len, name_len;
	char *t1, *t2, name[MAX_BUTTON_NAME_LEN], status_str[2];
	enum button_status status;

	json_len = strlen(json);
	if (!json_len || json[0] != '{' || json[json_len-1] != '}')
		return -1;

	// {"name":"shit","status":1}
	t1 = strstr(json, "\"name\"");
	if (!t1) return -1;
	t1 += 6;
	t1 = strstr(t1, "\"");
	if (!t1) return -1;
	t1++;
	t2 = strstr(t1, "\"");
	if (!t2) return -1;
	name_len = t2 - t1 < MAX_BUTTON_NAME_LEN - 1 ? t2 - t1 : MAX_BUTTON_NAME_LEN - 1;
	strncpy(name, t1, name_len);
	name[name_len] = 0;

	t1 = strstr(json, "\"status\":");
	if (!t1) return -1;
	t1 += 9;
	while (*t1 == ' ') t1++;
	*status_str = *t1;
	status_str[1] = 0;
	status = atoi(status_str) == 0 ? button_status_off : button_status_on;

	button_create(btn, name, status);
	return 0;
}

