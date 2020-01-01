/*circle with zhizhen */
#include <tizen.h>
#include "taijiwatch.h"
#include <cairo.h>
#include <math.h>
#include <sensor.h>
/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180)

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

	int hour24_x, hour24_y;
	int minute_x, minute_y;
	int second_x, second_y;
	int second_bar_x, second_bar_y;
	double * background;
	double * color_large_1;
	double * color_large_2;
	double * color_large_3;

	double * color_hour;
	double * color_minute;
	double * color_second;
} appdata_s;

#define TEXT_BUF_SIZE 256

double getNumber(char a) {
	double res = 0;
	if (a >= 'A' && a <= 'F')
		res = (a - 'A' + 10);
	else
		res = (a - '0');
	return res;
}
double* get_color(char* in) {
	double* res = (double*) malloc(3 * sizeof(double));
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
void free_memory(appdata_s *ad) {
	free(ad->background);
	free(ad->color_large_1);
	free(ad->color_large_1);
	free(ad->color_large_3);
	free(ad->color_hour);
	free(ad->color_minute);
	free(ad->color_second);
}
int init_watch(appdata_s *ad) {
//	background = (double*) malloc(3 * sizeof(double));
//	color_large_1 = get_color("#3F3736");
//	color_large_2 = get_color("#7E7F7A");
//	color_large_3 = get_color("#C1ABAD");

//245,2,58;153,211,29;7,217,230;
//123,22,112;231,234,118;52,182,112;
//43, 153, 92, 254, 227, 138, 49, 70, 179
//int colors[9] = { 245, 2, 58, 43, 153, 92, 253, 226, 75 };

	ad->background = (double*) malloc(3 * sizeof(double));
	ad->color_large_1 = (double*) malloc(3 * sizeof(double));
	ad->color_large_2 = (double*) malloc(3 * sizeof(double));
	ad->color_large_3 = (double*) malloc(3 * sizeof(double));
	ad->color_hour = (double*) malloc(3 * sizeof(double));
	ad->color_minute = (double*) malloc(3 * sizeof(double));
	ad->color_second = (double*) malloc(3 * sizeof(double));

	ad->background[0] = 0;
	ad->background[1] = 0;
	ad->background[2] = 0;

	//245,2,58;153,211,29;7,217,230;
	//123,22,112;231,234,118;52,182,112;
	//int colors[9] = { 245, 2, 58, 153, 211, 29, 7, 217, 230 };
	int colors[9] = { 245, 2, 58, 43, 153, 92, 253, 226, 75 };

	ad->color_large_1[0] = (double) colors[0] / 256;
	ad->color_large_1[1] = (double) colors[1] / 256;
	ad->color_large_1[2] = (double) colors[2] / 256;

	ad->color_large_2[0] = (double) colors[3] / 256;
	ad->color_large_2[1] = (double) colors[4] / 256;
	ad->color_large_2[2] = (double) colors[5] / 256;

	ad->color_large_3[0] = (double) colors[6] / 256;
	ad->color_large_3[1] = (double) colors[7] / 256;
	ad->color_large_3[2] = (double) colors[8] / 256;

	ad->color_second[0] = 1;
	ad->color_second[1] = 0;
	ad->color_second[2] = 0;
	ad->color_minute[0] = 1;
	ad->color_minute[1] = 1;
	ad->color_minute[2] = 1;

	ad->color_hour[0] = 1;
	ad->color_hour[1] = 1;
	ad->color_hour[2] = 1;
	//background
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	cairo_set_source_rgba(ad->cairo, ad->background[0], ad->background[1],
			ad->background[2], 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	cairo_set_source_rgba(ad->cairo, ad->color_large_1[0], ad->color_large_1[1],
			ad->color_large_1[2], 1);
	cairo_set_line_width(ad->cairo, 20);
	cairo_arc(ad->cairo, cx, cy, r - 13, 0, 2 * M_PI);
	cairo_stroke(ad->cairo);

	cairo_set_source_rgba(ad->cairo, ad->color_large_2[0], ad->color_large_2[1],
			ad->color_large_2[2], 1);
	cairo_set_line_width(ad->cairo, 20);
	cairo_arc(ad->cairo, cx, cy, r - 35, 0, 2 * M_PI);
	cairo_stroke(ad->cairo);

	cairo_set_source_rgba(ad->cairo, ad->color_large_3[0], ad->color_large_3[1],
			ad->color_large_3[2], 1);
	cairo_set_line_width(ad->cairo, 20);
	cairo_arc(ad->cairo, cx, cy, r - 57, 0, 2 * M_PI);
	cairo_stroke(ad->cairo);
	ad->hour24_x = cx;
	ad->hour24_y = cy;
	ad->minute_x = cx;
	ad->minute_y = cy;
	ad->second_x = cx;
	ad->second_y = cy;
	ad->second_bar_x = cx;
	ad->second_bar_y = cy;

	return 0;
}

void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;

	int hour24, minute, second;
	/* Get current time */
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);

	/* Variables for display time */
	double side, radian;
	cairo_line_cap_t line_cap_style;
	cairo_line_join_t line_join_style;

	/* Set hour hand line style, width */
	line_cap_style = CAIRO_LINE_CAP_ROUND;
	line_join_style = CAIRO_LINE_JOIN_BEVEL;
	cairo_set_line_join(ad->cairo, line_join_style);
	cairo_set_line_cap(ad->cairo, line_cap_style);

	//clear history
	cairo_set_line_width(ad->cairo, 12);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_move_to(ad->cairo, cx, cy);
	cairo_line_to(ad->cairo, ad->hour24_x, ad->hour24_y);
	cairo_stroke(ad->cairo);
	//clear history
	cairo_set_line_width(ad->cairo, 8);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_move_to(ad->cairo, cx, cy);
	cairo_line_to(ad->cairo, ad->minute_x, ad->minute_y);
	cairo_stroke(ad->cairo);
	//clear history
//	cairo_set_line_width(ad->cairo, 6);
//	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
//	cairo_move_to(ad->cairo, cx, cy);
//	cairo_line_to(ad->cairo, ad->second_x, ad->second_y);
//	cairo_stroke(ad->cairo);
	//clear history
	cairo_set_line_width(ad->cairo, 6);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_move_to(ad->cairo, ad->second_x, ad->second_y);
	cairo_line_to(ad->cairo, ad->second_bar_x, ad->second_bar_y);
	cairo_stroke(ad->cairo);

	/* Set hour hand line color */
	side = r / 3;
	radian = -hour24 * (M_PI / 180);
	ad->hour24_x = cx - (side * sin(radian * 30));
	ad->hour24_y = cy - (side * cos(radian * 30));
	cairo_set_line_width(ad->cairo, 11);
	cairo_set_source_rgba(ad->cairo, ad->color_hour[0], ad->color_hour[1],
			ad->color_hour[2], 1);
	cairo_move_to(ad->cairo, cx, cy);
	cairo_line_to(ad->cairo, ad->hour24_x, ad->hour24_y);
	cairo_stroke(ad->cairo);

	/* Set minute hand line color */
	cairo_set_line_width(ad->cairo, 7);
	cairo_set_source_rgba(ad->cairo, ad->color_minute[0], ad->color_minute[1],
			ad->color_minute[2], 1);
	side = r / 1.8;
	radian = -minute * (M_PI / 180);
	ad->minute_x = cx - (side * sin(radian * 6));
	ad->minute_y = cy - (side * cos(radian * 6));
	cairo_move_to(ad->cairo, cx, cy);
	cairo_line_to(ad->cairo, ad->minute_x, ad->minute_y);
	cairo_stroke(ad->cairo);

	/* Set second hand line style, width */
	if (!ambient) {
		cairo_set_line_width(ad->cairo, 4);
		cairo_set_source_rgba(ad->cairo, ad->color_second[0],
				ad->color_second[1], ad->color_second[2], 1);
		side = r -77;
		radian = -second * (M_PI / 180);
		ad->second_x = cx - (side * sin(radian * 6));
		ad->second_y = cy - (side * cos(radian * 6));
		cairo_move_to(ad->cairo, cx, cy);
		cairo_line_to(ad->cairo, ad->second_x, ad->second_y);
		cairo_stroke(ad->cairo);

		cairo_set_line_width(ad->cairo, 4);
		cairo_set_source_rgba(ad->cairo, ad->color_second[0],
				ad->color_second[1], ad->color_second[2], 1);
		int ra = 6;
		ad->second_bar_x = ((ra+1)*cx - ad->second_x)/ra;
		ad->second_bar_y = ((ra+1)*cy - ad->second_y)/ra;
		cairo_move_to(ad->cairo, ad->second_bar_x, ad->second_bar_y);
		cairo_line_to(ad->cairo, cx, cy);
		cairo_stroke(ad->cairo);
	}
	//center red points
	cairo_set_source_rgba(ad->cairo, ad->color_second[0], ad->color_second[1],
			ad->color_second[2], 1);
	cairo_set_line_width(ad->cairo, 6);
	cairo_arc(ad->cairo, cx, cy, 6, 0, 2 * M_PI);
	cairo_fill(ad->cairo);

	cairo_set_source_rgba(ad->cairo, 0,0,0, 1);
	cairo_set_line_width(ad->cairo, 2);
	cairo_arc(ad->cairo, cx, cy, 2, 0, 2 * M_PI);
	cairo_fill(ad->cairo);
	/* Display this cairo watch on screen */
	cairo_surface_flush(ad->surface);
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
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
	watch_time_h watch_time = NULL;

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

	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);
	/* Display first screen of watch */
	init_watch(ad);
	update_watch(ad, watch_time, 0);

	/* Destroy variable */
	watch_time_delete(watch_time);
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

}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
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
	free_memory(ad);
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
	/* Update your watch UI to conform to the ambient mode */
	//appdata_s *ad = data;
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
