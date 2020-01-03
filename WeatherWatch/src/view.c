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

#include <Elementary.h>
#include <efl_extension.h>
#include <app.h>
#include <dlog.h>

#include "weatherwatch.h"
#include "view.h"

static struct view_info {
	Evas_Object *watchface_layout;
	Evas_Object *battery_module_layout;
	Evas_Object *ap_module_layout;
} s_info = {
	.watchface_layout = NULL,
	.battery_module_layout = NULL,
	.ap_module_layout = NULL,
};

/**
 * @brief Set the watch face layout.
 */
void view_set_watchface_layout(Evas_Object *layout)
{
	if (layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "layout is NULL");
		return;
	}

	s_info.watchface_layout = layout;
}

/**
 * @brief Set indicator module layout.
 */
void view_set_indicator_module_layout(Evas_Object *layout, int type)
{
	if (layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "layout is NULL");
		return;
	}

	if (type == INDICATOR_MODULE_BATTERY) {
	s_info.battery_module_layout = layout;
	} else {
		s_info.ap_module_layout = layout;
	}
}

/**
 * @brief Get the watch face layout.
 */
Evas_Object *view_get_watchface_layout(void)
{
	return s_info.watchface_layout;
}

/**
 * @brief Get the ap indicator module layout.
 */
Evas_Object *view_get_ap_module_layout(void)
{
	return s_info.ap_module_layout;
}

/**
 * @brief Get the battery module layout.
 */
Evas_Object *view_get_battery_module_layout(void)
{
	return s_info.battery_module_layout;
}

/**
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

/**
 * @brief Rotate hands of the watch.
 * @param[in] hand The hand you want to rotate
 * @param[in] degree The degree you want to rotate
 * @param[in] cx The rotation's center horizontal position
 * @param[in] cy The rotation's center vertical position
 */
void view_rotate_hand(Evas_Object *hand, double degree, Evas_Coord cx, Evas_Coord cy)
{
	Evas_Map *m = NULL;

	if (hand == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "hand is NULL");
		return;
	}

	/*
	 * You can use following API order set to rotate the object.
	 */

	/*
	 * 1. Create a set of points.
	 */
	m = evas_map_new(4);

	/*
	 * 2. Initialize map of an object to match its original position and size.
	 */
	evas_map_util_points_populate_from_object(m, hand);

	/*
	 * 3. Rotate the indicated map's coordinates around the center coordinates given by cx and cy as the rotation center.
	 */
	evas_map_util_rotate(m, degree, cx, cy);

	/*
	 * 4. Set the map on a given object.
	 */
	evas_object_map_set(hand, m);

	/*
	 * 5. Free a previously allocated map.
	 */
	evas_map_free(m);
}

/**
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

	/*
	 * Create layout by EDC(edje file)
	 */
	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, file_path, group_name);

	/*
	 * Layout size setting
	 */
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	return layout;
}

/**
 * @brief Sends signal with source to the EDJE object.
 * @param[in] layout The layout object will receive the signal
 * @param[in] signal The appointed signal to trigger the function
 * @param[in] source The appointed source that normally indicate the object triggered the event
 */
void view_send_signal_to_edje(Evas_Object *layout, const char *signal, const char *source)
{
	elm_object_signal_emit(layout, signal, source);
}

/**
 * @brief Sets the function will be called when the appointed signal is occurred.
 * @param[in] item The object triggered the signal
 * @param[in] signal The appointed signal to trigger the function
 * @param[in] source The appointed source that normally indicate the object triggered the event
 * @param[in] signal_cb The function will be called when the signal is detected
 * @param[in] user_data The data passed to the 'signal_cb' function
 */
void view_set_customized_event_callback(Evas_Object *item, char *signal, char *source, Edje_Signal_Cb signal_cb, void *user_data)
{
	elm_object_signal_callback_add(item, signal, source, signal_cb, user_data);
}

/**
 * @brief Create a part for the watch.
 * @param[in] parent The object to which you want to add this object
 * @param[in] image_path The path of the image file you want to set
 * @param[in] x The X coordinate of the part
 * @param[in] y The Y coordinate of the part
 * @param[in] w The width size of the part
 * @param[in] h The height size of the part
 * @param[in] enabled The flag whether the map is enabled
 */
Evas_Object *view_create_parts(Evas_Object *parent, const char *image_path, int x, int y, int w, int h, Eina_Bool enabled)
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

	evas_object_move(parts, x, y);
	evas_object_resize(parts, w, h);
	evas_object_show(parts);

	/*
	 * It makes events on this object to also be repeated for the next lower objects' stack.
	 */
	evas_object_repeat_events_set(parts, EINA_TRUE);

	/*
	 * This API enables the use of map for the object.
	 */
	evas_object_map_enable_set(parts, enabled);

	return parts;
}

/**
 * @brief Destroy base GUI.
 */
void view_destroy_base_gui(void)
{
	if (s_info.watchface_layout) {
		evas_object_del(s_info.watchface_layout);
		s_info.watchface_layout = NULL;
	}

	if (s_info.battery_module_layout) {
		evas_object_del(s_info.battery_module_layout);
		s_info.battery_module_layout = NULL;
	}

	if (s_info.ap_module_layout) {
		evas_object_del(s_info.ap_module_layout);
		s_info.ap_module_layout = NULL;
	}
}
