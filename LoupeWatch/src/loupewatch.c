/*秒针是圆，时针分针为走针*/
#include <tizen.h>
#include "loupewatch.h"
#include <cairo.h>
#include <math.h>
/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)
#define TEXT_BUF_SIZE 256

typedef struct appdata {

	/* Variables for basic UI contents */
	Evas_Object *win;
	Evas_Object *img;
	Evas_Object **imgs;
	Evas_Object *indicator_blue;
	Evas_Object *indicator_red;
	Evas_Object *line_blue;

	Evas_Object *label;
	Evas_Object * label_date;
	Ecore_Timer *timer;
	char *res_path;
	/* Variables for watch size information */
	int width;
	int height;

	watch_time_h *watch_time;
	int hour, minute, second, msecond;
	int year, month, day, day_of_week;
	/* Variables for cairo image backend contents */
	cairo_t *cairo;
	cairo_surface_t *surface;
	unsigned char *pixels;
	int resume;
	int orient;
	double x, y;
	int frame_count;
	int an;

	int ambient_mode;
} appdata_s;
double* yin;
double* yang;
char watch_text[TEXT_BUF_SIZE];
char **p;
char * yin_color = "#383838"; //"#0d4261";
char * yang_color = "#FFFFFF";
char image_path[PATH_MAX];
int interval = 1;
int count = 60;
int loc[61][2];
char *month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
		"Sep", "Oct", "Nov", "Dec" };
char *week[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *week_cn[7] = { "周日", "周一", "周二", "周三", "周四", "周五", "周六" };

/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)
static Eina_Bool _timer1_cb(void *a);
static void compute_init(void *a);
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
void get_color_fo() {
	dlog_print(DLOG_ERROR, LOG_TAG, "get color %02x%02x%02x", 25, 11, 69);
}
static void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient) {
	int hour24, minute, second;
	/* Get current time */
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);
	ad->second = second;
	_timer1_cb(ad);
	if (hour24 == ad->hour && minute == ad->minute) {
	} else {	//&& second != ad->second
		ad->hour = hour24;
		ad->minute = minute;
		snprintf(watch_text, TEXT_BUF_SIZE, "<align=center>%02d:%02d</align>",
				hour24, minute);
		/* Set label */
		elm_object_text_set(ad->label, watch_text);
	}
}

static void compute_init(void *a) {
	double s = 515.7175208038408;
	double dl = s / count;
	double dy = 0.001;
	double f = 0.0008383;
	double l = 0;
	double x = 0;
	double y = 0;

	for (int i = 0; i < 60; i++) {
		l = 0;
		while (y < 470.65 && l < dl) {
			l = l + sqrt(4 * f * f * y * y + 1) * dy;
			y += dy;
		}
		x = f * y * y + 248.23;
		int dr_x = (int) (x * 360 / 529) - 12;
		int dr_y = (int) (y * 360 / 529) - 12;
		loc[i][0] = dr_x;
		loc[i][1] = dr_y;
	}
}
static Eina_Bool _timer1_cb(void *a) {
	appdata_s *ad = (appdata_s*) a;
	//0.000842     248,0;  434,470;
	//x = 0.0008383*y*y+248.23   433.93 470.65
	evas_object_move(ad->indicator_blue, loc[ad->second][0], loc[ad->second][1]);
//	if (ad->orient == 0) {
//		int t = ad->an;
//		t = t % 13;
////		snprintf(image_path, (int) PATH_MAX, "%s%d.png", ad->res_path, (t + 1));
////		evas_object_image_file_set(ad->img, image_path, NULL);
//		if(t==0){
//			evas_object_hide(ad->imgs[13]);
//		}
//		else{
//			evas_object_hide(ad->imgs[t]);
//		}
//		evas_object_show(ad->imgs[t+1]);
//		ad->an +=1;
//	}

//	evas_object_image_fill_set(ad->img, 0, 0, ad->width, ad->height);
//	evas_object_resize(ad->img, ad->width, ad->height);
//	evas_object_show(ad->img);
	return ECORE_CALLBACK_RENEW;
}

static void create_base_gui(appdata_s *ad) {
	int ret;
	watch_time_h watch_time = NULL;
	yin = get_color(yin_color);
	yang = get_color(yang_color);
	elm_config_accel_preference_override_set(EINA_TRUE);
	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, ad->width, ad->height);

	char in_blue[PATH_MAX];
	char line_blue[PATH_MAX];
	ad->res_path = app_get_resource_path();
	char **p = (char **) malloc(7 * sizeof(char *));
	for (int i = 0; i < 7; i++) {
		p[i] = (char*) malloc(sizeof(char) * PATH_MAX);
	}
	if (ad->res_path) {
		snprintf(image_path, (int) PATH_MAX, "%s%s", ad->res_path,
				"loupe_bk.png");
		snprintf(in_blue, (int) PATH_MAX, "%s%s", ad->res_path,
				"indicator.png");

//		snprintf(line_blue, (int) PATH_MAX, "%s%s", res_path, "blue.png");

//		free(res_path);
	}

	Evas *evas = evas_object_evas_get(ad->win);

	ad->img = evas_object_image_add(evas);
	evas_object_image_file_set(ad->img, image_path, NULL);
	evas_object_image_fill_set(ad->img, 0, 0, ad->width, ad->height);
	evas_object_resize(ad->img, ad->width, ad->height);
	evas_object_hide(ad->img);

	// 14 imgs
	ad->imgs = (Evas_Object**)malloc(sizeof(Evas_Object*)*14);
	for(int i=1;i<14;i++)
	{
		ad->imgs[i] = evas_object_image_add(evas);
		snprintf(image_path, (int) PATH_MAX, "%s%d.png", ad->res_path, i);
		evas_object_image_file_set(ad->imgs[i], image_path, NULL);
		evas_object_image_fill_set(ad->imgs[i], -1, -1, ad->width+2, ad->height+2);
		evas_object_resize(ad->imgs[i], ad->width, ad->height);
		evas_object_hide(ad->imgs[i]);
	}

	ad->indicator_blue = evas_object_image_add(evas);
	ad->indicator_red = evas_object_image_add(evas);
	evas_object_image_file_set(ad->indicator_blue, in_blue, NULL);
	evas_object_image_fill_set(ad->indicator_blue, 0, 0, 25, 25);
	evas_object_resize(ad->indicator_blue, 25, 25);
	evas_object_show(ad->indicator_blue);

	double wl = 514 * 360 / 529;
	double hl = 43 * 360 / 529;
	//269.8, 319.7
//	double cx = 332;
//	double cy = 205;
//	evas_object_image_file_set(ad->indicator_red, in_blue, NULL);
//	evas_object_image_fill_set(ad->indicator_red, 0, 0, 25, 25);
//	evas_object_resize(ad->indicator_red, 25, 25);
//	evas_object_move(ad->indicator_red, cx, cy);
//	evas_object_show(ad->indicator_red);
	ad->x = 248.23;
	ad->y = 0;

	/* Create Label */
	ad->label = elm_label_add(ad->win);
	evas_object_resize(ad->label, 90, 30);
	evas_object_move(ad->label, 210, 51);
	evas_object_show(ad->label);

	ad->label_date = elm_label_add(ad->win);
	evas_object_resize(ad->label_date, 105, 30);
	evas_object_move(ad->label_date, 210, 86);
	evas_object_show(ad->label_date);

	ad->watch_time = (watch_time_h*) malloc(sizeof(watch_time_h));
	ret = watch_time_get_current_time(ad->watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);
	ad->hour = -1;
	ad->minute = -1;
	compute_init(ad);
	//ecore_timer_add(0.13,_timer1_cb,ad);
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

static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
	appdata_s *ad = data;
//	if (ad->timer)
//		ecore_timer_freeze(ad->timer);
	evas_object_hide(ad->img);
}

/*
 * @brief This callback function is called each time
 * the application becomes visible to the user
 */
static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
	appdata_s *ad = data;
	ad->resume = 1;
//	if (ad->timer)
//			ecore_timer_thaw(ad->timer);
	evas_object_show(ad->img);
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
				"<align=center><font=Sans:style=Regular font_size=16>%d/%d %s</font></align>",
				 ad->month, ad->day,week_cn[ad->day_of_week-1]);
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
