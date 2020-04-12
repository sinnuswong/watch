#include "basicui3.h"
#include <app_control.h>

typedef struct appdata1 {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	char *path;
} appdata_s1;

static void image_selected_callback(app_control_h request, app_control_h reply,
		app_control_result_e result, void *user_data);
static void btn_file_select_cb(void *data, Evas_Object *obj, void *event_info);
static void win_delete_request_cb(void *data, Evas_Object *obj,
		void *event_info) {
	ui_app_exit();
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s1 *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void create_base_gui1(appdata_s1 *ad) {
	/* Window */
	/* Create and initialize elm_win.
	 elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win,
				(const int *) (&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request",
			win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb,
			ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	 elm_conformant is mandatory for base gui to have proper size
	 when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */
	/* Create an actual view of the base gui.
	 Modify this part to change the view. */
	ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>Hello Tizen</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);

	Evas_Object *bt = elm_button_add(ad->win);
	elm_object_text_set(bt, "Launch");
	evas_object_resize(bt, 130, 50);
	evas_object_move(bt, 150, 250);
	evas_object_show(bt);
	evas_object_smart_callback_add(bt, "clicked", btn_file_select_cb, ad);
	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

/* Callback function to get result */
static void image_selected_callback(app_control_h request, app_control_h reply,
		app_control_result_e result, void *user_data) {
	char **value;
	int len;
	int ret;

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		ret = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED,
				&value, &len);

		if (ret == APP_CONTROL_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "the path is %s ",*value);
		} else {
			dlog_print(DLOG_INFO, LOG_TAG, "Failed");
		}
	}
}
static void btn_file_select_cb(void *data, Evas_Object *obj, void *event_info) {
	app_control_h app_control;
	int ret;

	app_control_create(&app_control);
	app_control_set_operation(app_control, APP_CONTROL_OPERATION_PICK);
	app_control_set_mime(app_control, "image/*");
	app_control_add_extra_data(app_control, APP_CONTROL_DATA_SELECTION_MODE,
			"single");
	//app_control_set_launch_mode(app_control, APP_CONTROL_LAUNCH_MODE_GROUP);

	ret = app_control_send_launch_request(app_control, image_selected_callback,
	NULL);
	if (ret == APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_INFO, LOG_TAG,
				"Succeeded to launch a file manager picker app.");
	} else {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"Failed to launch a file manager picker app. error code: %d",
				ret);
	}

	app_control_destroy(app_control);
}
static bool app_create1(void *data) {
	/* Hook to take necessary actions before main event loop starts
	 Initialize UI resources and application's data
	 If this function returns true, the main loop of application starts
	 If this function returns false, the application is terminated */
	appdata_s1 *ad = data;

	create_base_gui1(ad);

	return true;
}

static void app_control1(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause1(void *data) {
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume1(void *data) {
	/* Take necessary actions when application becomes visible. */
}

static void app_terminate1(void *data) {
	/* Release all resources. */
}

static void ui_app_lang_changed1(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
			&locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void ui_app_orient_changed1(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void ui_app_region_changed1(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void ui_app_low_battery1(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory1(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_MEMORY*/
}

static int main3(int argc, char *argv[]) {
	appdata_s1 ad = { 0, };
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create1;
	event_callback.terminate = app_terminate1;
	event_callback.pause = app_pause1;
	event_callback.resume = app_resume1;
	event_callback.app_control = app_control1;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, ui_app_low_battery1, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, ui_app_low_memory1, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed1, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed1, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed1, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
