/*
 * custom_ui.h
 *
 *  Created on: Mar 30, 2020
 *      Author: sinnus
 */
#include <app_control.h>
#include <efl_extension_events.h>
#include <tizen.h>
#ifndef CUSTOM_UI_H_
#define CUSTOM_UI_H_

typedef struct appdata1 {

//	char **value;
} appdata_s1;
char str_buffer[PATH_MAX];

void win_custom_perform_click(void *data,Evas_Object *obj);
void show_image(void*data, char *image_path, int width, int height, int x, int y,int tag);
void get_size(char * path,int* w,int *h);
void start_custom(void *data);
void end_custom(void *data);
#endif /* CUSTOM_UI_H_ */
