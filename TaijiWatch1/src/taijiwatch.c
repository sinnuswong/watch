/* 里面只有圆,是圆在围绕着旋转，秒针，分针、时针都在旋转*/
#include <tizen.h>
#include "taijiwatch.h"
#include <cairo.h>
#include <math.h>
#define TEXT_BUF_SIZE 256

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

} appdata_s;

#define TEXT_BUF_SIZE 256

/*
 * @brief Update a watch face screen
 * @param[in] ad application's data structure
 * @param[in] watch_time current time information for update
 * @param[in] ambient ambient mode status
 */

/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)

int init_taiji(cairo_surface_t* surface, cairo_t* cr, int size) {
	int CX = size / 2;
	int CY = size / 2;
	int R = size / 2;

	/* 绘制太极边框 */
//	cairo_set_line_width(cr, 2.0);
//	cairo_set_source_rgba(cr, 1.0, 0.8, 0.8, 1);
//	cairo_arc(cr, CX, CY, R, ANGLE(0), ANGLE(360));
//	cairo_stroke(cr);
//	int side = ad->width/2*1;
//	double radian = -second *(M_PI/180);
//	double x = ad->width/2 - (side * sin(radian*6));
//	double y = ad->height/2 - (side * cos(radian*6));
	/* 绘制阴阳圆 */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY, R, ANGLE(90), ANGLE(270));
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, CX, CY, R, ANGLE(-90), ANGLE(90));
	cairo_fill(cr);

	/* 绘制阴阳线 */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY - R / 2, R / 2, ANGLE(-90), ANGLE(90));
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, CX, CY + R / 2, R / 2, ANGLE(90), ANGLE(270));
	cairo_fill(cr);

	/* 绘制太极眼 */
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, CX, CY - R / 2, R / 10, ANGLE(0), ANGLE(360));
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY + R / 2, R / 10, ANGLE(0), ANGLE(360));
	cairo_fill(cr);

	return 0;
}

static void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient) {
	/* Variables for time */
	//char watch_text[TEXT_BUF_SIZE];
	int hour24, minute, second;
	char watch_text[TEXT_BUF_SIZE];
	if (watch_time == NULL)
		return;

	/* Get current time */
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);
//	snprintf(watch_text, TEXT_BUF_SIZE, "%02d:%02d:%02d", hour24, minute,
//			second);
//	char s[3];
//	s[0] = watch_text[0];
//	s[1] = watch_text[1];
//	s[2] = '\0';
//	char t[2];
//	int a = (watch_text[0]-'0') * 10 + (watch_text[1]-'0');
//	int b = (watch_text[3]-'0') * 10 + (watch_text[4]-'0');
//	int c = (watch_text[6]-'0') * 10 + (watch_text[7]-'0');
//	int m = (s[0]-'0')*10+(s[1]-'0');
//	dlog_print(DLOG_INFO, LOG_TAG, "get time ************ %c %c ,%d",watch_text[0],watch_text[1],m);
//	dlog_print(DLOG_INFO, LOG_TAG, "get time %c %c %c %c %c %c %c %c", watch_text[0],watch_text[1],
//			watch_text[2],watch_text[3],watch_text[4],watch_text[5],watch_text[6],watch_text[7]);
//	dlog_print(DLOG_INFO, LOG_TAG, "get time %s", watch_text);
//	dlog_print(DLOG_INFO, LOG_TAG, "get time %d,%d,%d", a,b,c);
//	dlog_print(DLOG_INFO, LOG_TAG, "get time %d,%d,%d", hour24,minute,second);

	/* Set Background color as light blue */
	//cairo_set_source_rgba(ad->cairo, 0.9, 0.9, 0.9, 0.9);
	init_taiji(ad->surface, ad->cairo, ad->width);
//	cairo_save(ad->cairo);
	double width, height, x, y;
	width = ad->width / 2;
	height = ad->height / 2;
//	cairo_translate (ad->cairo, x, y);
//	cairo_rotate(ad->cairo,ANGLE(6));
//	cairo_translate (ad->cairo, -x, -y);
//	cairo_save(ad->cairo);
//	cairo_restore(ad->cairo);
//	cairo_paint(ad->cairo);

	/* Variables for display time */
	double side, radian;
	double inner_x = 0, inner_y = 0;
	cairo_line_cap_t line_cap_style;
	cairo_line_join_t line_join_style;

	/*
	 * How to draw second hand
	 * 1. Set second hand's line style, width, color
	 * 2. Set second hand's line start position
	 * 3. Compute second hand's line end position
	 * 4. Draw second hand
	 * 5. Stroke second hand as line configuration from step.1
	 */

	/* Set second hand line style, width */
	line_cap_style = CAIRO_LINE_CAP_ROUND;
	line_join_style = CAIRO_LINE_JOIN_BEVEL;
	cairo_set_line_width(ad->cairo, 2);
	cairo_set_line_join(ad->cairo, line_join_style);
	cairo_set_line_cap(ad->cairo, line_cap_style);

	if (ambient) {

	} else {

		/* Set second hand line color */
		cairo_set_source_rgba(ad->cairo, 0.92, 0.98, 0.98, 1);

		/*
		 * Set line start position
		 * The start position should be at the center of the screen
		 */

		//cairo_move_to(ad->cairo, width, height-width/2);
		/*
		 * Compute the line end position
		 * Using side and radian
		 */
		side = width / 2;
		radian = -second * (M_PI / 180);

		//inner_x = width - (side / 5 * sin(radian * 6));
		//inner_y = height - width / 2 - (side / 5 * cos(radian * 6));

		inner_x = width - ((side / 4+2) * sin(radian * 6));
		inner_y = height - width / 2 - ((side /4 +2) * cos(radian * 6));

		cairo_arc(ad->cairo, inner_x, inner_y, 3, ANGLE(0), ANGLE(360));
		cairo_fill(ad->cairo);

//		cairo_move_to(ad->cairo, inner_x, inner_y);
//		x = width - (side * sin(radian * 6));
//		y = height - width / 2 - (side * cos(radian * 6));
//
//		/* Draw second hand */
//		cairo_line_to(ad->cairo, x, y);
//		cairo_close_path(ad->cairo);
//
//		/*
//		 * Stroke second hand
//		 * With it's line configuration */
//		cairo_stroke(ad->cairo);
	}

	/*
	 * How to draw minute hand
	 * 1. Set minute hand's line style, width, color
	 * 2. Set minute hand's line start position
	 * 3. Compute minute hand's line end position
	 * 4. Draw minute hand
	 * 5. Stroke minute hand with as line configuration from step.1
	 */

	/* Set minute hand line style, width */
	line_cap_style = CAIRO_LINE_CAP_BUTT;
	line_join_style = CAIRO_LINE_JOIN_ROUND;
	cairo_set_line_width(ad->cairo, 4);
	cairo_set_line_join(ad->cairo, line_join_style);
	cairo_set_line_cap(ad->cairo, line_cap_style);

	/* Set minute hand line color */
	cairo_set_source_rgba(ad->cairo, 0, 0, 0, 0.9);

	/*
	 * Set line start position
	 * The start position should be at the center of the screen
	 */

	//cairo_move_to(ad->cairo, width, height+width/2);
	/*
	 * Compute the line end position
	 * Using side and radian
	 */
	side = width / 2;
	radian = -minute * (M_PI / 180);
//	inner_x = width - (side / 5 * sin(radian * 6));
//	inner_y = height + width / 2 - (side / 5 * cos(radian * 6));
//	cairo_move_to(ad->cairo, inner_x, inner_y);
//	x = width - (side * sin(radian * 6));
//	y = height + width / 2 - (side * cos(radian * 6));
//
//	/* Draw minute hand */
//	cairo_line_to(ad->cairo, x, y);
//	cairo_close_path(ad->cairo);
//
//	/*
//	 * Stroke minute hand
//	 * With it's line configuration */
//	cairo_stroke(ad->cairo);

	inner_x = width - (side / 4 * sin(radian * 6));
	inner_y = height + width / 2 - (side / 4 * cos(radian * 6));

	cairo_arc(ad->cairo, inner_x, inner_y, 3, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

	/*
	 * How to draw hour hand
	 * 1. Set hour hand's line style, width, color
	 * 2. Set hour hand's line start position
	 * 3. Compute hour hand's line end position
	 * 4. Draw hour hand
	 * 5. Stroke hour hand with as line configuration from step.1
	 */

	/* Set hour hand line style, width */
	line_cap_style = CAIRO_LINE_CAP_SQUARE;
	line_join_style = CAIRO_LINE_JOIN_ROUND;
	cairo_set_line_width(ad->cairo, 6);
	cairo_set_line_join(ad->cairo, line_join_style);
	cairo_set_line_cap(ad->cairo, line_cap_style);

	/* Set hour hand line color */
	cairo_set_source_rgba(ad->cairo, 1, 1, 1, 1.0);

	/*
	 * Set line start position
	 * The start position should be at the center of the screen
	 */

	//cairo_move_to(ad->cairo, width, height+width/2);
	/*
	 * Compute the line end position
	 * Using side and radian
	 */
	side = width / 2;
	dlog_print(DLOG_ERROR, LOG_TAG, "current side is %f.", side);
	radian = -(double)(hour24+(double)minute/60) * (M_PI / 180);

	inner_x = width - ((side / 6 - 2) * sin(radian * 30));
	inner_y = height + width / 2 - ((side / 6 - 2) * cos(radian * 30));

	cairo_arc(ad->cairo, inner_x, inner_y, 3, ANGLE(0), ANGLE(360));
	cairo_fill(ad->cairo);

//	inner_x = width - (side / 5 * sin(radian * 30));
//	inner_y = height + width / 2 - (side / 5 * cos(radian * 30));
//	cairo_move_to(ad->cairo, inner_x, inner_y);
//	x = width - (side * sin(radian * 30));
//	y = height + width / 2 - (side * cos(radian * 30));
//
//	/* Draw hour hand */
//	cairo_line_to(ad->cairo, x, y);
//	cairo_close_path(ad->cairo);

	/*
	 * Stroke hour hand
	 * With it's line configuration */
	cairo_stroke(ad->cairo);

	/* Render stacked cairo APIs on cairo context's surface */
	cairo_surface_flush(ad->surface);

	/* Display this cairo watch on screen */
	evas_object_image_data_update_add(ad->img, 0, 0, ad->width, ad->height);
	evas_object_show(ad->img);

	/* Set label text */
//	if (!ambient) {
//		snprintf(watch_text, TEXT_BUF_SIZE, "<align=center>Watch Cairo<br/>%02d:%02d:%02d</align>",
//			hour24, minute, second);
//	} else {
//		snprintf(watch_text, TEXT_BUF_SIZE, "<align=center>Watch Cairo<br/>%02d:%02d</align>",
//			hour24, minute);
//	}
	/* Set label */
	//elm_object_text_set(ad->label, "");
}

/*
 * Create and display the selected watch face screen
 ( User can change watch face in Wearable's Settings
 Select watch face option in 'Settings' - 'Display' - 'Watch face' menu )
 * In this function,
 * First, get the watch application's elm_win
 * Second, create an evas object image as a destination surface
 * Then, create cairo context and surface using evas object image's data
 * Third, Create a label for display the given time as a text type
 * Last, call update_watch() function to draw and display the first screen
 */
static void create_base_gui(appdata_s *ad) {
	int ret;
	watch_time_h watch_time = NULL;

	/*
	 * Window
	 * Get the watch application's elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	ret = watch_app_get_elm_win(&ad->win);
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

	/* Create Label */
//	ad->label = elm_label_add(ad->win);
//	evas_object_resize(ad->label, ad->width, ad->height/3);
//	evas_object_move(ad->label, 0, ad->height/3);
//	evas_object_show(ad->label);
	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",
				ret);

	/* Display first screen of watch */
	update_watch(ad, watch_time, 0);

	/* Destroy variable */
	watch_time_delete(watch_time);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

/*
 * @brief Hook to take necessary actions before main event loop starts
 * @param[in] width watch application's given elm_win's width
 * @param[in] height watch application's given elm_win's height
 * @param[in] data application's data structure
 * Initialize UI resources and application's data
 * If this function returns true, the main loop of application starts
 * If this function returns false, the application is terminated
 */
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

/*
 * @brief This callback function is called each time
 * the application is completely obscured by another application
 * and becomes invisible to the user
 */
static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
}

/*
 * @brief This callback function is called each time
 * the application becomes visible to the user
 */
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
