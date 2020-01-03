/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
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
#include <device/battery.h>

#include "weatherwatch.h"
#include "data.h"

#define IMAGE_HANDS_SEC "images/second_indicator.png"
#define IMAGE_HANDS_MIN "images/minute_indicator.png"
#define IMAGE_HANDS_HOUR "images/hour_indicator.png"
#define IMAGE_HANDS_PIN "images/pin.png"

/**
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
 * @brief Get plus angle for the hour hand.
 * @param[in] minute Current minute
 */
int data_get_plus_angle(int minute)
{
	int angle = 0;

	if (minute >= 0 && minute < 12) {
		angle = 0;
	} if (minute >= 12 && minute < 24) {
		angle = 6;
	} else if (minute >= 24 && minute < 36) {
		angle = 12;
	} else if (minute >= 36 && minute < 48) {
		angle = 18;
	} else if (minute >= 48 && minute < 60) {
		angle = 24;
	}

	return angle;
}

/**
 * @brief Get a image path of the part.
 * @param[in] type The part type
 * @remarks The returned data should be released.
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
	case PARTS_TYPE_HANDS_PIN:
		resource_image = IMAGE_HANDS_PIN;
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		return NULL;
	}

	data_get_resource_path(resource_image, image_path, sizeof(image_path));

	return strdup(image_path);
}

/**
 * @brief Get a position of the part.
 * @param[in] type The part type
 * @param[out] x The pointer to an integer in which to store the X coordinate of the part
 * @param[out] y The pointer to an integer in which to store the Y coordinate of the part
 */
void data_get_parts_position(parts_type_e type, int *x, int *y)
{
	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
		*x = (BASE_WIDTH / 2) - (HANDS_SEC_WIDTH / 2);
		*y = 36;
		break;
	case PARTS_TYPE_HANDS_MIN:
		*x = (BASE_WIDTH / 2) - (HANDS_MIN_WIDTH / 2);
		*y = (BASE_HEIGHT /2) - HANDS_MIN_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_HOUR:
		*x = (BASE_WIDTH / 2) - (HANDS_HOUR_WIDTH / 2);
		*y = (BASE_HEIGHT / 2) - HANDS_HOUR_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_PIN:
		*x = (BASE_WIDTH / 2) - (HANDS_PIN_WIDTH / 2);
		*y = (BASE_HEIGHT / 2) - (HANDS_PIN_HEIGHT / 2);
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		break;
	}
}

/**
 * @brief Get a width size of the part.
 * @param[in] type The part type
 * @param[out] w The pointer to an integer in which to store the width of the part
 * @param[out] h The pointer to an integer in which to store the height of the part
 */
void data_get_parts_size(parts_type_e type, int *w, int *h)
{
	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
		*w = HANDS_SEC_WIDTH;
		*h = HANDS_SEC_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_MIN:
		*w = HANDS_MIN_WIDTH;
		*h = HANDS_MIN_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_HOUR:
		*w = HANDS_HOUR_WIDTH;
		*h = HANDS_HOUR_HEIGHT;
		break;
	case PARTS_TYPE_HANDS_PIN:
		*w = HANDS_PIN_WIDTH;
		*h = HANDS_PIN_HEIGHT;
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		break;
	}
}

/**
 * @brief Gets the month name
 * @param[in] month The number of month
 * @remarks The returned data should be released.
 */
char *data_get_month_name(int month)
{
	char *month_name[12] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };

	return strdup(month_name[month]);
}

/**
 * @brief Gets the battery charge percentage.
 */
int data_get_battery_percent(void)
{
	int ret = 0;
	int percentage = 0;

	ret = device_battery_get_percent(&percentage);
	if (ret != DEVICE_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get battery information(%d)", ret);
		return 0;
	}

	return percentage;
}

/**
 * @brief Gets the battery charge color.
 * @remarks The returned data should be released.
 */
char *date_get_battery_color(int percentage)
{
	char *color[5] = { "blue", "green", "yellow", "orange", "red" };
	int idx = 0;

	if (percentage > 100 || percentage < 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery percentage is wrong");
		return strdup("red");
	}

	idx = (percentage == 0) ? 4 : ((100 - percentage) / 20);

	return strdup(color[idx]);
}

/**
 * @brief Gets the battery charge color.
 * @remarks The returned data should be released.
 */
char *date_get_air_pollution_color(int aqi)
{
	if (aqi < 30) {
		return strdup("blue");
	} else if (aqi < 60) {
		return strdup("green");
	} else if (aqi < 100) {
		return strdup("yellow");
	} else if (aqi < 150) {
		return strdup("orange");
	} else {
		return strdup("red");
	}
}
