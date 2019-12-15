#include "basicui.h"
#include <sensor.h>
#include <dlog.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
} appdata_s;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}
static Eina_Bool
_advance_frame(void *data, double pos)
{
    double frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_SINUSOIDAL, 12, 15);
    evas_object_resize(data, 50 * (1 + frame), 50 * (1 + frame));
    evas_object_move(data, 100 * frame, 100 * frame);
    evas_object_color_set(data, 255 * frame, 0, 255 * (1 - frame), 255);

    return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_advance_frame2(void *data, double pos)
{
    double frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_BOUNCE, 1.2, 50);
    evas_object_resize(data, 100 - (50 * frame), 100 - (50 * frame));
    evas_object_move(data, 100 * (1 - frame), 100 * (1 - frame));
    evas_object_color_set(data, 255 * (1 - frame), 0, 255 * frame, 255);

    return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_advance_frame3(void * data)
{
    static int x = 0;
    if (x >= 250)
        x = 0;
    evas_object_move(data, x += 2, 350);

    return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_start_second_anim(void *data)
{
    ecore_animator_frametime_set(1./10);
    ecore_animator_timeline_add(20, _advance_frame2, data);

    return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_freeze_third_anim(void *data)
{
    dlog_print(DLOG_ERROR, LOG_TAG, "ANIMATOR = %p", data);
    ecore_animator_freeze(data);

    return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_thaw_third_anim(void *data)
{
    ecore_animator_thaw(data);

    return ECORE_CALLBACK_CANCEL;
}

bool
example_sensor_recorder_callback(sensor_type_e type, sensor_recorder_data_h data, int remains,
                                 sensor_error_e error, void *user_data)
{
    int step;
    double distance;
    time_t start;
    time_t end;

    dlog_print(DLOG_INFO, "Test", "SENSOR_ERROR %d",error);
    if (error != SENSOR_ERROR_NONE){
    	dlog_print(DLOG_INFO, "Test", "SENSOR_ERROR_NONE %d",SENSOR_ERROR_NONE);
    	return false;
    }


    sensor_recorder_data_get_time(data, &start, &end);
    sensor_recorder_data_get_int(data, SENSOR_RECORDER_DATA_STEPS, &step);
    sensor_recorder_data_get_double(data, SENSOR_RECORDER_DATA_DISTANCE, &distance);

    dlog_print(DLOG_INFO, "Test", "steps is %d, distance is %f",step,distance);
    return true;
}
static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>Hello Tizen</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);


	/**/
	//Evas* canvas = evas_object_evas_get(ad->win);

	/* Evas */
	//Evas  *evas = evas_object_evas_get(ad->conform);

//	Evas *evas = evas_object_evas_get(ad->conform);
//
//	/* Rect */
//	Evas_Object * rect = evas_object_rectangle_add(evas);
//
//	evas_object_color_set(rect, 0, 0, 255, 255);
//	evas_object_resize(rect, 50, 50);
//	evas_object_show(rect);
//
//	Evas_Object *rect2 = evas_object_rectangle_add(evas);
//	evas_object_color_set(rect2, 0, 55, 0, 255);
//	evas_object_resize(rect2, 50, 50);
//	evas_object_show(rect2);
//
//	/* Animation */
//	ecore_animator_frametime_set(1. / 50);
	//Ecore_Animator *anim = ecore_animator_timeline_add(5, _advance_frame, rect);
	//Ecore_Animator *anim2 = ecore_animator_add(_advance_frame3, rect2);
	//Ecore_Timer *timer1 = ecore_timer_add(10, _start_second_anim, rect);
	//Ecore_Timer *timer2 = ecore_timer_add(5, _freeze_third_anim, rect2);
	//Ecore_Timer *timer3 = ecore_timer_add(10, _thaw_third_anim, rect2);


	bool supported = false;

	sensor_is_supported(SENSOR_HUMAN_PEDOMETER, &supported);
	if (!supported) {
		dlog_print(DLOG_INFO, "Test1", "Accelerometer is not supported on the current device");
	    /* Accelerometer is not supported on the current device */
	}
	else{
		dlog_print(DLOG_INFO, "Test1", "Accelerometer is supported on the current device");
	}
	sensor_recorder_query_h query;
	sensor_recorder_create_query(&query);
	/* Start 7 days ago */
	sensor_recorder_query_set_time(query, SENSOR_RECORDER_QUERY_START_TIME,
	                               (time_t)(time(NULL) - (7 * 24 * 3600)));
	/* End now */
	sensor_recorder_query_set_time(query, SENSOR_RECORDER_QUERY_END_TIME, time(NULL));
	/* Aggregate every 24 hours */
	sensor_recorder_query_set_int(query, SENSOR_RECORDER_QUERY_TIME_INTERVAL, 24 * 60);
	/* Start the aggregation at 7 AM */
	sensor_recorder_query_set_time(query, SENSOR_RECORDER_QUERY_ANCHOR_TIME, (time_t)(7 * 3600));

	sensor_recorder_read(SENSOR_HUMAN_PEDOMETER, query, example_sensor_recorder_callback, NULL);

	/* Show the window after the base GUI is set up */
	//dlog_print(DLOG_INFO, "Test1", "steps is %d, distance is %f",0,0);
	evas_object_show(ad->win);

}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
