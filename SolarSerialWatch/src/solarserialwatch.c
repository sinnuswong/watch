#include <tizen.h>
#include "solarserialwatch.h"
/*newest sun in center and earth rotate and moon*/
#include <cairo.h>
#include <math.h>
#include <time.h>
/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)
double interval = 0.1;
#define TEXT_BUF_SIZE 20
typedef struct appdata {
	/* Variables for basic UI contents */
	Evas_Object *win;
	Evas_Object *img;
	//Evas_Object *label;
	/* Variables for watch size information */
	int width;
	int height;
	/* Variables for cairo image backend contents */
	cairo_t *cairo;
	cairo_surface_t *surface;
	unsigned char *pixels;
	Ecore_Timer *timer;
	long long start_interval;
	int hour24, minute, second;
	cairo_pattern_t* sun_pattern;
	int cur_position_x;
	int cur_position_y;

	unsigned char isDown;
	int prex, prey;
	int dx, dy;
	int downX, downY;
	double downStamp;
	double avg_speedX;
	double avg_speedY;
	Ecore_Event_Handler * handler;
	cairo_pattern_t* earth_pattern;
	cairo_pattern_t* moon_pattern;
	bool ambient_mode;
	double earth_center_x, earth_center_y;
	double earth_side, earth_radian, moon_radian, moon_center_x, moon_center_y;

	char *watch_text;
	cairo_text_extents_t *extents;

	double* moon_color;
	double* earth_color;
	double* sun_color;

} appdata_s;
char * moon_color_s = "#FFFFA0";
char * sun_color_s = "#FFFFA0";
char * earth_color_s = "#4169E1";

static double getNumber(char a) {
	double res = 0;
	if (a >= 'A' && a <= 'F')
		res = (a - 'A' + 10);
	else
		res = (a - '0');
	return res;
}
static double* get_color(char* in) {
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %s",in);
	//int temp = getNumber()*16+(in[2]-'A'+10);
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %d",(in[1]));
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %d",('A'));
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %d",temp);
	double* res = (double*) malloc(3 * sizeof(double));

//		return res;
	double a, b, c;
	a = getNumber(in[1]) * 16 + getNumber(in[2]);
	b = getNumber(in[3]) * 16 + getNumber(in[4]);
	c = getNumber(in[5]) * 16 + getNumber(in[6]);
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %f,%f,%f",a,b,c);
	res[0] = a / 255;
	res[1] = b / 255;
	res[2] = c / 255;
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %f,%f,%f",res[0],res[1],res[2]);
	return res;
}
static long long _get_current_ms_time(void) {
	return (long long) (ecore_time_unix_get() * 1000);
}

static void free_data(appdata_s *ad) {
	ecore_timer_del(ad->timer);
	cairo_surface_destroy(ad->surface);
	cairo_destroy(ad->cairo);

	if (ad->sun_pattern) {
		cairo_pattern_destroy(ad->sun_pattern);
	}
	if (ad->earth_pattern)
		cairo_pattern_destroy(ad->earth_pattern);
	if (ad->moon_pattern)
		cairo_pattern_destroy(ad->moon_pattern);

	free(ad->moon_color);
	free(ad->earth_color);
	free(ad->sun_color);

	free(moon_color_s);
	free(sun_color_s);
	free(earth_color_s);
	free(ad->watch_text);
	free(ad->extents);
}

int init_solar_system(appdata_s *ad) {
	ad->sun_color = get_color(sun_color_s);
	ad->moon_color = get_color(moon_color_s);
	ad->earth_color = get_color(earth_color_s);
	/**compute the locations*/
	/**create parttern for sun erath and moon.*/
	int cx, cy, size;
	size = ad->width / 2;
	cx = size;
	cy = size;
	int r = size;
	ad->cur_position_x = cx;
	ad->cur_position_y = cy;
	ad->sun_pattern = cairo_pattern_create_radial(ad->cur_position_x,
			ad->cur_position_y, 0, ad->cur_position_x, ad->cur_position_y,
			size / 4);
	cairo_pattern_add_color_stop_rgba(ad->sun_pattern, 0.05, ad->sun_color[0] - 0.1,
			ad->sun_color[1] - 0.1, ad->sun_color[2] - 0.1, 1);
	cairo_pattern_add_color_stop_rgba(ad->sun_pattern, 0.5, ad->sun_color[0],
			ad->sun_color[1], ad->sun_color[2], 1);
	cairo_pattern_add_color_stop_rgba(ad->sun_pattern, 0.9, ad->sun_color[0],
			ad->sun_color[1], ad->sun_color[2], 0);
	//cairo_pattern_destroy(pat3);

	/* 绘制背景黑 和 太阳 */
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);
	//draw sun in center
	cx = ad->cur_position_x;
	cy = ad->cur_position_y;
	cairo_arc(ad->cairo, cx, cy, r / 4, ANGLE(0), ANGLE(360));
	cairo_set_source(ad->cairo, ad->sun_pattern);
	cairo_fill(ad->cairo);

	//init text size and width and length
	ad->watch_text = (char*) malloc(TEXT_BUF_SIZE * sizeof(char));
	ad->extents = (cairo_text_extents_t *) malloc(sizeof(cairo_text_extents_t));

	cairo_set_source_rgba(ad->cairo, 1, 1, 1, 1);
	cairo_select_font_face(ad->cairo, "Courier", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(ad->cairo, 20);

	snprintf(ad->watch_text, TEXT_BUF_SIZE, "%02d:%02d", 11, 59);
	cairo_text_extents(ad->cairo, ad->watch_text, ad->extents);
	return 0;
}

static void update_solar_system(appdata_s *ad, int hour24, int minute,
		int second, int msecond) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;

	//clear earth history
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, ad->earth_center_x, ad->earth_center_y, r / 12,
			ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//clear moon history
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, ad->moon_center_x, ad->moon_center_y, 3, ANGLE(0),
			ANGLE(360));
	cairo_fill(ad->cairo);
	//remove history.and set black
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_rectangle(ad->cairo, size - ad->extents->width - 20,
			size / 2 - ad->extents->height / 2, ad->extents->width,
			ad->extents->height);
	cairo_fill(ad->cairo);

	//draw earth
	ad->earth_side = r / 1.7;
	ad->earth_radian = -(second * 1000 + msecond) * (M_PI / 180);
	double a = 22 * M_PI / 180;
	ad->earth_center_x = cx + 20
			- (ad->earth_side * 1.5 * sin(ad->earth_radian * 0.006) * cos(a)
					+ ad->earth_side / 1.5 * cos(ad->earth_radian * 0.006)
							* sin(a));
	ad->earth_center_y = cy - 10
			- (ad->earth_side / 1.5 * cos(ad->earth_radian * 0.006) * cos(a)
					- ad->earth_side * 1.5 * sin(ad->earth_radian * 0.006)
							* sin(a));

	ad->earth_pattern = cairo_pattern_create_radial(ad->earth_center_x,
			ad->earth_center_y, 0, ad->earth_center_x, ad->earth_center_y,
			r / 10);
	cairo_pattern_add_color_stop_rgba(ad->earth_pattern, 0.05,
			ad->earth_color[0] - 0.1, ad->earth_color[1] - 0.1, ad->earth_color[2] - 0.1,
			1);
	cairo_pattern_add_color_stop_rgba(ad->earth_pattern, 0.5, ad->earth_color[0],
			ad->earth_color[1], ad->earth_color[2], 1);
	cairo_pattern_add_color_stop_rgba(ad->earth_pattern, 0.9, ad->earth_color[0],
			ad->earth_color[1], ad->earth_color[2], 0);
	cairo_arc(ad->cairo, ad->earth_center_x, ad->earth_center_y, r / 12,
			ANGLE(0), ANGLE(360));
	cairo_set_source(ad->cairo, ad->earth_pattern);
	cairo_fill(ad->cairo);
	cairo_pattern_destroy(ad->earth_pattern);

	/****draw moon***/
	ad->moon_radian = -(second * 1000 + msecond) * (M_PI / 180);
	ad->moon_center_x = ad->earth_center_x
			- (16 * sin(ad->moon_radian * 0.024));
	ad->moon_center_y = ad->earth_center_y
			- (16 * cos(ad->moon_radian * 0.024));
	cairo_set_source_rgba(ad->cairo, ad->moon_color[0], ad->moon_color[1],
			ad->moon_color[2], 1);
	cairo_arc(ad->cairo, ad->moon_center_x, ad->moon_center_y, 2, ANGLE(0),
			ANGLE(360));
	cairo_fill(ad->cairo);

	//add new time text
	cairo_set_source_rgba(ad->cairo, 1, 1, 1, 1);
	cairo_select_font_face(ad->cairo, "Courier", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(ad->cairo, 20);

	snprintf(ad->watch_text, TEXT_BUF_SIZE, "%02d:%02d", hour24, minute);
	cairo_move_to(ad->cairo, size - ad->extents->width - 20,
			size / 2 + ad->extents->height / 2);
	cairo_show_text(ad->cairo, ad->watch_text);

	/* Display this cairo watch on screen */
	cairo_surface_flush(ad->surface);
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
}

static Eina_Bool _timer1_cb(void *ad) {
	appdata_s *a = (appdata_s*) ad;
	long long cms = _get_current_ms_time();
	long long dms = cms - (a->start_interval);
	int d = dms / 1000;
	int misec = dms % 1000;

	long total = a->hour24 * 3600 + a->minute * 60 + a->second + d;
	int hour24 = total / 3600;
	int minute = (total % 3600) / 60;
	int second = total % 60;
//	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %02d:%02d:%02d:%02d",hour,minute,
//			second,misec);

	update_solar_system(a, hour24, minute, second, misec);
	return ECORE_CALLBACK_RENEW;
}
static Eina_Bool event_move(void *data, int type, void *ev) {
	appdata_s *ad = data;
	if (!ad->isDown)
		return ECORE_CALLBACK_DONE;

	Ecore_Event_Mouse_Move *event = ev;
	int cx, cy, size;
	size = ad->width / 2;
	int cur_x = event->root.x;
	int cur_y = event->root.y;
	int pre_x = ad->prex;
	int pre_y = ad->prey;

	ad->dx = cur_x - pre_x;
	ad->dy = cur_y - pre_y;
	int dx = ad->dx;
	int dy = ad->dy;

	dlog_print(DLOG_ERROR, LOG_TAG,
			"current position event_move ta index %d %d", ad->prex, ad->prey);
	ad->prex = cur_x;
	ad->prey = cur_y;

	//dlog_print(DLOG_ERROR, LOG_TAG, "current position %d %d",ev->cur.output.x,ev->cur.output.y);

	cx = ad->cur_position_x + dx / 1.05;
	cy = ad->cur_position_y + dy / 1.05;

	int ta = (cx - size) * (cx - size) + (cy - size) * (cy - size);
	//dlog_print(DLOG_ERROR, LOG_TAG, "current position ta %d", ta);
	//dlog_print(DLOG_ERROR, LOG_TAG, "current position ta %d", size * size);

	if (ta > 164 * 164) {
		return ECORE_CALLBACK_PASS_ON;
	}
	ad->cur_position_x = cx;
	ad->cur_position_y = cy;
//	_timer1_cb(ad);
	return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool event_down(void *data, int type, void *ev) {
	appdata_s * ad = data;
	Ecore_Event_Mouse_Button *event = ev;
	ad->isDown = 1;
	ad->prex = event->root.x;
	ad->prey = event->root.y;
	ad->downX = event->root.x;
	ad->downY = event->root.y;
	ad->downStamp = ecore_time_unix_get();
	dlog_print(DLOG_ERROR, LOG_TAG, "current position event_down ta inde %d %d",
			ad->prex, ad->prey);
	return ECORE_CALLBACK_DONE;
}

static Eina_Bool event_up(void *data, int type, void *ev) {
	appdata_s * ad = data;
	Ecore_Event_Mouse_Button *event = ev;
	ad->isDown = 0;
	//计算惯性动作
	double duration = ecore_time_unix_get() - ad->downStamp;
	dlog_print(DLOG_ERROR, LOG_TAG, "current position event_up moved time %lf",
			duration);
	int dx = event->root.x - ad->downX;
	int dy = event->root.y - ad->downY;

	ad->avg_speedX = (double) dx / duration;
	ad->avg_speedY = (double) dy / duration;
//	dlog_print(DLOG_ERROR, LOG_TAG, "current position event_up d and speed %d %d %lf %lf",dx,dy,avg_speedX,avg_speedY);
//	ecore_animator_timeline_add(2, scroll_animation_callback, ad);

	ad->downX = -1;
	ad->downY = -1;
	dlog_print(DLOG_ERROR, LOG_TAG, "current position event_up ta %d %d",
			ad->prex, ad->prey);
	return ECORE_CALLBACK_DONE;
}

static void create_base_gui(appdata_s *ad) {

	/*
	 * Window
	 * Get the watch application's elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	int ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, ad->width, ad->height);

	/* Create image */
	ad->img = evas_object_image_filled_add(evas_object_evas_get(ad->win));
	evas_object_image_size_set(ad->img, ad->width, ad->height);
	evas_object_resize(ad->img, ad->width, ad->height);
	evas_object_image_content_hint_set(ad->img,
			EVAS_IMAGE_CONTENT_HINT_DYNAMIC);
	evas_object_image_colorspace_set(ad->img, EVAS_COLORSPACE_ARGB8888);
	evas_object_show(ad->img);
//	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, event_move,
//			ad);
//	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, event_up,
//			ad);
//	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
//			event_down, ad);

	/* Create Cairo context */
	ad->pixels = (unsigned char*) evas_object_image_data_get(ad->img, 1);
	ad->surface = cairo_image_surface_create_for_data(ad->pixels,
			CAIRO_FORMAT_ARGB32, ad->width, ad->height, ad->width * 4);
	ad->cairo = cairo_create(ad->surface);
	init_solar_system(ad);
	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool app_create(int width, int height, void *data) {
	appdata_s *ad = data;
	ad->width = width;
	ad->height = height;
	create_base_gui(ad);
	return true;
}

/*
 * @brief This callback function is called when another application
 * sends the launch request to the application
 */
static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
	appdata_s *ad = data;
	if (ad->timer)
		ecore_animator_freeze(ad->timer);
}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
	appdata_s *ad = data;
	watch_time_h watch_time = NULL;
	int hour24, minute, second;
	int ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);

	/* Get current time */
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);
	watch_time_delete(watch_time);
	ad->hour24 = hour24;
	ad->minute = minute;
	ad->second = second;
	ad->start_interval = _get_current_ms_time();
	if (ad->timer) {
		ecore_animator_thaw(ad->timer);
	} else {
	ad->timer = ecore_timer_add(interval, _timer1_cb, ad);
}
}

/*
 * @brief This callback function is called once after the main loop of the application exits
 */
static void app_terminate(void *data) {
/* Release all resources. */
appdata_s *ad = data;
/* Destroy cairo surface and context */
free_data(ad);
}

/*
 * @brief This function will be called at each second.
 */
static void app_time_tick(watch_time_h watch_time, void *data) {
/* Called at each second while your app is visible. Update watch UI. */

appdata_s *ad = data;
//	if(ad->ambient_mode == 0)
//	update_watch(ad, watch_time, 0);
//	if (!ad->timer) {
//		app_resume(data);
//	}
}

/*
 * @brief This function will be called at each minute.
 */
static void app_ambient_tick(watch_time_h watch_time, void *data) {
/* Called at each minute while the device is in ambient mode. Update watch UI. */
appdata_s *ad = data;
//	update_watch(ad, watch_time, 1);
if (ad->ambient_mode) {
	int hour24, minute, second;

	/* Get current time */
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);

	ad->hour24 = hour24;
	ad->minute = minute;
	ad->second = second;
	update_solar_system(ad, hour24, minute, second, 0);
}
}

/*
 * @brief This function will be called when the ambient mode is changed
 */
static void app_ambient_changed(bool ambient_mode, void *data) {
/* Update your watch UI to conform to the ambient mode */
appdata_s *ad = data;
ad->ambient_mode = ambient_mode;
if (ambient_mode) {
	if (ad->timer) {
		ecore_timer_freeze(ad->timer);
	}
} else {
	if (!ambient_mode) {
		if (!ad->timer) {
			app_resume(data);
		} else {
			ecore_timer_thaw(ad->timer);
		}
	}
}
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
