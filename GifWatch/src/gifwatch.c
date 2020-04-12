#include <tizen.h>
#include "gifwatch.h"
#include <math.h>
#include <app_control.h>
#include "util.h"
#include "custom_ui.h"
char *month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
		"Sep", "Oct", "Nov", "Dec" };
char *week[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *week_cn[7] = { "周日", "周一", "周二", "周三", "周四", "周五", "周六" };
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)

#define TEXT_BUF_SIZE 256
double* yin;
double* yang;
char watch_text[TEXT_BUF_SIZE];
char **p;
char * yin_color = "#383838"; //"#0d4261";
char * yang_color = "#FFFFFF";
char image_path[PATH_MAX];
int count = 60;

/* 把角度转换为所对应的弧度 */
static Eina_Bool animate_cb(void *a);
static void app_data_init(void *data);

static void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient) {
	int hour24, minute, second;
	/* Get current time */
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);
	ad->second = second;
	if (hour24 == ad->hour && minute == ad->minute) {
	} else {	//&& second != ad->second
		ad->hour = hour24;
		ad->minute = minute;
		snprintf(watch_text, TEXT_BUF_SIZE, "<align=center>%02d:%02d</align>",
				hour24, minute);
		/* Set label */
		elm_object_text_set(ad->label_time, watch_text);
	}
}

// resume animate callback
static Eina_Bool animate_cb(void *a) {
	appdata_s *ad = (appdata_s*) a;
	ad->an += 1;
	int c = ad->frame_count;
	int index = ad->an % c;
	evas_object_image_animated_frame_set(ad->img, index);
	//dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ad->frame_count);

	if (index == 0) {
		//ecore_timer_freeze(ad->timer);
		ad->an = 0;
		return ECORE_CALLBACK_RENEW;	//ECORE_CALLBACK_DONE;
	} else
		return ECORE_CALLBACK_RENEW;
}

// always animate callback

void addImage(void *a, char *image_path, int width, int height, int x, int y) {
	//324,128
	appdata_s *ad = a;
	Evas *evas = evas_object_evas_get(ad->win);
	if (ad->img) {
	} else
		ad->img = evas_object_image_add(evas);

	evas_object_image_file_set(ad->img, image_path, NULL);
	int w, h;
	evas_object_image_load_size_get(ad->img, &w, &h);
	dlog_print(DLOG_ERROR, LOG_TAG, "image load size is %d %d", w, h);
	evas_object_image_fill_set(ad->img, 0, 0, width, height);
	evas_object_resize(ad->img, width, height);
	evas_object_show(ad->img);
	evas_object_move(ad->img, x, y);

	Evas_Object* obj = ad->img;
	if (evas_object_image_animated_get(obj)) {
		int loop_count;
		Evas_Image_Animated_Loop_Hint loop_type;
		double duration;
		ad->frame_count = evas_object_image_animated_frame_count_get(obj);
		dlog_print(DLOG_ERROR, LOG_TAG, "haha image frame count = %d",
				ad->frame_count);
		duration = evas_object_image_animated_frame_duration_get(obj, 1, 0);
		printf(
				"Frame 1's duration is . You had better set object's frame to 2 after this duration using timer\n");
		loop_count = evas_object_image_animated_loop_count_get(obj);
		printf("loop count is %d. You had better run loop %d times\n",
				loop_count, loop_count);

		loop_type = evas_object_image_animated_loop_type_get(obj);
		if (loop_type == EVAS_IMAGE_ANIMATED_HINT_LOOP)
			printf("You had better set frame like 1->2->3->1->2->3...\n");
		else if (loop_type == EVAS_IMAGE_ANIMATED_HINT_PINGPONG)
			printf("You had better set frame like 1->2->3->2->1->2...\n");
		else
			printf("Unknown loop type\n");
		evas_object_image_animated_frame_set(obj, 1);
		printf("You set image object's frame to 1. You can see frame 1\n");
	}
}

//需要添加 label_time label_date
int which_object(void *a, int x, int y) {
	appdata_s * ad = a;
	dlog_print(DLOG_ERROR, LOG_TAG,
			"which_object event_down ta inde %d %d %d %d %d %d", x, y,
			ad->img_location_x, ad->img_location_y,
			ad->img_location_x + ad->img_width,
			ad->img_location_y + ad->img_height);
	if (x >= ad->tmp_location_x && x <= ad->tmp_location_x + ad->tmp_width
			&& y >= ad->tmp_location_y
			&& y <= ad->tmp_location_y + ad->tmp_height) {
		return 1;
	}
	return 0;
}

// make double click to new win.
Eina_Bool event_up(void *data, int type, void *ev) {

	appdata_s * ad = data;
	Ecore_Event_Mouse_Button *event = ev;

//	if (ad->current_win == 1) { // in custom window.
//		ad->moved_which = 0;
//		ad->isDown = 0;
//		return ECORE_CALLBACK_PASS_ON;
//	}

	dlog_print(DLOG_ERROR, LOG_TAG, "up current position event_up ta %d %d",
			event->root.x, event->root.y);
	double duration = ecore_time_unix_get() - ad->last_up_million;
	if (duration > 0.85) {
		ad->upx = event->root.x;
		ad->upy = event->root.y;
		ad->last_up_million = ecore_time_unix_get();
	} else {
		int dx = event->root.x - ad->upx;
		int dy = event->root.y - ad->upy;
		dlog_print(DLOG_ERROR, LOG_TAG,
				"current position event_up ta %d %d %lf", dx, dy, duration);
		if(ad->current_win == 0) start_custom(data);
		else if(ad->current_win == 1) end_custom(data);
		ad->last_up_million = 0;
		ad->upx = 0;
		ad->upy = 0;
	}

	return ECORE_CALLBACK_DONE;
}

static void create_base_gui(appdata_s *ad) {
	int ret;
	yin = get_color(yin_color);
	yang = get_color(yang_color);
	elm_config_accel_preference_override_set(EINA_TRUE);
	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}
	evas_object_resize(ad->win, ad->width, ad->height);

	ad->res_path = app_get_resource_path();
	if (ad->res_path) {
		snprintf(image_path, (int) PATH_MAX, "%s%s", ad->res_path, "cxk.gif");
	}
	ad->value = (char**) malloc(sizeof(char*) * 5);
	ad->value[0] = image_path;

	//compute image width height ,and other.
	app_data_init(ad);

	//"/opt/usr/home/owner/media/Images/aeb61f8d41746a601440a97b6aa4364b.gif"
	addImage(ad, image_path, ad->img_width, ad->img_height, ad->img_location_x,
			ad->img_location_y);

	/* Create Label */ //should custom it size color,location change.
	ad->label_time = elm_label_add(ad->win);
	evas_object_resize(ad->label_time, ad->label_time_width,
			ad->label_time_height);
	evas_object_move(ad->label_time, ad->label_time_location_x,
			ad->label_time_location_y);
	evas_object_show(ad->label_time);

	ad->label_date = elm_label_add(ad->win);
	evas_object_resize(ad->label_date, ad->label_date_width,
			ad->label_date_height);
	evas_object_move(ad->label_date, ad->label_date_location_x,
			ad->label_date_location_y);
	evas_object_show(ad->label_date);

	ad->watch_time = (watch_time_h*) malloc(sizeof(watch_time_h));
	ret = watch_time_get_current_time(ad->watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);
	ad->hour = -1;
	ad->minute = -1;
	/* Show window after base gui is set up */
	ad->handler_up = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,
			event_up, ad);
	evas_object_show(ad->win);
}

static void app_data_init(void *data) {
	appdata_s *ad = data;

	//get image w,h
	int w, h;
	get_size(*(ad->value),&w,&h);
	dlog_print(DLOG_INFO, LOG_TAG, "the image width height is: %d %d\n ", w, h);
	ad->img_width = w;
	ad->img_height = h;
	ad->img_location_x = ad->width/2 - ad->img_width / 2;
	ad->img_location_y = ad->width/2 - ad->img_height / 2;

	ad->label_time_width = 90;
	ad->label_time_height = 30;
	ad->label_time_location_x = 180 - ad->label_time_width / 2;
	ad->label_time_location_y = 90 - ad->label_time_height / 2;

	ad->label_date_width = 105;
	ad->label_date_height = 30;
	ad->label_date_location_x = 160 - ad->label_date_width / 2;
	ad->label_date_location_y = 120 - ad->label_date_height / 2;
	ad->fps = 16;
	ad->interval = 0.06;
}
static bool app_create(int width, int height, void *data) {
	appdata_s *ad = data;
	ad->width = width;
	ad->height = height;
	create_base_gui(ad);
	return true;
}

static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
	dlog_print(DLOG_ERROR, LOG_TAG, "app_pause ");
	appdata_s *ad = data;
	if (ad->key_back_tag == 1) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_pause ad->key_back_tag %d",
				ad->key_back_tag);
		ad->key_back_tag = 0;
	} else if (ad->timer) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_pause ad->key_back_tag %d",
				ad->key_back_tag);
		ecore_timer_freeze(ad->timer);
		ecore_event_handler_del(ad->handler_up);
		//evas_object_image_animated_frame_set(ad->img, 1);
	}

}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
	dlog_print(DLOG_ERROR, LOG_TAG, "app_resume ");
	appdata_s *ad = data;
	ad->resume = 1;
	if (ad->timer && ad->status_changed == 0) {
		ecore_timer_thaw(ad->timer);
	} else {
		if (ad->timer)
			ecore_timer_del(ad->timer);
		ad->status_changed = 0;
		ad->timer = ecore_timer_add(ad->interval, animate_cb, ad);
		dlog_print(DLOG_ERROR, LOG_TAG, "app_resume ad->interval %f",
				ad->interval);
	}
	//evas_object_show(ad->img);
	int ret = watch_time_get_current_time(ad->watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);

	int year, month, day;
	watch_time_get_year(*(ad->watch_time), &year);
	watch_time_get_month(*(ad->watch_time), &month);
	watch_time_get_day(*(ad->watch_time), &day);
	watch_time_get_day_of_week(*(ad->watch_time), &(ad->day_of_week));
	if (year != ad->year || month != ad->month || day != ad->day) {
		ad->year = year;
		ad->month = month;
		ad->day = day;
		snprintf(watch_text, TEXT_BUF_SIZE,
				"<align=center><font=Sans:style=Regular font_size=19>%d/%d %s</font></align>",
				ad->month, ad->day, week_cn[ad->day_of_week - 1]);
		elm_object_text_set(ad->label_date, watch_text);
		//星期日
	}
//	update_watch(ad, *(ad->watch_time), ad->ambient_mode);
}

/*
 * @brief This callback function is called once after the main loop of the application exits
 */
static void app_terminate(void *data) {
	/* Release all resources. */
	appdata_s *ad = data;
	cairo_surface_destroy(ad->surface);
	cairo_destroy(ad->cairo);
	watch_time_delete(*(ad->watch_time));
}

/*
 * @brief This function will be called at each second.
 */
static void app_time_tick(watch_time_h watch_time, void *data) {
	/* Called at each second while your app is visible. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time, 0);
}

/*
 * @brief This function will be called at each minute.
 */
static void app_ambient_tick(watch_time_h watch_time, void *data) {
	/* Called at each minute while the device is in ambient mode. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time, 1);
}

/*
 * @brief This function will be called when the ambient mode is changed
 */
static void app_ambient_changed(bool ambient_mode, void *data) {
	appdata_s *ad = data;
	ad->ambient_mode = ambient_mode;
}

/*
 * @brief This function will be called when the language is changed
 */
static void watch_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	app_event_get_language(event_info, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

/*
 * @brief This function will be called when the region is changed
 */
static void watch_app_region_changed(app_event_info_h event_info,
		void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

/*
 * @brief Main function of the application
 */
int main(int argc, char *argv[]) {
	appdata_s ad = { 0, };
	int ret = 0;

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
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d",
				ret);

	return ret;
}
