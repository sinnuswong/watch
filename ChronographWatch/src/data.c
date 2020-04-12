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
#include <dlog.h>

#include "chronographwatch.h"
#include "data.h"

#define IMAGE_HANDS_SEC "images/chrono_s_60_hand.png"
#define IMAGE_HANDS_MIN "images/chrono_hand_min.png"
#define IMAGE_HANDS_HOUR "images/chrono_hand_hour.png"
#define IMAGE_HANDS_SEC_SHADOW "images/chrono_s_60_hand_shadow.png"
#define IMAGE_HANDS_MIN_SHADOW "images/chrono_hand_min_shadow.png"
#define IMAGE_HANDS_HOUR_SHADOW "images/chrono_hand_hour_shadow.png"
#define IMAGE_HANDS_STOPWATCH_SEC "images/chrono_stopwatch_hand_sec.png"
#define IMAGE_HANDS_STOPWATCH_30S "images/chrono_ss_30_hand.png"
#define IMAGE_HANDS_STOPWATCH_12H "images/chrono_h_12_hand.png"

/*
 * @brief Get path of resource.
 * @param[in] file_in File name
 * @param[out] file_path_out The point to which save full path of the resource
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

/*
 * @brief Initialization function for data module.
 */
void data_initialize(void)
{
	/*
	 * If you need to initialize managing data,
	 * please use this function.
	 */
}

/*
 * @brief Finalization function for data module.
 */
void data_finalize(void)
{
	/*
	 * If you need to finalize managing data,
	 * please use this function.
	 */
}

/*
 * @brief Get a image path of the part.
 * @param[in] type The part type
 */
char *data_get_parts_image_path(parts_type_e type)
{
	char image_path[PATH_MAX] = { 0, };
	char *resource_image = NULL;

	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
		resource_image = IMAGE_HANDS_SEC;
		break;
	case PARTS_TYPE_HANDS_MIN:
		resource_image = IMAGE_HANDS_MIN;
		break;
	case PARTS_TYPE_HANDS_HOUR:
		resource_image = IMAGE_HANDS_HOUR;
		break;
	case PARTS_TYPE_HANDS_SEC_SHADOW:
		resource_image = IMAGE_HANDS_SEC_SHADOW;
		break;
	case PARTS_TYPE_HANDS_MIN_SHADOW:
		resource_image = IMAGE_HANDS_MIN_SHADOW;
		break;
	case PARTS_TYPE_HANDS_HOUR_SHADOW:
		resource_image = IMAGE_HANDS_HOUR_SHADOW;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_SEC:
		resource_image = IMAGE_HANDS_STOPWATCH_SEC;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_30S:
		resource_image = IMAGE_HANDS_STOPWATCH_30S;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_12H:
		resource_image = IMAGE_HANDS_STOPWATCH_12H;
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		return NULL;
	}

	data_get_resource_path(resource_image, image_path, sizeof(image_path));

	return strdup(image_path);
}

/*
 * @brief Get a position of the part.
 * @param[in] type The part type
 * @param[out] position_x The pointer to an integer in which to store the X coordinate of the part
 * @param[out] position_y The pointer to an integer in which to store the Y coordinate of the part
 */
void data_get_parts_position(parts_type_e type, int *position_x, int *position_y)
{
	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
		*position_x = HANDS_SEC_PIVOT_POSITION_X - (HANDS_SEC_WIDTH / 2);
		*position_y = HANDS_SEC_POSITION_Y;
		break;
	case PARTS_TYPE_HANDS_MIN:
		*position_x = (BASE_WIDTH / 2) - (HANDS_MIN_WIDTH / 2);
		*position_y = 0;
		break;
	case PARTS_TYPE_HANDS_HOUR:
		*position_x = (BASE_WIDTH / 2) - (HANDS_HOUR_WIDTH / 2);
		*position_y = 0;
		break;
	case PARTS_TYPE_HANDS_SEC_SHADOW:
		*position_x = HANDS_SEC_PIVOT_POSITION_X - (HANDS_SEC_WIDTH / 2);
		*position_y = HANDS_SEC_POSITION_Y;
		break;
	case PARTS_TYPE_HANDS_MIN_SHADOW:
		*position_x = (BASE_WIDTH / 2) - (HANDS_MIN_WIDTH / 2);
		*position_y = HANDS_MIN_SHADOW_PADDING;
		break;
	case PARTS_TYPE_HANDS_HOUR_SHADOW:
		*position_x = (BASE_WIDTH / 2) - (HANDS_HOUR_WIDTH / 2);
		*position_y = HANDS_HOUR_SHADOW_PADDING;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_SEC:
		*position_x = (BASE_WIDTH /2) - (HANDS_SW_SEC_WIDTH / 2);
		*position_y = 0;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_30S:
		*position_x = (BASE_WIDTH /2) - (HANDS_SEC_WIDTH / 2);
		*position_y = HANDS_STOPWATCH_30S_PIVOT_POSITION_Y - (HANDS_SEC_HEIGHT /2);
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_12H:
		*position_x = (BASE_WIDTH /2) - (HANDS_SEC_WIDTH / 2);
		*position_y = HANDS_STOPWATCH_12H_PIVOT_POSITION_Y - (HANDS_SEC_HEIGHT /2);
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		break;
	}

}

/*
 * @brief Get a width size of the part.
 * @param[in] type The part type
 */
int data_get_parts_width_size(parts_type_e type)
{
	int parts_width = 0;

	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
	case PARTS_TYPE_HANDS_SEC_SHADOW:
	case PARTS_TYPE_HANDS_STOPWATCH_30S:
	case PARTS_TYPE_HANDS_STOPWATCH_12H:
		parts_width = HANDS_SEC_WIDTH;
		break;
	case PARTS_TYPE_HANDS_MIN:
	case PARTS_TYPE_HANDS_MIN_SHADOW:
		parts_width = HANDS_MIN_WIDTH;
		break;
	case PARTS_TYPE_HANDS_HOUR:
	case PARTS_TYPE_HANDS_HOUR_SHADOW:
		parts_width = HANDS_HOUR_WIDTH;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_SEC:
		parts_width = HANDS_SW_SEC_WIDTH;
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		break;
	}

	return parts_width;
}

/*
 * @brief Get a height size of the part.
 * @param[in] type The part type
 */
int data_get_parts_height_size(parts_type_e type)
{
	int parts_height = 0;

	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
	case PARTS_TYPE_HANDS_SEC_SHADOW:
	case PARTS_TYPE_HANDS_STOPWATCH_30S:
	case PARTS_TYPE_HANDS_STOPWATCH_12H:
		parts_height = HANDS_SEC_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_MIN:
	case PARTS_TYPE_HANDS_MIN_SHADOW:
		parts_height = HANDS_MIN_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_HOUR:
	case PARTS_TYPE_HANDS_HOUR_SHADOW:
		parts_height = HANDS_HOUR_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_SEC:
		parts_height = HANDS_SW_SEC_HEIGHT;
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		break;
	}

	return parts_height;
}
