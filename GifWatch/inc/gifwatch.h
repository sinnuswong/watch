/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_GIFWATCH_H_)
#define _GIFWATCH_H_

#include <watch_app.h>
#include <watch_app_efl.h>
#include <Elementary.h>
#include <dlog.h>
#include <cairo.h>
#include "custom_ui.h"
#if !defined(PACKAGE)
#define PACKAGE "org.eros.gifwatch"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "gifwatch"

typedef struct appdata {
	/* Variables for basic UI contents */
	Evas_Object *win;
	int width;
	int height;
	Evas_Object *img;
	int img_width;
	int img_height;
	int img_location_x;
	int img_location_y;

	Evas_Object * label_fps;
	Evas_Object * increase_img;
	Evas_Object * decrease_img;
	Evas_Object * bt_pick;

	Evas_Object * bigger_image;
	Evas_Object * smaller_image;
	float scale;
	int tmp_width;
	int tmp_height;
	int tmp_location_x;
	int tmp_location_y;

	Ecore_Event_Handler * handler_up;
	Ecore_Event_Handler * handler_down;
	Ecore_Event_Handler * handler_move;
	int isDown;
	int down_x;
	int down_y;

	//touch point move prev x,y
	int move_prev_x;
	int move_prev_y;
	int move_dx;
	int move_dy;

	int img_moved_to_location_x;
	int img_moved_to_location_y;

	int moved_which; //which object has been moved ,0 is no object moved

	int upx;
	int upy;
	//int up_count;
	int last_up_million;

	Evas_Object *label_time;
	int label_time_width;
	int label_time_height;
	int label_time_location_x;
	int label_time_location_y;

	Evas_Object * label_date;
	int label_date_width;
	int label_date_height;
	int label_date_location_x;
	int label_date_location_y;
	Ecore_Timer *timer;
	char *res_path;
	/* Variables for watch size information */

	watch_time_h *watch_time;
	int hour, minute, second, msecond;
	int year, month, day, day_of_week;
	/* Variables for cairo image backend contents */
	cairo_t *cairo;
	cairo_surface_t *surface;
	unsigned char *pixels;
	int resume;
	int orient;
	double x, y;
	int frame_count;
	int an;

	int ambient_mode;

	char ** value;
	int key_back_tag;
	float interval;
	int fps;
	int current_win;
	int status_changed;
} appdata_s;

static int image_increase_width = 50;
static int image_increase_height = 50;
static int image_decrease_width = 50;
static int image_decrease_height = 50;

static int image_decrease_locate_x = 120 - 50 / 2;
static int image_decrease_locate_y = 280 - 50 / 2;
static int image_increase_locate_x = 240 - 50 / 2;
static int image_increase_locate_y = 280 - 50 / 2;

static int label_fps_width = 50;
static int label_fps_height = 30;
static int label_fps_location_x = 180 - 50 / 2;
static int label_fps_location_y = 280 - 30 / 2;

static int bt_pick_width = 130 ;
static int bt_pick_height = 50;
static int bt_pick_location_x = 180-130/2;
static int bt_pick_location_y = 310;
static int ic_width = 50;

void addImage(void *a, char *image_path, int width, int height, int x, int y);
int which_object(void *a, int x, int y);
Eina_Bool event_up(void *data, int type, void *ev);
#endif
