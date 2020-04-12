
/*earth in center,地球月球和月球卫星*/
#include <tizen.h>
#include "solarserialwatch.h"
#include <cairo.h>
#include <math.h>
//#include "googlecode.c"
/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)

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
	cairo_pattern_t* earth_pattern;
	cairo_pattern_t* moon_pattern;
	bool ambient_mode;
} appdata_s;
double* moon;
double* earth;
double* sun;
double* mar;

double interval = 0.1;
char * moon_color = "#FFFFA0";
char * sun_color = "#FFFFA0	";
char * earth_color = "#4169E1";
char * mar_color= "#E46908";
double * sun_point;
double * moon_point;

#define TEXT_BUF_SIZE 256
double ** earth_locations;

/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)

double getNumber(char a) {
	double res = 0;
	if (a >= 'A' && a <= 'F')
		res = (a - 'A' + 10);
	else
		res = (a - '0');
	return res;
}
double* get_color(char* in) {
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

double * get_next_earth_position(int hour24) {
	return earth_locations[hour24 - 1];
}
int init_solar_system(appdata_s *ad) {
	sun = get_color(sun_color);
	moon = get_color(moon_color);
	earth = get_color(earth_color);
	mar = get_color(mar_color);
	/**compute the locations*/
	/**create parttern for sun erath and moon.*/
	int cx, cy, size;
	size = ad->width / 2;
	cx = size;
	cy = size;

	ad->sun_pattern = cairo_pattern_create_radial(-20, -20, 0, -20, -20,
			size);
	cairo_pattern_add_color_stop_rgba(ad->sun_pattern, 0.05, sun[0] - 0.1,
			sun[1] - 0.1, sun[2] - 0.1,1);
	cairo_pattern_add_color_stop_rgba(ad->sun_pattern, 0.5, sun[0], sun[1],
			sun[2],1);
	cairo_pattern_add_color_stop_rgba(ad->sun_pattern, 0.9, sun[0], sun[1],
			sun[2],0);

	//actually is mar
	ad->earth_pattern = cairo_pattern_create_radial(cx, cy, 0, cx, cy,
			size / 4);
	cairo_pattern_add_color_stop_rgba(ad->earth_pattern, 0.05, mar[0] - 0.1,
			mar[1] - 0.1, mar[2] - 0.1,1.0);
	cairo_pattern_add_color_stop_rgba(ad->earth_pattern, 0.5, mar[0], mar[1],
			mar[2],1.0);
	cairo_pattern_add_color_stop_rgba(ad->earth_pattern, 0.9, mar[0], mar[1],
				mar[2],0);


	return 0;
}

void update_solar_system(appdata_s *ad, int hour24, int minute, int second,
		int msecond) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;

	/* 绘制背景黑 */
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//draw sun in edge
	cairo_arc(ad->cairo, -20, -20, r, ANGLE(0), ANGLE(120));
	cairo_set_source(ad->cairo, ad->sun_pattern);
	cairo_fill(ad->cairo);

	//draw earth in center
	cairo_arc(ad->cairo, cx, cy, r / 4, ANGLE(0), ANGLE(360));
	cairo_set_source(ad->cairo, ad->earth_pattern);
	cairo_fill(ad->cairo);

	double earth_side, earth_radian;
	double earth_center_x, earth_center_y;

	earth_side = r / 1.7;
	earth_radian = -(second * 1000 + msecond) * (M_PI / 180);
	double a = 22 * M_PI / 180;
	earth_center_x = cx + 20
			- (earth_side * 1.5 * sin(earth_radian * 0.024) * cos(a)
					+ earth_side / 2 * cos(earth_radian * 0.024) * sin(a));
	earth_center_y = cy + 30
			- (earth_side / 2  * cos(earth_radian * 0.024) * cos(a)
					- earth_side * 1.5 * sin(earth_radian * 0.024) * sin(a));

	ad->moon_pattern = cairo_pattern_create_radial(earth_center_x,
			earth_center_y, 0, earth_center_x, earth_center_y, r / 10);
	cairo_pattern_add_color_stop_rgba(ad->moon_pattern, 0.05, moon[0] - 0.1,
			moon[1] - 0.1, moon[2] - 0.1,1);
	cairo_pattern_add_color_stop_rgba(ad->moon_pattern, 0.4, moon[0], moon[1],
			moon[2],1);
	cairo_pattern_add_color_stop_rgba(ad->moon_pattern, 0.9, moon[0], moon[1],
			moon[2],0);

	cairo_arc(ad->cairo, earth_center_x, earth_center_y, r / (10 * 1.2),
			ANGLE(0), ANGLE(360));
	cairo_set_source(ad->cairo, ad->moon_pattern);
	cairo_fill(ad->cairo);

	cairo_pattern_destroy(ad->moon_pattern);

	/****draw 卫星***/
	cairo_set_source_rgba(ad->cairo, moon[0], moon[1], moon[2], 1);
	double radian = -(second * 1000 + msecond) * (M_PI / 180);
	double inner_x, inner_y, side;
	side = r;
	inner_x = earth_center_x - (16 * sin(radian * 0.024));
	inner_y = earth_center_y - (16 * cos(radian * 0.024));

	cairo_arc(ad->cairo, inner_x, inner_y, 2, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);
	char watch_text[TEXT_BUF_SIZE];
	cairo_set_source_rgba(ad->cairo, 0,0,0, 1);
	cairo_select_font_face(ad->cairo, "Comforta", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %02d:%02d", hour24,
			minute);
	cairo_set_font_size(ad->cairo, 25);

	snprintf(watch_text, TEXT_BUF_SIZE, "%02d:%02d", hour24, minute);
	cairo_text_extents_t extents;
	cairo_text_extents(ad->cairo, watch_text, &extents);
	//cairo_move_to(cr, x - extents.width/2, y);
	cairo_move_to(ad->cairo, size / 2 - extents.width / 2, size / 1.1);
	cairo_show_text(ad->cairo, watch_text);

	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
}

static long long _get_current_ms_time(void) {
	struct timespec tp;
	long long res = 0;

	if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
		/*
		 * Zero mean invalid time
		 */
		return 0;
	} else {
		/*
		 * Calculate milliseconds time
		 */
		res = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
		return res;
	}
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
		ecore_timer_del(ad->timer);
}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
	appdata_s *ad = data;
	if (ad->timer) {
		ecore_timer_del(ad->timer);
	}
	ad->start_interval = _get_current_ms_time();
	ad->timer = ecore_timer_add(interval, _timer1_cb, ad);
	int hour24, minute, second;

	watch_time_h watch_time = NULL;
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
}

/*
 * @brief This callback function is called once after the main loop of the application exits
 */
static void app_terminate(void *data) {
	/* Release all resources. */
	appdata_s *ad = data;
	/* Destroy cairo surface and context */
	cairo_surface_destroy(ad->surface);
	cairo_destroy(ad->cairo);

	if (ad->sun_pattern) {
		cairo_pattern_destroy(ad->sun_pattern);
	}
	if (ad->earth_pattern)
		cairo_pattern_destroy(ad->earth_pattern);
	if (ad->moon_pattern)
		cairo_pattern_destroy(ad->moon_pattern);
}

/*
 * @brief This function will be called at each second.
 */
static void app_time_tick(watch_time_h watch_time, void *data) {
	/* Called at each second while your app is visible. Update watch UI. */

	appdata_s *ad = data;
//	if(ad->ambient_mode == 0)
//	update_watch(ad, watch_time, 0);
	if (!ad->timer) {
		app_resume(data);
	}
}

/*
 * @brief This function will be called at each minute.
 */
static void app_ambient_tick(watch_time_h watch_time, void *data) {
	/* Called at each minute while the device is in ambient mode. Update watch UI. */
	appdata_s *ad = data;
//	update_watch(ad, watch_time, 1);
	if (ad->ambient_mode && !ad->timer) {
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
			ecore_timer_del(ad->timer);
			ad->timer = NULL;
		}
	} else {
		if (!ambient_mode) {
			if (!ad->timer) {
				app_resume(data);
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
