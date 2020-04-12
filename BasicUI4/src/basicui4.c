#include "basicui4.h"
#include <tizen.h>
#include <cairo.h>
#include <math.h>
#include <time.h>
#include <Ecore_Input.h>
#include <sensor.h>
/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)
#define POINT_SIZE 256

typedef struct appdata {
	/* Variables for basic UI contents */
	Evas_Object *win;
	Evas_Object *img;
	Evas_Object *conform;
	//Evas_Object *label;
	/* Variables for watch size information */
	int width;
	int height;
	cairo_t *cairo;
	cairo_surface_t *surface;
	unsigned char *pixels;
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
	Ecore_Animator * anim;
	int * ridus;
	int step;

	cairo_surface_t *image;
	cairo_pattern_t *pattern;
	cairo_matrix_t *matrix;
	int w, h;
} appdata_s;
double* moon;
double* earth;
double* sun;

double interval = 0.1;
char * moon_color = "#FFFFA0";
char * sun_color = "#FFFFA0	";
char * earth_color = "#4169E1";
double * sun_point;
double * moon_point;
#define TEXT_BUF_SIZE 256
double ** earth_locations;

/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)

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

static int computeRidus(int cx, int cy, int x, int y) {
	double d = (cx - x) * (cx - x) + (cy - y) * (cy - y);
	double t = 0.9;
	double r = cx * cx + cy * cy;
	int res = 0;
	if (r >= 3 * d) {
		res = (sqrt(r) - sqrt(d)) / 3;	//res = sqrt(r - d) / 6;
	} else {
		res = (sqrt(r) - sqrt(d)) / 3;
	}
	return res;
	//return 30;
}

static Eina_Bool update_solar_system1(appdata_s *ad) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %d:%d:%d", cx, cy, r);
	/* 绘制背景黑 和 太阳 */

	cairo_move_to(ad->cairo, cx, cy);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//draw sun in center
	cx = ad->cur_position_x;
	cy = ad->cur_position_y;

	cairo_set_source_rgba(ad->cairo, sun[0], sun[1], sun[2], 1);
	r = computeRidus(size / 2, size / 2, cx, cy);
	double ratioX = (double) ad->dx / sqrt(ad->dx * ad->dx + ad->dy * ad->dy);
	double ratioY = (double) ad->dy / sqrt(ad->dx * ad->dx + ad->dy * ad->dy);
	int * ridus = ad->ridus;
	if (ridus[0] == 0) {
		ridus[0] = r;
		cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	} else {
		cx += (r - ridus[0]) * ratioX;
		cy += (r - ridus[0]) * ratioY;
		cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	}

	cairo_set_source_rgba(ad->cairo, 1, 0, 0, 1);
	r = computeRidus(size / 2, size / 2, cx + r + 20, cy + r + 20);
	if (ridus[1] == 0) {
		ridus[1] = r;
		cairo_arc(ad->cairo, cx + r + 20, cy + r + 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	} else {
		cx += (r - ridus[1]) * ratioX;
		cy += (r - ridus[1]) * ratioY;
		cairo_arc(ad->cairo, cx + r + 20, cy + r + 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	}

	cairo_set_source_rgba(ad->cairo, 0, 1, 0, 1);
	r = computeRidus(size / 2, size / 2, cx + r + 20, cy - r - 20);
	if (ridus[2] == 0) {
		ridus[2] = r;
		cairo_arc(ad->cairo, cx + r + 20, cy - r - 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	} else {
		cx += (r - ridus[2]) * ratioX;
		cy += (r - ridus[2]) * ratioY;
		cairo_arc(ad->cairo, cx + r + 20, cy - r - 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	}

	cairo_set_source_rgba(ad->cairo, 0, 0, 1, 1);
	r = computeRidus(size / 2, size / 2, cx - r - 20, cy - r - 20);
	if (ridus[3] == 0) {
		ridus[3] = r;
		cairo_arc(ad->cairo, cx - r - 20, cy - r - 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	} else {
		cx += (r - ridus[3]) * ratioX;
		cy += (r - ridus[3]) * ratioY;
		cairo_arc(ad->cairo, cx - r - 20, cy - r - 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	}

	cairo_set_source_rgba(ad->cairo, 1, 1, 0, 1);
	r = computeRidus(size / 2, size / 2, cx - r - 20, cy + r + 20);
	if (ridus[4] == 0) {
		ridus[4] = r;
		cairo_arc(ad->cairo, cx - r - 20, cy + r + 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	} else {
		cx += (r - ridus[4]) * ratioX;
		cy += (r - ridus[4]) * ratioY;
		cairo_arc(ad->cairo, cx - r - 20, cy + r + 20, r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	}

	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
	return EINA_TRUE;
}

static Eina_Bool update_solar_system2(appdata_s *ad) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %d:%d:%d", cx, cy, r);
	/* 绘制背景黑 和 太阳 */

	cairo_move_to(ad->cairo, cx, cy);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//draw sun in center
	cx = ad->cur_position_x;
	cy = ad->cur_position_y;

	int side = r - 33;
	double inner_x, inner_y;
	for (int i = 0; i < 12; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 30) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, 30, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}

	side = r - 100;
	for (int i = 0; i < 8; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 45) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, 20, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}

	side = r - 145;
	for (int i = 0; i < 6; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 60) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, 15, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}
	side = 5;
	double a = rand() / (RAND_MAX + 0.0);
	double b = rand() / (RAND_MAX + 0.0);
	double c = rand() / (RAND_MAX + 0.0);
	cairo_set_source_rgba(ad->cairo, a, b, c, 1);

	cairo_arc(ad->cairo, cx, cy, side, ANGLE(0), ANGLE(360));
	//	cairo_set_source(ad->cairo, ad->moon_pattern);
	cairo_fill(ad->cairo);

	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
	return EINA_TRUE;
}

static Eina_Bool update_solar_system3(appdata_s *ad) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %d:%d:%d", cx, cy, r);
	/* 绘制背景黑 和 太阳 */

	cairo_move_to(ad->cairo, cx, cy);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//draw sun in center
	cx = ad->cur_position_x;
	cy = ad->cur_position_y;

	int side = 0;
	int cur_r = r / 5;
	int padding = 4;
	double a = rand() / (RAND_MAX + 0.0);
	double b = rand() / (RAND_MAX + 0.0);
	double c = rand() / (RAND_MAX + 0.0);
	cairo_set_source_rgba(ad->cairo, a, b, c, 1);

	cairo_arc(ad->cairo, cx, cy, cur_r, ANGLE(0), ANGLE(360));
	//	cairo_set_source(ad->cairo, ad->moon_pattern);
	cairo_fill(ad->cairo);

	side += cur_r;
	side += padding;
	cur_r = r / 6;
	side += cur_r;
	double inner_x, inner_y;

	for (int i = 0; i < 6; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 60) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, cur_r, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);
	}

	side += cur_r;
	side += padding;
	cur_r = r / 8;
	side += cur_r;
	for (int i = 0; i < 15; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 24) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, cur_r, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}
	dlog_print(DLOG_ERROR, LOG_TAG, "get color %d", side + cur_r);
	side += cur_r;
	side += padding;
	cur_r = (r - side - 3) / 2;
	side += cur_r;
	side = 180 - 3 - 5;
	for (int i = 0; i < 60; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 6) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, 5, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}

	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
	return EINA_TRUE;
}
static Eina_Bool breath_light(appdata_s *ad) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	//dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %d:%d:%d", cx, cy, r);
	/* 绘制背景黑 和 太阳 */

	cairo_move_to(ad->cairo, cx, cy);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	int side = 0, inner_x, inner_y;
	int cur_r = r / 15;
	int padding = 4;
	double a = rand() / (RAND_MAX + 0.0);
	double b = rand() / (RAND_MAX + 0.0);
	double c = rand() / (RAND_MAX + 0.0);
	cairo_set_source_rgba(ad->cairo, a, b, c, 1);

	cairo_arc(ad->cairo, cx, cy, cur_r, ANGLE(0), ANGLE(360));
	//	cairo_set_source(ad->cairo, ad->moon_pattern);
	cairo_fill(ad->cairo);
	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
	return EINA_TRUE;
}
static Eina_Bool update_solar_system4(appdata_s *ad) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %d:%d:%d", cx, cy, r);
	/* 绘制背景黑 和 太阳 */

	cairo_move_to(ad->cairo, cx, cy);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//draw sun in center
	cx = ad->cur_position_x;
	cy = ad->cur_position_y;

	int side = 0, inner_x, inner_y;
	int cur_r = r / 15;
	int padding = 4;
	double a = rand() / (RAND_MAX + 0.0);
	double b = rand() / (RAND_MAX + 0.0);
	double c = rand() / (RAND_MAX + 0.0);
	cairo_set_source_rgba(ad->cairo, a, b, c, 1);

	cairo_arc(ad->cairo, cx, cy, cur_r, ANGLE(0), ANGLE(360));
	//	cairo_set_source(ad->cairo, ad->moon_pattern);
	cairo_fill(ad->cairo);

	side = r * 2 / 3;
	for (int i = 0; i < 12; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 30) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, r / 12, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}

	side = 180 - 3 - 5;
	for (int i = 0; i < 60; i++) {
		double a = rand() / (RAND_MAX + 0.0);
		double b = rand() / (RAND_MAX + 0.0);
		double c = rand() / (RAND_MAX + 0.0);
		cairo_set_source_rgba(ad->cairo, a, b, c, 1);
		double radian = -(i * 6) * (M_PI / 180);
		inner_x = cx - (side * sin(radian));
		inner_y = cx - (side * cos(radian));
		cairo_arc(ad->cairo, inner_x, inner_y, 5, ANGLE(0), ANGLE(360));
		//	cairo_set_source(ad->cairo, ad->moon_pattern);
		cairo_fill(ad->cairo);
	}

	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
	return EINA_TRUE;
}
static Eina_Bool update_solar_system(appdata_s *ad) {
	int size = ad->width;
	int cx = size / 2;
	int cy = size / 2;
	int r = size / 2;
	dlog_print(DLOG_ERROR, LOG_TAG, "current time. err %d:%d:%d", cx, cy, r);
	/* 绘制背景黑 和 太阳 */

	cairo_move_to(ad->cairo, cx, cy);
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 1);
	cairo_arc(ad->cairo, cx, cy, r, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	//draw sun in center
	cx = ad->cur_position_x;
	cy = ad->cur_position_y;

	int side = 0, inner_x, inner_y;
	int cur_r = r / 15;

	int x = cx;
	int y = cy;
	r = computeRidus(size / 2, size / 2, x, y);
	cairo_matrix_init_scale(ad->matrix,0.5,0.5);
//	cairo_pattern_set_matrix(ad->pattern, ad->matrix);

	//cairo_translate(ad->cairo, x-r/2, y-r/2);
//	cairo_scale(ad->cairo,0.2,0.2);
	cairo_set_matrix (ad->cairo,ad->matrix);
	//cairo_scale(ad->cairo,2,2);
	cairo_set_source_surface(ad->cairo, ad->image,0,0);
	//cairo_arc(ad->cairo, cx,cy,cx,0,ANGLE(360));
	//cairo_clip(ad->cairo)

	cairo_paint(ad->cairo);
	cairo_translate(ad->cairo, 180,180);
		cairo_rotate(ad->cairo,ANGLE(30));
	cairo_scale(ad->cairo,2,2);
	//cairo_translate(ad->cairo, -x+r/2, -y+r/2);

//	x = cx - r - 5;
//	y = cy - r - 5;
//	r = computeRidus(size / 2, size / 2, x, y);
//	cairo_matrix_init_scale(ad->matrix, ad->w / r, ad->h / r);
//	cairo_pattern_set_matrix(ad->pattern, ad->matrix);
//
//	cairo_translate(ad->cairo, x - r / 2, y - r / 2);
//	cairo_set_source(ad->cairo, ad->pattern);
//	cairo_rectangle(ad->cairo, 0, 0, 1.2*r, 1.2*r);
//	cairo_fill(ad->cairo);
//	cairo_translate(ad->cairo, -x + r / 2, -y + r / 2);

	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);
	return EINA_TRUE;
}

bool example_sensor_recorder_callback(sensor_type_e type,
		sensor_recorder_data_h data, int remains, sensor_error_e error,
		void *user_data) {
	int step;
	double distance;
	time_t start;
	time_t end;
	dlog_print(DLOG_INFO, "Test", "SENSOR_ERROR %d", error);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, "Test", "SENSOR_ERROR_NONE %d",
				SENSOR_ERROR_NONE);
		return false;
	}
	sensor_recorder_data_get_time(data, &start, &end);
	sensor_recorder_data_get_int(data, SENSOR_RECORDER_DATA_STEPS, &step);
	sensor_recorder_data_get_double(data, SENSOR_RECORDER_DATA_DISTANCE,
			&distance);

	dlog_print(DLOG_INFO, "Test", "c is %d, distance is %f", step, distance);
	return true;
}

/* Define callback */
void example_sensor_callback(sensor_h sensor, sensor_event_s *event,
		void *user_data) {
	/*
	 If a callback is used to listen for different sensor types,
	 it can check the sensor type
	 */
	sensor_type_e type;
	sensor_get_type(sensor, &type);

	if (type == SENSOR_HUMAN_SLEEP_DETECTOR) {
		unsigned long long timestamp = event->timestamp;
		int accuracy = event->accuracy;
		float x = event->values[0];

		dlog_print(DLOG_INFO, "Test1",
				"Accelerometer is not supported on the current device %f", x);
	} else if (type == SENSOR_HRM_LED_GREEN) {
		unsigned long long timestamp = event->timestamp;
		int v = (int) event->values[0];
		dlog_print(DLOG_INFO, "Test1",
				"Accelerometer is not supported on the current device %d", v);
	}
}

/* Register callback */
void get_health() {
	bool supported = false;
	sensor_is_supported(SENSOR_HUMAN_PEDOMETER, &supported);
	if (!supported) {
		dlog_print(DLOG_INFO, "Test1",
				"Accelerometer is not supported on the current device");
		/* Accelerometer is not supported on the current device */
	} else {
		dlog_print(DLOG_INFO, "Test1",
				"Accelerometer is supported on the current device");
	}
//	sensor_recorder_option_h option;
//
//	sensor_recorder_create_option(&option);
//	/* 720 hours (30 days) */
//	sensor_recorder_option_set_int(option, SENSOR_RECORDER_OPTION_RETENTION_PERIOD, 30 * 24);
//
//	sensor_recorder_start(SENSOR_HUMAN_PEDOMETER, option);

	sensor_recorder_query_h query;
	sensor_recorder_create_query(&query);
	/* Start 7 days ago */
	sensor_recorder_query_set_time(query, SENSOR_RECORDER_QUERY_START_TIME,
			(time_t) (time(NULL) - (7 * 24 * 3600)));
	/* End now */
	sensor_recorder_query_set_time(query, SENSOR_RECORDER_QUERY_END_TIME,
			time(NULL));
	/* Aggregate every 24 hours */
	sensor_recorder_query_set_int(query, SENSOR_RECORDER_QUERY_TIME_INTERVAL,
			24 * 60);
	/* Start the aggregation at 7 AM */
	sensor_recorder_query_set_time(query, SENSOR_RECORDER_QUERY_ANCHOR_TIME,
			(time_t) (7 * 3600));

	sensor_recorder_read(SENSOR_HUMAN_PEDOMETER, query,
			example_sensor_recorder_callback, NULL);

	/* Show the window after the base GUI is set up */
	dlog_print(DLOG_INFO, "Test1", "steps is %d, distance is %f get_health", 0,
			0);

//	 supported = false;
//
//	sensor_is_supported(SENSOR_HUMAN_SLEEP_DETECTOR, &supported);
//	if (!supported) {
//	    /* Accelerometer is not supported on the current device */
//	}
//	sensor_h sensor;
//	sensor_get_default_sensor(SENSOR_HUMAN_SLEEP_DETECTOR , &sensor);
//	sensor_listener_h listener;
//	sensor_create_listener(sensor, &listener);
//	sensor_listener_set_event_cb(listener, 1000, example_sensor_callback, NULL);
//	sensor_listener_start(listener);

}
int init_solar_system(appdata_s *ad) {
	sun = get_color(sun_color);
	moon = get_color(moon_color);
	earth = get_color(earth_color);

	/**compute the locations*/
	/**create parttern for sun erath and moon.*/
	int cx, cy, size;
	size = ad->width / 2;
	cx = size;
	cy = size;
	ad->ridus = (int *) malloc(100 * sizeof(int));
	ad->ridus[0] = 0;
	ad->ridus[1] = 0;
	ad->ridus[2] = 0;
	ad->ridus[3] = 0;
	ad->ridus[4] = 0;
	ad->cur_position_x = cx;
	ad->cur_position_y = cy;
	///

	char image_path[PATH_MAX];
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(image_path, (int) PATH_MAX, "%s%s", res_path, "tak.png");
		free(res_path);
	}
	ad->matrix = (cairo_matrix_t *) malloc(sizeof(cairo_matrix_t));
	ad->image = cairo_image_surface_create_from_png(image_path);
	ad->w = cairo_image_surface_get_width(ad->image);
	ad->h = cairo_image_surface_get_height(ad->image);

//	ad->pattern = cairo_pattern_create_for_surface(ad->image);
//	cairo_pattern_set_extend(ad->pattern, CAIRO_EXTEND_NONE);

	update_solar_system(ad);
	return 0;
}

static void win_delete_request_cb(void *data, Evas_Object *obj,
		void *event_info) {
	ui_app_exit();
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
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
	update_solar_system(ad);
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

static Eina_Bool scroll_animation_callback(void *data, double pos) {
	appdata_s * ad = data;
	ad->cur_position_x = ad->prex + ad->avg_speedX / abs(ad->avg_speedX) * pos;
	ad->cur_position_y = ad->prey + ad->avg_speedY / abs(ad->avg_speedY) * pos;
	update_solar_system(ad);
	dlog_print(DLOG_ERROR, LOG_TAG,
			"current position scroll_animation_callback %f", pos);
	return EINA_TRUE;
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

	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->img = evas_object_image_filled_add(evas_object_evas_get(ad->conform));
	evas_object_image_size_set(ad->img, ad->width, ad->height);
	evas_object_resize(ad->img, ad->width, ad->height);
	evas_object_image_content_hint_set(ad->img,
			EVAS_IMAGE_CONTENT_HINT_DYNAMIC);
	evas_object_image_colorspace_set(ad->img, EVAS_COLORSPACE_ARGB8888);
	evas_object_show(ad->img);

	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, event_move,
			ad);
	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, event_up,
			ad);
	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
			event_down, ad);

//	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_IN, event_in, ad);
//	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_OUT, event_out,
//			ad);

	ad->pixels = (unsigned char*) evas_object_image_data_get(ad->img, 1);
	ad->surface = cairo_image_surface_create_for_data(ad->pixels,
			CAIRO_FORMAT_ARGB32, ad->width, ad->height, ad->width * 4);
	ad->cairo = cairo_create(ad->surface);
	init_solar_system(ad);

	elm_object_content_set(ad->conform, ad->img);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool app_create(void *data) {
	/* Hook to take necessary actions before main event loop starts
	 Initialize UI resources and application's data
	 If this function returns true, the main loop of application starts
	 If this function returns false, the application is terminated */
	appdata_s *ad = data;
	ad->width = 360;
	ad->height = 360;
	create_base_gui(ad);

	return true;
}

static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
	appdata_s * ad = data;
//	if (ad->anim) {
//		ecore_animator_freeze(ad->anim);
//	}
}

static void app_resume(void *data) {
	appdata_s * ad = data;
//	if (ad->anim) {
//		ecore_animator_thaw(ad->anim);
//	} else {
//		ad->anim = ecore_animator_add(update_solar_system, ad);
//	}

}

static void app_terminate(void *data) {
	/* Release all resources. */
	/* Release all resources. */
	appdata_s *ad = data;
	/* Destroy cairo surface and context */
	cairo_surface_destroy(ad->surface);
	cairo_destroy(ad->cairo);
	ecore_event_handler_del(ad->handler);
//	ecore_animator_del(ad->anim);
	cairo_pattern_destroy(ad->pattern);
	cairo_surface_destroy(ad->image);
	free(ad->ridus);
	free(ad);

}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
			&locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char *argv[]) {
	appdata_s ad = { 0, };
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
