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

#include <Elementary.h>
#include <efl_extension.h>
#include <watch_app.h>
#include <watch_app_efl.h>
#include <app.h>
#include <dlog.h>

#include "chronographwatch.h"
#include "view.h"
#include "data.h"

static struct view_info {
	Evas_Object *bg;
	Evas_Object *chronograph_layout;
	Evas_Object *hands_sec;
	Evas_Object *hands_min;
	Evas_Object *hands_hour;
	Evas_Object *hands_sec_shadow;
	Evas_Object *hands_min_shadow;
	Evas_Object *hands_hour_shadow;
	Evas_Object *hands_stopwatch_sec;
	Evas_Object *hands_stopwatch_30s;
	Evas_Object *hands_stopwatch_12h;
	Ecore_Animator *animator;
} s_info = {
	.bg = NULL,
	.chronograph_layout = NULL,
	.hands_sec = NULL,
	.hands_min = NULL,
	.hands_hour = NULL,
	.hands_sec_shadow = NULL,
	.hands_min_shadow = NULL,
	.hands_hour_shadow = NULL,
	.hands_stopwatch_sec = NULL,
	.hands_stopwatch_30s = NULL,
	.hands_stopwatch_12h = NULL,
	.animator = NULL,
};

/*
 * @brief Set text to the part.
 * @param[in] parent Object has part to which you want to set text
 * @param[in] part_name Part name to which you want to set the text
 * @param[in] text Text you want to set to the part
 */
void view_set_text(Evas_Object *parent, const char *part_name, const char *text)
{
	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	/* Set text of target part object */
	elm_object_part_text_set(parent, part_name, text);
}

/*
 * @brief Rotate hands of the watch.
 * @param[in] hand The hand you want to rotate
 * @param[in] degree The degree you want to rotate
 * @param[in] cx The rotation's center horizontal position
 * @param[in] cy The rotation's center vertical position
 */
void view_rotate_hand(Evas_Object *hand, double degree, Evas_Coord cx, Evas_Coord cy)
{
	Evas_Map *m = NULL;

	m = evas_map_new(4);
	evas_map_util_points_populate_from_object(m, hand);
	evas_map_util_rotate(m, degree, cx, cy);
	evas_object_map_set(hand, m);
	evas_object_map_enable_set(hand, EINA_TRUE);
	evas_map_free(m);
}

/*
 * @brief Create the watch window.
 * @param[in] width Window's width you want to create
 * @param[in] height Window's height you want to create
 */
Evas_Object *view_create_watch_window(int width, int height)
{
	Evas_Object *win = NULL;
	int ret = EINA_FALSE;

	/*
	 * Get watch window object
	 */
	ret = watch_app_get_elm_win(&win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get watch window. err = %d", ret);
		return NULL;
	}
	evas_object_resize(win, width, height);
	evas_object_show(win);

	return win;
}

/*
 * @breif Create a bg(background) object for the watch.
 * @param[in] win Target window object
 * @param[in] image_path The image path for bg object
 * @param[in] width The width size of bg object
 * @param[in] height The height size of bg object
 */
Evas_Object *view_create_bg(Evas_Object *win, const char *image_path, int width, int height)
{
	Evas_Object *bg = NULL;
	Eina_Bool ret = EINA_FALSE;

	if (win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "window is NULL");
		return NULL;
	}

	bg = elm_bg_add(win);
	if (bg == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to add bg");
		return NULL;
	}

	ret = elm_bg_file_set(bg, image_path, NULL);
	if (ret != EINA_TRUE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set the background image");
		evas_object_del(bg);
		return NULL;
	}

	elm_bg_option_set(bg, ELM_BG_OPTION_CENTER);

	evas_object_move(bg, 0, 0);
	evas_object_resize(bg, width, height);
	evas_object_show(bg);

	s_info.bg = bg;

	return bg;
}

/*
 * @brief Make a layout to target parent object with edje file.
 * @param[in] parent The object to which you want to add this layout
 * @param[in] file_path File path of EDJ file will be used
 * @param[in] group_name Name of group in EDJ you want to set to
 * @param[in] user_data The user data to be passed to the callback functions
 */
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, void *user_data)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	/* Create layout by EDC(edje file) */
	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, file_path, group_name);

	/* Layout size setting */
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	return layout;
}

/*
 * @brief Create main layout for the watch.
 * @param[in] parent The object to which you want to add this layout
 * @param[in] file_path File path of EDJ file will be used
 */
Evas_Object *view_chronograph_create_layout(Evas_Object *parent, const char *file_path)
{
	Evas_Object *chronograph_layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "bg is NULL");
		return NULL;
	}

	chronograph_layout = view_create_layout(parent, file_path, "chronograph", NULL);
	if (chronograph_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create stop_watch_layout");
		return NULL;
	}

	evas_object_resize(chronograph_layout, BASE_WIDTH, BASE_HEIGHT);
	evas_object_show(chronograph_layout);

	s_info.chronograph_layout = chronograph_layout;

	return chronograph_layout;
}

/*
 * @brief Create a part for the chronograph watch.
 * @param[in] image_path The path of the image file you want to set
 * @param[in] position_x The X coordinate of the part
 * @param[in] position_y The Y coordinate of the part
 * @param[in] size_w The width size of the part
 * @param[in] size_h The height size of the part
 * @param[in] type The type of the part
 */
void view_chronograph_create_parts(const char *image_path, int position_x, int position_y, int size_w, int size_h, parts_type_e type)
{
	Evas_Object *part = NULL;

    /* Create parts */
	part = view_create_parts(s_info.bg, image_path, position_x, position_y, size_w, size_h);

	/*
	 * Set opacity to shadow hands
	 */
	if (type == PARTS_TYPE_HANDS_HOUR_SHADOW ||
			type == PARTS_TYPE_HANDS_MIN_SHADOW ||
			type == PARTS_TYPE_HANDS_SEC_SHADOW) {
		evas_object_color_set(part, 255, 255, 255, 255 * 0.3);
	}

	switch (type) {
	case PARTS_TYPE_HANDS_SEC:
		s_info.hands_sec = part;
		break;
	case PARTS_TYPE_HANDS_MIN:
		s_info.hands_min = part;
		break;
	case PARTS_TYPE_HANDS_HOUR:
		s_info.hands_hour = part;
		break;
	case PARTS_TYPE_HANDS_SEC_SHADOW:
		s_info.hands_sec_shadow = part;
		break;
	case PARTS_TYPE_HANDS_MIN_SHADOW:
		s_info.hands_min_shadow = part;
		break;
	case PARTS_TYPE_HANDS_HOUR_SHADOW:
		s_info.hands_hour_shadow = part;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_SEC:
		s_info.hands_stopwatch_sec = part;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_30S:
		s_info.hands_stopwatch_30s = part;
		break;
	case PARTS_TYPE_HANDS_STOPWATCH_12H:
		s_info.hands_stopwatch_12h = part;
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "type error : %d", type);
		return;
	}
}

/*
 * @brief Create a part for the watch.
 * @param[in] parent The object to which you want to add this object
 * @param[in] image_path The path of the image file you want to set
 * @param[in] position_x The X coordinate of the part
 * @param[in] position_y The Y coordinate of the part
 * @param[in] size_w The width size of the part
 * @param[in] size_h The height size of the part
 */
Evas_Object *view_create_parts(Evas_Object *parent, const char *image_path, int position_x, int position_y, int size_w, int size_h)
{
	Evas_Object *parts = NULL;
	Eina_Bool ret = EINA_FALSE;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "bg is NULL");
		return NULL;
	}

	parts = elm_image_add(parent);
	if (parts == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to add minute hand image");
		return NULL;
	}

	ret = elm_image_file_set(parts, image_path, NULL);
	if (ret != EINA_TRUE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set minute hand image");
		evas_object_del(parts);
		return NULL;
	}

	evas_object_move(parts, position_x, position_y);
	evas_object_resize(parts, size_w, size_h);
	evas_object_show(parts);

	return parts;
}

/*
 * @brief Update time at the chronograph watch.
 * @param[in] user_data The user data to be passed to the callback functions
 */
Eina_Bool view_chronograph_update_time(void *data)
{
	int msec = 0;
	int sec = 0;
	int min = 0;
	int hour = 0;
	double degree = 0.0f;
	int ret = 0;
	watch_time_h watch_time;

	/*
	 * Get current time
	 */
	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get current time. err = %d", ret);
		return EINA_TRUE;
	}

	watch_time_get_hour(watch_time, &hour);
	watch_time_get_minute(watch_time, &min);
	watch_time_get_second(watch_time, &sec);
	watch_time_get_millisecond(watch_time, &msec);

    /* Time data should be freed. */
	watch_time_delete(watch_time);

	/*
	 * Rotate hands at the watch
	 */
	degree = sec * SEC_ANGLE;
	degree += msec * SEC_ANGLE / 1000.0;

	view_rotate_hand(s_info.hands_sec, degree, HANDS_SEC_PIVOT_POSITION_X, HANDS_SEC_PIVOT_POSITION_Y);
	view_rotate_hand(s_info.hands_sec_shadow, degree,  HANDS_SEC_PIVOT_POSITION_X, 183/*HANDS_SEC_PIVOT_POSITION_Y + HANDS_SEC_SHADOW_PADDING*/);

	degree = min * MIN_ANGLE;
	degree += sec * MIN_ANGLE / 60.0;

	view_rotate_hand(s_info.hands_min, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2));
	view_rotate_hand(s_info.hands_min_shadow, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2) + HANDS_MIN_SHADOW_PADDING);

	degree = hour * HOUR_ANGLE;
	degree += min * HOUR_ANGLE / 60.0;

	view_rotate_hand(s_info.hands_hour, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2));
	view_rotate_hand(s_info.hands_hour_shadow, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2) + HANDS_HOUR_SHADOW_PADDING);
	return EINA_TRUE;
}

/*
 * @brief Set date at the watch.
 * @pram[in] watch_time watch_time handle
 */
void view_chronograph_set_date(watch_time_h watch_time)
{
	char txt_day_num[32] = { 0, };
	int day_num = 0;

	/*
	 * In this case, time data will be freed by the framework.
	 */
	watch_time_get_day(watch_time, &day_num);

	/*
	 * Set day number at the watch
	 */
	snprintf(txt_day_num, sizeof(txt_day_num), "%d", day_num);
	view_set_text(s_info.chronograph_layout, "txt.day.num", txt_day_num);
}

/*
 * @brief Resume chronograph watch animation.
 */
void view_chronograph_animator_thaw(void)
{
	if (s_info.animator)
		ecore_animator_thaw(s_info.animator);
}

/*
 * @brief Pause chronograph watch animation.
 */
void view_chronograph_animator_freeze(void)
{
	if (s_info.animator)
		ecore_animator_freeze(s_info.animator);
}

/*
 * @brief Start chronograph watch animation.
 */
void view_chronograph_animator_add(void)
{
	s_info.animator = ecore_animator_add(view_chronograph_update_time, NULL);
}

/*
 * @brief Destroy base GUI.
 */
void view_destroy_base_gui(void)
{
	if (s_info.chronograph_layout) {
		evas_object_del(s_info.chronograph_layout);
		s_info.chronograph_layout = NULL;
	}

	if (s_info.bg) {
		evas_object_del(s_info.bg);
		s_info.bg = NULL;
	}

	if (s_info.hands_sec) {
		evas_object_del(s_info.hands_sec);
		s_info.hands_sec = NULL;
	}

	if (s_info.hands_sec_shadow) {
		evas_object_del(s_info.hands_sec_shadow);
		s_info.hands_sec_shadow = NULL;
	}

	if (s_info.hands_min) {
		evas_object_del(s_info.hands_min);
		s_info.hands_min = NULL;
	}

	if (s_info.hands_min_shadow) {
		evas_object_del(s_info.hands_min_shadow);
		s_info.hands_min_shadow = NULL;
	}

	if (s_info.hands_hour) {
		evas_object_del(s_info.hands_hour);
		s_info.hands_hour = NULL;
	}

	if (s_info.hands_hour_shadow) {
		evas_object_del(s_info.hands_hour_shadow);
		s_info.hands_hour_shadow = NULL;
	}

	if (s_info.hands_stopwatch_sec) {
		evas_object_del(s_info.hands_stopwatch_sec);
		s_info.hands_stopwatch_sec = NULL;
	}

	if (s_info.hands_stopwatch_30s) {
		evas_object_del(s_info.hands_stopwatch_30s);
		s_info.hands_stopwatch_30s = NULL;
	}

	if (s_info.hands_stopwatch_12h) {
		evas_object_del(s_info.hands_stopwatch_12h);
		s_info.hands_stopwatch_12h = NULL;
	}

	if (s_info.animator) {
		ecore_animator_del(s_info.animator);
		s_info.animator = NULL;
	}
}
