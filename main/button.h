#ifndef _BUTTON_H
#define _BUTTON_H

#include <stddef.h>

#define MAX_BUTTON_NAME_LEN 20

enum button_status {
	button_status_off = 0,
	button_status_on
};

typedef struct button {
	char name[MAX_BUTTON_NAME_LEN];
	enum button_status status;
} button_t;

#define BUTTON_INIT(_name, _status) { .name = _name, .status = _status }
#define INIT_BUTTON(btn) button_t btn = BUTTON_INIT("default_name", button_status_off);

void button_create(button_t* btn, const char* name, enum button_status status);

void button_print(button_t* btn);

void button_clone(button_t* to, button_t* from);

void button_to_json(button_t* btn, char** json);

void button_arr_to_json(button_t* btns, size_t size, char** json);

#endif
