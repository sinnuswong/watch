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

#include <app_common.h>
#include <Elementary.h>
#include <efl_extension.h>
#include "buddyui.h"
#include "data.h"

#define ITEM_COUNT 15
#define CENTER_OF_X_AXIS 180
#define CENTER_OF_Y_AXIS 180

typedef struct _item_data_s {
	char *name;
	char *number;
	char *image_path;
} item_data_s;

typedef struct _position_info_s {
	int deg;
	int radius;
	int x;
	int y;
	int w;
	int h;
	int opacity;
	int font_size;
} position_info_s;

static item_data_s item_data[ITEM_COUNT] = {
	/*
	 * Name, Call number, Image path
	 */
	{ "Geunsun Lee", "010-1234-5678", "NULL" },
	{ "Soofeel Kim", "010-3344-5566", "NULL" },
	{ "Yongkook Kim", "010-1122-3344", "NULL" },
	{ "Moonkyoung Park", "010-2233-4455", "NULL" },
	{ "Boyeon Son", "010-4455-6677", "NULL" },
	{ "Hyerim Kim", "010-6677-8899", "NULL" },
	{ "Heonjae Jang", "010-7788-9900", "NULL" },
	{ "Junkyu Han", "010-8899-0011", "NULL" },
	{ "Seongmoon Jeong", "010-0000-0000", "NULL" },
	{ "Yoon Jin", "010-0000-0000", "NULL" },
	{ "Jinyoung Kim", "010-0000-0000", "NULL" },
	{ "Younghun Jo", "010-0000-0000", "NULL" },
	{ "Juhee Park", "010-0000-0000", "NULL" },
	{ "Hyoeun Ahn", "010-5566-7788", "images/buddy.png" },
	{ "Youngjoo Park", "010-0000-0000", "NULL" },
};

static position_info_s position_info[11] = {
	/*
	 * degree, radius, x, y, w, h, opacity, font_size
	 */
	{ 30, 150, 309, 254, 27, 27, 255 * 0.2, 14 }, //bottom
	{ 51, 145, 271, 292, 36, 36, 255 * 0.4, 18 },
	{ 76, 145, 215, 320, 40, 40, 255 * 0.6, 20 },
	{ 104, 144, 145, 319, 46, 46, 255 * 0.8, 22 },
	{ 138, 139, 76, 273, 56, 56, 255, 29 },
	{ 180, 126, 54, 180, 76, 76, 255, 48 }, //center
	{ 222, 139, 76, 86, 56, 56, 255, 29 },
	{ 256, 144, 145, 40, 46, 46, 255 * 0.8, 22 },
	{ 284, 145, 215, 39, 40, 40, 255 * 0.6, 20 },
	{ 309, 145, 271, 67, 36, 36, 255 * 0.4, 18 },
	{ 330, 150, 309, 104, 27, 27, 255 * 0.2, 14 }, //top
};

static struct data_info {
	Eina_List *item_list;
} s_info = {
	.item_list = NULL,
};

/**
 * @brief Get the list that contains item.
 */
Eina_List *data_get_item_list(void)
{
	return s_info.item_list;
}

/**
 * @brief Set the list that contains item.
 * @param[in] list The item list to which you want to set
 */
void data_set_item_list(Eina_List *list)
{
	s_info.item_list = list;
}

/**
 * @brief Initialization function for data module.
 */
void data_initialize(void)
{
	/*
	 * If you need to initialize managing data,
	 * please use this function.
	 */
}

/**
 * @brief Finalization function for data module.
 */
void data_finalize(void)
{
	/*
	 * If you need to finalize managing data,
	 * please use this function.
	 */
}

/**
 * @brief Get path of resource.
 * @param[in] file_in File name
 * @param[in] file_path_out The point to which save full path of the resource
 * @param[in] file_path_max Size of file name include path
 */
void data_get_resource_path(const char *file_in, char *file_path_out, int file_path_max)
{
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(file_path_out, file_path_max, "%s%s", res_path, file_in);
		free(res_path);
	}
}

/**
 * @brief Get name of item information.
 * @param[in] index The order of position
 */
char *data_get_item_name(int index)
{
	char buf[128] = { 0, };
	snprintf(buf, sizeof(buf), "%s", item_data[index].name);
	return strdup(buf);
}

/**
 * @brief Get image of item information.
 * @param[in] index The order of position
 */
char *data_get_item_image(int index)
{
	char buf[PATH_MAX] = { 0, };
	if (!strcmp(item_data[index].image_path, "NULL")) {
		snprintf(buf, sizeof(buf), "%s", item_data[index].image_path);
	} else {
		data_get_resource_path(item_data[index].image_path, buf, (int)PATH_MAX);
	}
	return strdup(buf);
}

/**
 * @brief Get call number of item information.
 * @param[in] index The order of position
 */
char *data_get_item_number(int index)
{
	char buf[128] = { 0, };
	snprintf(buf, sizeof(buf), "%s", item_data[index].number);
	return strdup(buf);
}

/**
 * @brief Get total count of item to display.
 */
int data_get_item_total_count(void)
{
	int total = 0;

	total = ITEM_COUNT;

	return total;
}

/**
 * @brief Get next index by rotary event direction.
 * @param[in] direction Rotary event direction
 * @param[in] index The order of position
 */
int data_get_next_index(Eext_Rotary_Event_Direction direction, int index)
{
	return (direction == EEXT_ROTARY_DIRECTION_CLOCKWISE) ? index + 1 : index - 1;
}

/**
 * @brief Get information of position.
 * @param[in] index The order of position
 * @param[out] x The pointer to an integer in which to store the X coordinate of the object
 * @param[out] y The pointer to an integer in which to store the Y coordinate of the object
 * @param[out] w The pointer to an integer in which to store the width of the object
 * @param[out] h The pointer to an integer in which to store the height of the object
 * @param[out] opacity The pointer to an integer in which to store the opacity of the object
 */
void data_get_position_info(int index, int *x, int *y, int *w, int *h, int *opacity)
{
	*x = position_info[index].x;
	*y = position_info[index].y;
	*w = position_info[index].w;
	*h = position_info[index].h;
	*opacity = position_info[index].opacity;
}

/**
 * @brief Get font size of position.
 * @param[in] index The order of position
 */
int data_get_position_font_size(int index)
{
	return position_info[index].font_size;
}

/**
 * @brief Get degree of position.
 * @param[in] index The order of position
 */
int data_get_position_degree(int index)
{
	return position_info[index].deg;
}

/**
 * @brief Get radius of position.
 * @param[in] index The order of position
 */
int data_get_position_radius(int index)
{
	return position_info[index].radius;
}

/**
 * @brief Get simple circle path coordinate to set item layout for simple circle path animation.
 * @param[out] nx X-coordinate
 * @param[out[ ny Y-coordinate
 * @param[in] radius Radius from origin point
 * @param[in] deg Degree of item layout
 */
void data_get_simple_circle_path(int *nx, int *ny, float radius, int deg)
{
	int center_x = CENTER_OF_X_AXIS;
	int center_y = CENTER_OF_Y_AXIS;
	float radian = deg * PI / 180;

	*nx = cos(radian) * radius + center_x;
	*ny = sin(radian) * radius + center_y;
}

/**
 * @brief Get size to set item layout for simple circle path animation.
 * @param[out] item_w Current width of item layout
 * @param[out] item_h Current height of item layout
 * @param[in] from_w Width of item layout at start position
 * @param[in] from_h Height of item layout at start position
 * @param[in] to_w Width of item layout at end position
 * @param[in] to_h Height of item layout at end position
 * @param[in] deg_delta Degree delta from start position to end position
 */
void data_get_item_size(float *item_w, float *item_h, int from_w, int from_h, int to_w, int to_h, float deg_delta)
{
	float w_size_delta = (fabs((float) from_w - (float) to_w) * (1 / deg_delta)) * (deg_delta / FRAME_MAX);
	float h_size_delta = (fabs((float) from_h - (float) to_h) * (1 / deg_delta)) * (deg_delta / FRAME_MAX);

	if (from_w > to_w) {
		*item_w -= w_size_delta;
		*item_h -= h_size_delta;
	} else {
		*item_w += w_size_delta;
		*item_h += h_size_delta;
	}
}

/**
 * @brief Get angle to set item layout for simple circle path animation.
 * @param[in] from_deg Current degree of item layout
 * @param[in] to_deg Degree of item layout at end position
 * @param[in] deg_delta Degree delta from start position to end position
 * @param[in] frame Current frame of simple circle path animation
 */
float data_get_item_angle(int from_deg, int to_deg, float deg_delta, int frame)
{
	float angle = 0.0f;

	if (from_deg > to_deg) {
		angle = from_deg - (float) (deg_delta * frame / FRAME_MAX);
	} else {
		angle = from_deg + (float) (deg_delta * frame / FRAME_MAX);
	}

	return angle;
}

/**
 * @brief Get radius to set item layout for simple circle path animation.
 * @param[in] index Current index of item layout
 * @param[in] direction Rotary event direction
 * @param[in] frame Current frame of simple circle path animation
 */
int data_get_item_radius(int index, Eext_Rotary_Event_Direction direction, int frame)
{
	int cur_radius = 0;
	int from_radius = data_get_position_radius(index);
	int to_radius = data_get_position_radius(data_get_next_index(direction, index));
	int radius_delta = abs(from_radius - to_radius);

	if (from_radius > to_radius) {
		cur_radius = from_radius - (radius_delta * frame / FRAME_MAX);
	} else {
		cur_radius = from_radius + (radius_delta * frame / FRAME_MAX);
	}

	return cur_radius;
}

/**
 * @brief Get opacity to set item layout for simple circle path animation.
 * @param[in] item The item that you want to get opacity
 * @param[in] from_opacity Current opacity of item layout
 * @param[in] to_opacity Opacity of item layout at end position
 * @param[in] deg_delta Degree delta from start position to end position
 */
int data_get_item_opacity(Evas_Object *item, int from_opacity, int to_opacity, float deg_delta)
{
	int opacity = 0;
	int cur_opacity = 0;
	int opacity_delta = fabs((float) from_opacity - to_opacity) * (1 / deg_delta);

	evas_object_color_get(item, NULL, NULL, NULL, &cur_opacity);

	if (from_opacity > to_opacity) {
		opacity = cur_opacity - opacity_delta;
	} else {
		opacity = cur_opacity + opacity_delta;
	}

	return opacity;
}

/**
 * @brief Sorts a list according to the ordering.
 * @param[in] d1 Data to compare
 * @param[in] d2 Data to compare
 */
static int _sort_list_cb(const void *d1, const void *d2)
{
	const Evas_Object *item1 = d1;
	const Evas_Object *item2 = d2;
	item_info_s *item_info1 = NULL;
	item_info_s *item_info2 = NULL;

	if (!item1) return 1;
	if (!item2) return -1;

	item_info1 = evas_object_data_get(item1, "__ITEM_INFO__");
	item_info2 = evas_object_data_get(item2, "__ITEM_INFO__");

	return (strcmp(item_info1->name, item_info2->name));
}

/**
 * @brief Push item layout to item list.
 * @param[out] list Item list that contains item layouts
 * @param[in] user_data Item layout
 */
void data_push_item_to_list(Eina_List **list, void *user_data)
{
	*list = eina_list_sorted_insert(*list, _sort_list_cb, (void *) user_data);
}
