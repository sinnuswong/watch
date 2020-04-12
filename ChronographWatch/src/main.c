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

#include <tizen.h>
#include <app.h>
#include <watch_app.h>
#include <watch_app_efl.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include "chronographwatch.h"
#include "view.h"
#include "data.h"

#define IMAGE_BG "images/chrono_clock_bg.png"

static void _create_parts(parts_type_e type);
static void _create_base_gui(int width, int height);

/*
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

/*
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

/*
 * @brief The low battery event callback function.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_battery(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low battery
	 */
	watch_app_exit();
}

/*
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

/*
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

/*
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
	_create_base_gui(width, height);

	/*
	 * Add chronograph watch animation
	 */
	view_chronograph_animator_add();

	return true;
}

/*
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

/*
 * @brief Called when the application is completely obscured by another application and becomes invisible.
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_pause(void *user_data)
{
	/*
	 * Take necessary actions when application becomes invisible.
	 */
	view_chronograph_animator_freeze();
}

/*
 * @brief Called when the application becomes visible.
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_resume(void *user_data)
{
	/*
	 * Take necessary actions when application becomes visible.
	 */
	view_chronograph_animator_thaw();
}

/*
 * @brief Called when the application's main loop exits.
 * @param[in] user_data The user data passed from the callback registration function
 */
static void app_terminate(void *user_data)
{
	view_destroy_base_gui();
}

/*
 * @brief Called at each second. This callback is not called while the app is paused or the device is in ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_time_tick(watch_time_h watch_time, void* user_data)
{
	view_chronograph_set_date(watch_time);
}

/*
 * @brief Called at each minute when the device in the ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_ambient_tick(watch_time_h watch_time, void* user_data)
{
	/* Make a ambient mode operation here */
}

/*
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

/*
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

/*
 * @brief Create base GUI for the watch.
 * @param[in] width width size of the watch
 * @param[in] height height size of the watch
 */
void _create_base_gui(int width, int height)
{
	Evas_Object *win = NULL;
	Evas_Object *bg = NULL;
	Evas_Object *chronograph_layout = NULL;
	char bg_path[PATH_MAX] = { 0, };
	char edj_path[PATH_MAX] = { 0, };
	int i;

	/*
	 * Creation sequence decide object's position (upper or lower)
	 */
	parts_type_e target_parts[PARTS_TYPE_NUM] = {
		PARTS_TYPE_HANDS_STOPWATCH_SEC,
		PARTS_TYPE_HANDS_STOPWATCH_30S,
		PARTS_TYPE_HANDS_STOPWATCH_12H,
		PARTS_TYPE_HANDS_SEC_SHADOW,
		PARTS_TYPE_HANDS_SEC,
		PARTS_TYPE_HANDS_MIN_SHADOW,
		PARTS_TYPE_HANDS_MIN,
		PARTS_TYPE_HANDS_HOUR_SHADOW,
		PARTS_TYPE_HANDS_HOUR,
	};

	/*
	 * Create watch window object
	 */
	win = view_create_watch_window(width, height);
	if (win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create a watch window");
		return;
	}

	/*
	 * Create BG(background) object
	 */
	data_get_resource_path(IMAGE_BG, bg_path, sizeof(bg_path));
	bg = view_create_bg(win, bg_path, width, height);
	if (bg == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create a bg");
		return;
	}

	/*
	 * Create layout to display the chronograph view at the watch
	 */
	data_get_resource_path(EDJ_FILE, edj_path, sizeof(edj_path));
	chronograph_layout = view_chronograph_create_layout(bg, edj_path);
	if (chronograph_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create chronograph_layout");
		return;
	}

	/*
	 * Create hands & shadow hands to display at the watch
	 */
	for (i = 0 ;  i < PARTS_TYPE_NUM ; i++)
		_create_parts(target_parts[i]);
}

/*
 * @brief Create parts of watch
 * @param[in] type Parts type
 */
void _create_parts(parts_type_e type)
{
	char *parts_image_path = NULL;
	int position_x = 0, position_y = 0;
	int size_w = 0, size_h = 0;

	/*
	 * Get the information about the part
	 */
	parts_image_path = data_get_parts_image_path(type);
	data_get_parts_position(type, &position_x, &position_y);
	size_w = data_get_parts_width_size(type);
	size_h = data_get_parts_height_size(type);

	/*
	 * Create the part object
	 */
	view_chronograph_create_parts(parts_image_path, position_x, position_y, size_w, size_h, type);

	free(parts_image_path);
}
