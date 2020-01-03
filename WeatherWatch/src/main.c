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

#include <tizen.h>
#include <app.h>
#include <watch_app.h>
#include <watch_app_efl.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <device/callback.h>

#include "weatherwatch.h"
#include "view.h"
#include "data.h"

static struct main_info {
	int dummy;
	int cur_min;
	int cur_day;
	int cur_month;
} s_info = {
	.dummy = 0,
	.cur_min = 0,
	.cur_day = 0,
	.cur_month = 0,
};

static void _set_time(int hour, int min, int sec);
static void _set_calendar(int month, int day);
static int _create_base_gui(int width, int height);
static int _create_indicator_module_layout(int type);
static Evas_Object *_create_parts(parts_type_e type);
static void _set_indicator_information(Evas_Object *parent, int value, const char *color, int type);
static void _battery_changed_cb(device_callback_e type, void *value, void *user_data);
static void _battery_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _ap_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _set_air_pollution_value(int aqi);
static void _dummy_operation(void);
static void _update_weather_state(weather_state_e state);

/**
 * @brief The system language changed event callback function.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void lang_changed(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when language setting is changed
	 */
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	if (locale == NULL)
		return;

	elm_language_set(locale);
	free(locale);

	return;
}

/**
 * @brief The region format changed event callback function.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void region_changed(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when region setting is changed
	 */
}

/**
 * @brief The low battery event callback function.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_battery(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low battery
	 */
}

/**
 * @brief The low memory event callback function.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_memory(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low memory
	 */
	watch_app_exit();
}

/**
 * @brief The device orientation changed event callback function.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void device_orientation(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when device orientation is changed
	 */
}

/**
 * @brief Called when the application starts.
 * @param[in] width The width of the window of idle screen that will show the watch UI
 * @param[in] height The height of the window of idle screen that will show the watch UI
 * @param[in] user_data The user data passed from the callback registration function
 */
static bool app_create(int width, int height, void* user_data)
{
	/*
	 * Hook to take necessary actions before main event loop starts
	 * Initialize UI resources and application's data
	 */

	app_event_handler_h handlers[5] = { NULL, };

	/*
	 * Register callbacks for each system event
	 */
	if (watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, lang_changed, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, region_changed, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, low_battery, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, low_memory, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, device_orientation, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);

	/*
	 * Create base GUI
	 */
	if (_create_base_gui(width, height) < 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create base GUI");
		return false;
	}

	/*
	 * Create battery module
	 */
	if (_create_indicator_module_layout(INDICATOR_MODULE_BATTERY) < 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create battery module layout");
		view_destroy_base_gui();
		return false;
	}

	/*
	 * Create air pollution module
	 */
	if (_create_indicator_module_layout(INDICATOR_MODULE_AIR_POLLUTION) < 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create air pollution module layout");
		view_destroy_base_gui();
		return false;
	}

	return true;
}

/**
 * @brief Called when another application sends a launch request to the application.
 * @param[in] width The width of the window of idle screen that will show the watch UI
 * @param[in] height The height of the window of idle screen that will show the watch UI
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_control(app_control_h app_control, void *user_data)
{
	/*
	 * Handle the launch request.
	 */
}

/**
 * @brief Called when the application is completely obscured by another application and becomes invisible.
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_pause(void *user_data)
{
	/*
	 * Take necessary actions when application becomes invisible.
	 */
	int ret = 0;

	dlog_print(DLOG_INFO, LOG_TAG, "%s", __func__);

	/*
	 * Unregister callback function for battery charge percentage
	 */
	ret = device_remove_callback(DEVICE_CALLBACK_BATTERY_CAPACITY, _battery_changed_cb);
	if (ret != DEVICE_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to unregister battery event callback function(%d)", ret);
	}
}

/**
 * @brief Called when the application becomes visible.
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_resume(void *user_data)
{
	/*
	 * Take necessary actions when application becomes visible.
	 */
	Evas_Object *battery_module_layout = NULL;
	char *battery_color = NULL;
	int battery_percentage = 0;
	int ret = 0;

	dlog_print(DLOG_INFO, LOG_TAG, "%s", __func__);

	battery_module_layout = view_get_battery_module_layout();
	if (battery_module_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get battery module layout");
		return;
	}

	/*
	 * Set current battery charge percentage
	 */
	battery_percentage = data_get_battery_percent();

	/*
	 * Get battery charge percentage color
	 */
	battery_color = date_get_battery_color(battery_percentage);

	_set_indicator_information(battery_module_layout, battery_percentage, battery_color, INDICATOR_MODULE_BATTERY);

	free(battery_color);

	/*
	 * Register callback function for battery charge percentage
	 */
	ret = device_add_callback(DEVICE_CALLBACK_BATTERY_CAPACITY, _battery_changed_cb, battery_module_layout);
	if (ret != DEVICE_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to register battery event callback function(%d)", ret);
	}
}

/**
 * @brief Called when the application's main loop exits.
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_terminate(void *user_data)
{
	view_destroy_base_gui();
}

/**
 * @brief Called at each second. This callback is not called while the app is paused or the device is in ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_time_tick(watch_time_h watch_time, void* user_data)
{
	int hour = 0;
	int min = 0;
	int sec = 0;
	int month = 0;
	int day = 0;

	watch_time_get_hour(watch_time, &hour);
	watch_time_get_minute(watch_time, &min);
	watch_time_get_second(watch_time, &sec);
	watch_time_get_day(watch_time, &day);
	watch_time_get_month(watch_time, &month);

	_set_time(hour, min, sec);
	_set_calendar(month, day);
}

/**
 * @brief Called at each minute when the device in the ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_ambient_tick(watch_time_h watch_time, void* user_data)
{
}

/**
 * @brief Called when the device enters or exits the ambient mode.
 * @param[in] ambient_mode If @c true the device enters the ambient mode, otherwise @c false
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_ambient_changed(bool ambient_mode, void* user_data)
{
	if (ambient_mode) {
		/*
		 * Prepare to enter the ambient mode
		 */
	} else {
		/*
		 * Prepare to exit the ambient mode
		 */
	}
}

/**
 * @brief Main function of the application.
 */
int main(int argc, char *argv[])
{
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = { 0, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	ret = watch_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);

	return ret;
}

/**
 * @brief Set time at the watch.
 * @pram[in] hour The hour number
 * @pram[in] min The min number
 * @pram[in] sec The sec number
 */
static void _set_time(int hour, int min, int sec)
{
	Evas_Object *watchface_layout = NULL;
	Evas_Object *hands = NULL;
	double degree = 0.0f;

	watchface_layout = view_get_watchface_layout();
	if (watchface_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get watch face layout");
		return;
	}

	/*
	 * Rotate hands at the watch
	 */
	degree = sec * SEC_ANGLE;
	hands = evas_object_data_get(watchface_layout, DATA_KEY_HANDS_SEC);
	view_rotate_hand(hands, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2));

	if (s_info.cur_min != min) {
		degree = min * MIN_ANGLE;
		hands = evas_object_data_get(watchface_layout, DATA_KEY_HANDS_MIN);
		view_rotate_hand(hands, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2));
		s_info.cur_min = min;

		degree = (hour * HOUR_ANGLE) + data_get_plus_angle(min);
		hands = evas_object_data_get(watchface_layout, DATA_KEY_HANDS_HOUR);
		view_rotate_hand(hands, degree, (BASE_WIDTH / 2), (BASE_HEIGHT / 2));
	}
}

/**
 * @brief Set calendar at the watch.
 * @pram[in] month The month number
 * @pram[in] day The day number
 */
static void _set_calendar(int month, int day)
{
	Evas_Object *watchface_layout = NULL;

	watchface_layout = view_get_watchface_layout();
	if (watchface_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get watch face layout");
		return;
	}

	if (s_info.cur_month != month || s_info.cur_day != day) {
		char *month_name = NULL;
		char txt_calendar[128] = { 0, };

		month_name = data_get_month_name(month - 1);
		if (month_name == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get the month name");
			month_name = strdup("JANUARY");
		}

		snprintf(txt_calendar, sizeof(txt_calendar), "%s <day>%d</day>", month_name, day);
		view_set_text(watchface_layout, "txt.calendar", txt_calendar);

		free(month_name);
	}
}

/**
 * @brief Create indicator module layout.
 * @pram[in] type Indicator type
 */
static int _create_indicator_module_layout(int type)
{
	Evas_Object *watchface_layout = NULL;
	Evas_Object *indicator_module_layout = NULL;
	char edj_path[PATH_MAX] = { 0, };

	/*
	 * Get the watch face layout
	 */
	watchface_layout = view_get_watchface_layout();
	if (watchface_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch face layout is NULL");
		return -1;
	}

	/*
	 * Get edje file path
	 */
	data_get_resource_path(EDJ_FILE, edj_path, sizeof(edj_path));

	/*
	 * Create indicator module layout
	 */
	if (type == INDICATOR_MODULE_BATTERY) {
		indicator_module_layout = view_create_layout(watchface_layout, edj_path, "layout_battery_module", NULL);
	} else {
		indicator_module_layout = view_create_layout(watchface_layout, edj_path, "layout_air_pollution_module", NULL);
	}

	if (indicator_module_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create indicator module");
		return -1;
	}

	view_set_indicator_module_layout(indicator_module_layout, type);

	if (type == INDICATOR_MODULE_BATTERY) {
		elm_object_part_content_set(watchface_layout, "sw.battery.module", indicator_module_layout);
		evas_object_data_set(indicator_module_layout, DATA_KEY_BATTYER_INFO_TYPE, (void *) INDICATOR_INFO_TYPE_ICON);
		view_send_signal_to_edje(indicator_module_layout, "hide,battery,txt", "txt.battery");

	/*
	 * Register callback function for battery toggle event
	 */
		view_set_customized_event_callback(indicator_module_layout, "mouse,clicked", "rect.battery", _battery_clicked_cb, NULL);

	} else {
		elm_object_part_content_set(watchface_layout, "sw.air.pollution.module", indicator_module_layout);
		evas_object_data_set(indicator_module_layout, DATA_KEY_AP_INFO_TYPE, (void *) INDICATOR_INFO_TYPE_ICON);
		view_send_signal_to_edje(indicator_module_layout, "hide,air_pollution,txt", "txt.air_pollution");

		/*
		 * Register callback function for air pollution toggle event
		 */
		view_set_customized_event_callback(indicator_module_layout, "mouse,clicked", "rect.air_pollution", _ap_clicked_cb, NULL);
	}

	return 0;
}

/**
 * @brief Create base GUI for the watch.
 * @param[in] width The width size of the watch
 * @param[in] height The height size of the watch
 */
static int _create_base_gui(int width, int height)
{
	Evas_Object *win = NULL;
	Evas_Object *watchface_layout = NULL;
	Evas_Object *hands_sec = NULL;
	Evas_Object *hands_min = NULL;
	Evas_Object *hands_hour = NULL;
	Evas_Object *hands_pin = NULL;
	char edj_path[PATH_MAX] = { 0, };
	int ret = 0;

	/*
	 * Get window object
	 */
	ret = watch_app_get_elm_win(&win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get window. err = %d", ret);
		return -1;
	}
	evas_object_resize(win, width, height);
	evas_object_show(win);

	/*
	 * Get edje file path
	 */
	data_get_resource_path(EDJ_FILE, edj_path, sizeof(edj_path));

	/*
	 * Create watch face layout
	 */
	watchface_layout = view_create_layout(win, edj_path, "layout_watchface", NULL);
	if (watchface_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create watch face layout");
		evas_object_del(win);
		return -1;
	}
	view_set_watchface_layout(watchface_layout);

	/*
	 * Create hands to display at the watch
	 */
	hands_sec = _create_parts(PARTS_TYPE_HANDS_SEC);
	evas_object_data_set(watchface_layout, DATA_KEY_HANDS_SEC, hands_sec);

	hands_min = _create_parts(PARTS_TYPE_HANDS_MIN);
	evas_object_data_set(watchface_layout, DATA_KEY_HANDS_MIN, hands_min);

	hands_hour = _create_parts(PARTS_TYPE_HANDS_HOUR);
	evas_object_data_set(watchface_layout, DATA_KEY_HANDS_HOUR, hands_hour);

	hands_pin = _create_parts(PARTS_TYPE_HANDS_PIN);

	return 0;
}

/**
 * @brief Create parts of watch.
 * @param[in] type The parts type
 */
static Evas_Object *_create_parts(parts_type_e type)
{
	Evas_Object *parts = NULL;
	Evas_Object *watchface_layout = NULL;
	char *parts_image_path = NULL;
	int x = 0, y = 0, w = 0, h = 0;

	/*
	 * Get the watch face layout
	 */
	watchface_layout = view_get_watchface_layout();
	if (watchface_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get watch face layout");
		return NULL;
	}

	/*
	 * Get the information about the part
	 */
	parts_image_path = data_get_parts_image_path(type);
	data_get_parts_position(type, &x, &y);
	data_get_parts_size(type, &w, &h);

	/*
	 * Create the part object
	 */
	if (type == PARTS_TYPE_HANDS_PIN) {
		parts = view_create_parts(watchface_layout, parts_image_path, x, y, w, h, EINA_FALSE);
	} else {
		parts = view_create_parts(watchface_layout, parts_image_path, x, y, w, h, EINA_TRUE);
	}

	if (parts == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create parts : %d", type);
	}

	free(parts_image_path);

	return parts;
}

/**
 * @brief Sets indicator's information. e.g. battery charge percentage, air pollution index and color.
 * @param[in] parent The object to which you want to set battery information
 * @param[in] value Indicator's value to be updated
 * @param[in] color Indicator's color to be updated
 * @param[in] type Indicator type
 */
static void _set_indicator_information(Evas_Object *parent, int value, const char *color, int type)
{
	char txt[8] = { 0, };
	char signal[128] = { 0, };

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL");
		return;
	}

	if (type == INDICATOR_MODULE_BATTERY) {
	/*
	 * Set battery charge percentage & color
	 */
		snprintf(signal, sizeof(signal), "change,battery,%s", color);
	view_send_signal_to_edje(parent, signal, "img.battery.icon");

	/*
	 * Set battery charge percentage text
	 */
		snprintf(txt, sizeof(txt), "%d%%", value);
		view_set_text(parent, "txt.battery", txt);
	} else {
		/*
		 * Set air pollution index value & color
		 */
		snprintf(signal, sizeof(signal), "change,air_pollution,%s", color);
		view_send_signal_to_edje(parent, signal, "img.air_pollution.icon");

		/*
		 * Set air pollution index value text
		 */
		snprintf(txt, sizeof(txt), "%dAQI", value);
		view_set_text(parent, "txt.air_pollution", txt);
	}
}

/**
 * @brief This function is called when a battery status is changed.
 * @param[out] type The device type to monitor
 * @param[out] value The changed value
 * @param[out] user_data The user data passed from the callback registration function
 */
static void _battery_changed_cb(device_callback_e type, void *value, void *user_data)
{
	Evas_Object *battery_module_layout = NULL;
	char *battery_color = NULL;

	battery_module_layout = (Evas_Object *) user_data;
	if (battery_module_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery module layout is NULL");
		return;
	}

	/*
	 * Get battery charge percentage color
	 */
	battery_color = date_get_battery_color((int) value);

	_set_indicator_information(battery_module_layout, (int) value, battery_color, INDICATOR_MODULE_BATTERY);

	free(battery_color);
}

/**
 * @brief This function is called when battery module is clicked.
 * @param[in] data A pointer to the data to pass to the callback function
 * @param[in] obj The Edje object where the signal comes from
 * @param[in] emission The signal name
 * @param[in] source The signal source
 */
static void _battery_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Evas_Object *battery_module_layout = NULL;
	indicator_info_type_e type = INDICATOR_INFO_TYPE_ICON;

	battery_module_layout = obj;
	if (battery_module_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get battery module layout");
		return;
	}

	/*
	 * Get current battery information type
	 */
	type = (indicator_info_type_e) evas_object_data_get(battery_module_layout, (void *) DATA_KEY_BATTYER_INFO_TYPE);

	dlog_print(DLOG_INFO, LOG_TAG, "Battery icon is clicked. current type : %d", type);

	if (type == INDICATOR_INFO_TYPE_ICON) {
		/*
		 * Show battery charge percentage text and set battery information type by text
		 */
		view_send_signal_to_edje(battery_module_layout, "show,battery,txt", "txt.battery");
		evas_object_data_set(battery_module_layout, DATA_KEY_BATTYER_INFO_TYPE, (void *) INDICATOR_INFO_TYPE_TEXT);
	} else if (type == INDICATOR_INFO_TYPE_TEXT) {
		/*
		 * Hide battery charge percentage text and set battery information type by icon
		 */
		view_send_signal_to_edje(battery_module_layout, "hide,battery,txt", "txt.battery");
		evas_object_data_set(battery_module_layout, DATA_KEY_BATTYER_INFO_TYPE, (void *) INDICATOR_INFO_TYPE_ICON);
	} else {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery info type error(%d)", type);
		return;
	}
}

/**
 * @brief This function is called when air pollution module is clicked.
 * @param[in] data A pointer to the data to pass to the callback function
 * @param[in] obj The Edje object where the signal comes from
 * @param[in] emission The signal name
 * @param[in] source The signal source
 */
static void _ap_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Evas_Object *ap_module_layout = NULL;
	indicator_info_type_e type = INDICATOR_INFO_TYPE_ICON;

	ap_module_layout = obj;
	if (ap_module_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get air pollution module layout");
		return;
	}

	/*
	 * Below dummy operation code is only for sample display
	 * When click the air pollution module, _dummy_operation() change air pollution and weather status.
	 */
	_dummy_operation();

	/*
	 * Get current air pollution information type
	 */
	type = (indicator_info_type_e) evas_object_data_get(ap_module_layout, (void *) DATA_KEY_AP_INFO_TYPE);

	dlog_print(DLOG_INFO, LOG_TAG, "Air Pollution icon is clicked. current type : %d", type);

	if (type == INDICATOR_INFO_TYPE_ICON) {
		/*
		 * Show air pollution index value text and set air pollution information type by text
		 */
		view_send_signal_to_edje(ap_module_layout, "show,air_pollution,txt", "txt.air_pollution");
		evas_object_data_set(ap_module_layout, DATA_KEY_AP_INFO_TYPE, (void *) INDICATOR_INFO_TYPE_TEXT);
	} else if (type == INDICATOR_INFO_TYPE_TEXT) {
		/*
		 * Hide air pollution index value text and set air pollution information type by icon
		 */
		view_send_signal_to_edje(ap_module_layout, "hide,air_pollution,txt", "txt.air_pollution");
		evas_object_data_set(ap_module_layout, DATA_KEY_AP_INFO_TYPE, (void *) INDICATOR_INFO_TYPE_ICON);
	} else {
		dlog_print(DLOG_ERROR, LOG_TAG, "air pollution info type error(%d)", type);
		return;
	}
}

/**
 * @brief Updates air pollution view according to the AQI(Air Quality Index).
 * @param[in] aqi AQI value to be used for updating air pollution module
 */
static void _set_air_pollution_value(int aqi)
{
	Evas_Object *ap_module_layout = NULL;
	char *color = NULL;

	ap_module_layout = view_get_ap_module_layout();
	if (ap_module_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "air pollution module layout is NULL");
		return;
	}

	/*
	 * Get air pollution status color
	 */
	color = date_get_air_pollution_color(aqi);

	_set_indicator_information(ap_module_layout, aqi, color, INDICATOR_MODULE_AIR_POLLUTION);

	free(color);
}

/**
 * @brief Changes air pollution and weather status.
 * @remarks This dummy function is only for sample app. When click the air pollution module, _dummy_operation() change air pollution and weather status.
 */
static void _dummy_operation(void)
{
	if (s_info.dummy < 0)
		s_info.dummy = 0;

	int ap_val = s_info.dummy % 5;
	int weather_val = s_info.dummy % (int)WEATHER_STATE_MAX;

	_set_air_pollution_value(ap_val * 40);
	_update_weather_state(weather_val);

	s_info.dummy++;

}

/**
 * @brief Updates weather state and view.
 * @param[in] state Weather state to be used
 */
static void _update_weather_state(weather_state_e state)
{
	char signal[128] = { 0, };
	char text[128] = { 0, };

	/*
	 * Check weather state text and signal according to input state.
	 */
	switch (state) {
	case WEATHER_STATE_CLEAR_SKY_DAY:
		snprintf(text, sizeof(text), "%s", "Clear Sky");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_CLEAR_SKY_DAY);
		break;
	case WEATHER_STATE_FEW_CLOUDS_DAY:
		snprintf(text, sizeof(text), "%s", "Cloudy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_FEW_CLOUDS_DAY);
		break;
	case WEATHER_STATE_SCATTERED_CLOUDS_DAY:
		snprintf(text, sizeof(text), "%s", "Cloudy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_SCATTERED_CLOUDS_DAY);
		break;
	case WEATHER_STATE_BROKEN_CLOUDS_DAY:
		snprintf(text, sizeof(text), "%s", "Cloudy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_BROKEN_CLOUDS_DAY);
		break;
	case WEATHER_STATE_SHOWER_RAIN_DAY:
		snprintf(text, sizeof(text), "%s", "Rainy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_SHOWER_RAIN_DAY);
		break;
	case WEATHER_STATE_RAIN_DAY:
		snprintf(text, sizeof(text), "%s", "Rainy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_RAIN_DAY);
		break;
	case WEATHER_STATE_THUNDERSTORM_DAY:
		snprintf(text, sizeof(text), "%s", "Thunderstorm");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_THUNDERSTORM_DAY);
		break;
	case WEATHER_STATE_SNOW_DAY:
		snprintf(text, sizeof(text), "%s", "Snowy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_SNOW_DAY);
		break;
	case WEATHER_STATE_MIST_DAY:
		snprintf(text, sizeof(text), "%s", "Misty");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_MIST_DAY);
		break;
	case WEATHER_STATE_CLEAR_SKY_NIGHT:
		snprintf(text, sizeof(text), "%s", "Clear Sky");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_CLEAR_SKY_NIGHT);
		break;
	case WEATHER_STATE_FEW_CLOUDS_NIGHT:
		snprintf(text, sizeof(text), "%s", "Cloudy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_FEW_CLOUDS_NIGHT);
		break;
	case WEATHER_STATE_SCATTERED_CLOUDS_NIGHT:
		snprintf(text, sizeof(text), "%s", "Cloudy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_SCATTERED_CLOUDS_NIGHT);
		break;
	case WEATHER_STATE_BROKEN_CLOUDS_NIGHT:
		snprintf(text, sizeof(text), "%s", "Cloudy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_BROKEN_CLOUDS_NIGHT);
		break;
	case WEATHER_STATE_SHOWER_RAIN_NIGHT:
		snprintf(text, sizeof(text), "%s", "Rainy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_SHOWER_RAIN_NIGHT);
		break;
	case WEATHER_STATE_RAIN_NIGHT:
		snprintf(text, sizeof(text), "%s", "Rainy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_RAIN_NIGHT);
		break;
	case WEATHER_STATE_THUNDERSTORM_NIGHT:
		snprintf(text, sizeof(text), "%s", "Thunderstorm");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_THUNDERSTORM_NIGHT);
		break;
	case WEATHER_STATE_SNOW_NIGHT:
		snprintf(text, sizeof(text), "%s", "Snowy");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_SNOW_NIGHT);
		break;
	case WEATHER_STATE_MIST_NIGHT:
		snprintf(text, sizeof(text), "%s", "Misty");
		snprintf(signal, sizeof(signal), "change,weather,%s", WEATHER_ICON_STR_MIST_NIGHT);
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "state error : %d", state);
		return;
	}

	/*
	 * Set weather state text.
	 */
	view_send_signal_to_edje(view_get_watchface_layout(), signal, "img.weather");

	/*
	 * Send the signal for weather icon program of edje.
	 */
	view_set_text(view_get_watchface_layout(), "txt.weather", text);
}
