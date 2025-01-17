#include <tizen.h>
#include "iwatch.h"
#include <app_control.h>
#include <efl_extension_events.h>
#include "basicui3.c"
typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;

	Evas_Object *win1;
	Evas_Object *conform1;
	int width, height;
	Evas_Object *label1;
	int argc;
	char **argv;
} appdata_s;
#define TEXT_BUF_SIZE 256

static void create_ui_app(void *data, Evas_Object *obj, void *event_info){
	appdata_s *ad = data;
	int n = ad->argc;
	char**p = ad->argv;
	char*m[n];
	for(int i=0;i<n;i++)
	{
		m[i] = p[i];
	}
	main3(n,m);
}
void create_new_win(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = data;
	ad->win1 = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win1, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win1)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win1,
				(const int *) (&rots), 4);
	}
	evas_object_resize(ad->win1, ad->width, ad->height);

	evas_object_smart_callback_add(ad->win1, "delete,request",
			win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win1, EEXT_CALLBACK_BACK, win_back_cb,
			ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	 elm_conformant is mandatory for base gui to have proper size
	 when indicator or virtual keypad is visible. */

	ad->conform1 = elm_conformant_add(ad->win1);
	elm_win_indicator_mode_set(ad->win1, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win1, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform1, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win1, ad->conform1);
	evas_object_show(ad->conform1);

	/* Label */
	/* Create an actual view of the base gui.
	 Modify this part to change the view. */
	ad->label1 = elm_label_add(ad->conform1);
	elm_object_text_set(ad->label1, "<align=center>Hello Tizen</align>");
	evas_object_size_hint_weight_set(ad->label1, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform1, ad->label1);

	Evas_Object *bt = elm_button_add(ad->win1);
	elm_object_text_set(bt, "Launch");
	evas_object_resize(bt, 130, 50);
	evas_object_move(bt, 150, 250);
	evas_object_show(bt);
	evas_object_smart_callback_add(bt, "clicked", btn_file_select_cb, ad);
	/* Show window after base gui is set up */
	evas_object_show(ad->win1);
}
static void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient) {
	char watch_text[TEXT_BUF_SIZE];
	int hour24, minute, second;

	if (watch_time == NULL)
		return;

	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);
	if (!ambient) {
		snprintf(watch_text, TEXT_BUF_SIZE,
				"<align=center>Hello Watch<br/>%02d:%02d:%02d</align>", hour24,
				minute, second);
	} else {
		snprintf(watch_text, TEXT_BUF_SIZE,
				"<align=center>Hello Watch<br/>%02d:%02d</align>", hour24,
				minute);
	}

	elm_object_text_set(ad->label, watch_text);
}

static void create_base_gui(appdata_s *ad, int width, int height) {
	int ret;
	watch_time_h watch_time = NULL;

	/* Window */
	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, width, height);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label*/
	ad->label = elm_label_add(ad->conform);
	evas_object_resize(ad->label, width, height / 3);
	evas_object_move(ad->label, 0, height / 3);
	evas_object_show(ad->label);

	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);

	update_watch(ad, watch_time, 0);
	watch_time_delete(watch_time);

	Evas_Object *bt = elm_button_add(ad->win);
	elm_object_text_set(bt, "Launch");
	evas_object_resize(bt, 130, 50);
	evas_object_move(bt, 150, 250);
	evas_object_show(bt);
	evas_object_smart_callback_add(bt, "clicked", create_ui_app, ad);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool app_create(int width, int height, void *data) {
	/* Hook to take necessary actions before main event loop starts
	 Initialize UI resources and application's data
	 If this function returns true, the main loop of application starts
	 If this function returns false, the application is terminated */
	appdata_s *ad = data;
	ad->width = width;
	ad->height = height;
	create_base_gui(ad, width, height);

	return true;
}

static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
}

static void app_terminate(void *data) {
	/* Release all resources. */
}

static void app_time_tick(watch_time_h watch_time, void *data) {
	/* Called at each second while your app is visible. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time, 0);
}

static void app_ambient_tick(watch_time_h watch_time, void *data) {
	/* Called at each minute while the device is in ambient mode. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time, 1);
}

static void app_ambient_changed(bool ambient_mode, void *data) {
	/* Update your watch UI to conform to the ambient mode */
}

static void watch_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	app_event_get_language(event_info, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void watch_app_region_changed(app_event_info_h event_info,
		void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

int main(int argc, char *argv[]) {
	appdata_s ad = { 0, };
	int ret = 0;
	ad.argc = argc;
	ad.argv = argv;
//	for(int i=0;i<argc;i++)
//	{
//		ad.argv[i] =
//	}


	watch_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, watch_app_lang_changed, &ad);
	watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, watch_app_region_changed, &ad);

	ret = watch_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d",
				ret);
	}

	return ret;
}

