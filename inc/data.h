/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_DATA_H)
#define _DATA_H

#define PI 3.14159
#define FRAME_MAX 10

typedef struct _item_info_s {
	int position;
	char *name;
	char *number;
	char *img_path;
	float w;
	float h;
} item_info_s;

/*
 * Initialize the data component
 */
void data_initialize(void);

/*
 * Finalize the data component
 */
void data_finalize(void);

Eina_List *data_get_item_list(void);
void data_set_item_list(Eina_List *list);

void data_get_resource_path(const char *file_in, char *file_path_out, int file_path_max);
char *data_get_item_name(int index);
char *data_get_item_image(int index);
char *data_get_item_number(int index);
int data_get_item_total_count(void);
int data_get_next_index(Eext_Rotary_Event_Direction direction, int index);
void data_get_position_info(int index, int *x, int *y, int *w, int *h, int*opacity);
int data_get_position_font_size(int index);
int data_get_position_degree(int index);
int data_get_position_radius(int index);
void data_get_simple_circle_path(int *nx, int *ny, float radius, int deg);
void data_get_item_size(float *item_w, float *item_h, int from_w, int from_h, int to_w, int to_h, float deg_delta);
float data_get_item_angle(int from_deg, int to_deg, float deg_delta, int frame);
int data_get_item_radius(int index, Eext_Rotary_Event_Direction direction, int frame);
int data_get_item_opacity(Evas_Object *item, int from_opacity, int to_opacity, float deg_delta);
void data_push_item_to_list(Eina_List **list, void *user_data);

#endif
