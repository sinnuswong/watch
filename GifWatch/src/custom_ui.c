//1. crop and locate the gif,
//2. locate the time and date. size and color
//2. frame
//4.  pro add multi gif

#include "custom_ui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "gifwatch.h"
#include <metadata_extractor.h>
#include <util.h>

void fix_ratio(void *data, int *w, int *h);
void scale(void *data, float scale);
void perform_post_event(void *data);
void change_tmp_data(void *data, int w, int h, int x, int y);
void show_all_custom_buttons(void *data);
void hide_all_custom_buttons(void *data);
static Eina_Bool event_move(void *data, int type, void *ev);
static Eina_Bool event_down(void *data, int type, void *ev);

static void image_request_callback(app_control_h request, app_control_h reply,
		app_control_result_e result, void *user_data);
static void button_click_cb(void *data, Evas_Object *obj, void *event_info);

//位置是左上角m
// tag = 0 not thing changed. immediately return
// tag = 1 file path changed, change size and location
// tag = 2 only location changed, cause by motion move event.
// tag = 3 only size changed by scale button.
void show_image(void* data, char *image_path, int width, int height, int x,
		int y, int tag) {
	appdata_s *ad = data;
	Evas_Object *image = ad->img;
	if (tag == 0) { //image path not changed, only  move event.
		return;
	} else if (tag == 1) { //image path changed
		evas_object_image_file_set(image, image_path, NULL);
		evas_object_image_fill_set(image, 0, 0, width, height);
		evas_object_resize(image, width, height);
		evas_object_show(image);
		evas_object_move(image, x, y);
		change_tmp_data(data,width, height, x, y);
	} else if (tag == 2) {
		evas_object_move(image, x, y);
		change_tmp_data(data,width, height, x, y);
	} else if (tag == 3) {
		evas_object_image_file_set(image, image_path, NULL); //?
		evas_object_image_fill_set(image, 0, 0, width, height);
		evas_object_resize(image, width, height);
		evas_object_show(image);

		evas_object_move(image, x, y);
		change_tmp_data(data,width, height, x, y);
	} else
		return;

//	dlog_print(DLOG_ERROR, LOG_TAG, "show image size is %d %d", w, h);
}

//also should add label date info
void change_tmp_data(void *data, int w, int h, int x, int y) {
	appdata_s *ad = data;
	ad->tmp_width = w;
	ad->tmp_height = h;
	ad->tmp_location_x = x;
	ad->tmp_location_y = y;
}
//execute after an event ends.
void perform_post_event(void *data) {
	appdata_s *ad = data;
	//must ensure status changed.
	ad->img_width = ad->tmp_width;
	ad->img_height = ad->tmp_height;
	ad->img_location_x = ad->tmp_location_x;
	ad->img_location_y = ad->tmp_location_y;
}

void init_elm_image(Evas_Object* image, char* image_path, int x, int y, int w,
		int h) {
	elm_image_no_scale_set(image, EINA_TRUE);
	elm_image_resizable_set(image, EINA_TRUE, EINA_TRUE);
	/* Tell the image to keep original aspect ratio */
	elm_image_aspect_fixed_set(image, EINA_TRUE);
	/* Then let the image fill the entire object */
	elm_image_fill_outside_set(image, EINA_TRUE);
	elm_image_smooth_set(image, EINA_TRUE);
	elm_image_preload_disabled_set(image, EINA_TRUE);
	elm_image_file_set(image, image_path, NULL);
//	int w, h;
//	elm_image_object_size_get(image, &w, &h);
//	evas_object_image_load_size_get(ad->img, &w, &h);
	evas_object_resize(image, w, h);
//	dlog_print(DLOG_ERROR, LOG_TAG, "image load size is %d %d", w, h);
	evas_object_move(image, x, y);
	evas_object_show(image);
}
//invoke in gifwatch.
void create_custom_button(void *data) {
	//image increase and decrease button.
	appdata_s *ad = data;
	char image_path[PATH_MAX];
	if (ad->res_path) {
		snprintf(image_path, (int) PATH_MAX, "%s%s", ad->res_path,
				"decrease.png");
	}
	ad->decrease_img = elm_image_add(ad->win);
	init_elm_image(ad->decrease_img, image_path, image_decrease_locate_x,
			image_decrease_locate_y, ic_width, ic_width);

	if (ad->res_path) {
		snprintf(image_path, (int) PATH_MAX, "%s%s", ad->res_path,
				"increase.png");
	}
	ad->increase_img = elm_image_add(ad->win);
	init_elm_image(ad->increase_img, image_path, image_increase_locate_x,
			image_increase_locate_y, ic_width, ic_width);
	evas_object_smart_callback_add(ad->decrease_img, "clicked", button_click_cb,
			data);
	evas_object_smart_callback_add(ad->increase_img, "clicked", button_click_cb,
			data);

	// bigger and smaller
	ad->bigger_image = elm_image_add(ad->win);
	init_elm_image(ad->bigger_image, image_path, image_increase_locate_x,
			image_increase_locate_y - 60, ic_width, ic_width);

	ad->smaller_image = elm_image_add(ad->win);
	init_elm_image(ad->smaller_image, image_path, image_decrease_locate_x,
			image_decrease_locate_y - 60, ic_width, ic_width);
	evas_object_smart_callback_add(ad->bigger_image, "clicked", button_click_cb,
			data);
	evas_object_smart_callback_add(ad->smaller_image, "clicked",
			button_click_cb, data);

	ad->label_fps = elm_label_add(ad->win);
	evas_object_resize(ad->label_fps, label_fps_width, label_fps_height);
	evas_object_move(ad->label_fps, label_fps_location_x, label_fps_location_y);
	evas_object_show(ad->label_fps);

	snprintf(str_buffer, (int) PATH_MAX, "<align=center>%d</align>",
			ad->fps);
	elm_object_text_set(ad->label_fps, str_buffer);

	ad->bt_pick = elm_button_add(ad->win);
	elm_object_text_set(ad->bt_pick, "pick");
	evas_object_resize(ad->bt_pick, bt_pick_width, bt_pick_height);
	evas_object_move(ad->bt_pick, bt_pick_location_x, bt_pick_location_y);
	evas_object_show(ad->bt_pick);
	evas_object_smart_callback_add(ad->bt_pick, "clicked", button_click_cb,
			data);
	hide_all_custom_buttons(data);
}
void show_all_custom_buttons(void *data) {
	appdata_s *ad = data;
	evas_object_show(ad->label_fps);
	evas_object_show(ad->increase_img);
	evas_object_show(ad->decrease_img);
	evas_object_show(ad->bt_pick);
	evas_object_show(ad->bigger_image);
	evas_object_show(ad->smaller_image);
}
void hide_all_custom_buttons(void *data) {
	appdata_s *ad = data;
	evas_object_hide(ad->label_fps);
	evas_object_hide(ad->increase_img);
	evas_object_hide(ad->decrease_img);
	evas_object_hide(ad->bt_pick);
	evas_object_hide(ad->bigger_image);
	evas_object_hide(ad->smaller_image);
}
void start_custom(void *data) {
	appdata_s *ad = data;
	show_all_custom_buttons(data);
	ad->current_win = 1;
	ad->status_changed = 0;
	ad->scale = 1.0;
	change_tmp_data(ad, ad->img_width, ad->img_height, ad->img_location_x,
			ad->img_location_y);
	ad->handler_down = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
			event_down, ad);
	ad->handler_move = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,
			event_move, ad);
	// register click or move events.
}
void end_custom(void *data) {
	perform_post_event(data);
	appdata_s *ad = data;
	hide_all_custom_buttons(data);
	ad->key_back_tag = 1;

	// window change to 0.
	ad->current_win = 0;

	ecore_event_handler_del(ad->handler_down);
	ecore_event_handler_del(ad->handler_move);
//	addImage(ad, *(ad->value), ad->img_width, ad->img_height,
//				ad->img_location_x, ad->img_location_y);
}
//only works in custom window
static Eina_Bool event_move(void *data, int type, void *ev) {
	appdata_s *ad = data;

	if (ad->current_win == 1) { // in custom window.
		if (!ad->isDown)
			return ECORE_CALLBACK_DONE;
		if (ad->current_win == 0) { // in watch window.
			return ECORE_CALLBACK_PASS_ON;
		}

		Ecore_Event_Mouse_Move *event = ev;
		int cx, cy;
		int dx = event->root.x - ad->move_prev_x;
		int dy = event->root.y - ad->move_prev_y;

		dlog_print(DLOG_ERROR, LOG_TAG,
				"current position event_move ta index %d %d", dx, dy);
		ad->move_prev_x = event->root.x;
		ad->move_prev_y = event->root.y;

		//dlog_print(DLOG_ERROR, LOG_TAG, "current position %d %d",ev->cur.output.x,ev->cur.output.y);
		ad->move_dx += dx;
		ad->move_dy += dy;

		// check is in which widget
		if (ad->moved_which == 1) {
			cx = ad->img_location_x + (int) ((float) (ad->move_dx));
			cy = ad->img_location_y + (int) ((float) (ad->move_dy));
			//		dlog_print(DLOG_ERROR, LOG_TAG, "current position  cx,cy %d %d", cx,
			//				cy);
			show_image(ad, *(ad->value), ad->img_width,
					ad->img_height, cx, cy, 2);
		} else if (ad->moved_which == 2) {
			// label date and time

		}
	}

	return ECORE_CALLBACK_PASS_ON;
}

//only works at custom window
static Eina_Bool event_down(void *data, int type, void *ev) {
	appdata_s * ad = data;
	if (ad->current_win == 1) { // in custom window.
		Ecore_Event_Mouse_Button *event = ev;
		ad->isDown = 1;

		ad->move_dx = 0;
		ad->move_dy = 0;

		ad->down_x = event->root.x;
		ad->down_y = event->root.y;
		ad->move_prev_x = event->root.x;
		ad->move_prev_y = event->root.y;
		//compute moved which
		//check down_x,down_y with img_location_x,
		ad->moved_which = which_object(ad, ad->down_x, ad->down_y);
		dlog_print(DLOG_ERROR, LOG_TAG,
				"current position event_down ta inde %d %d", ad->move_prev_x,
				ad->move_prev_y);
		return ECORE_CALLBACK_PASS_ON;
	}

	return ECORE_CALLBACK_PASS_ON;
}
static void button_click_cb(void *data, Evas_Object *obj, void *event_info) {

	appdata_s *ad = data;
	ad->status_changed = 1;
	if (obj == ad->increase_img) {
		ad->fps = ad->fps + 1;
		if (ad->fps > 30) {
			ad->fps = 30;
		}
		ad->interval = 1.0 / (ad->fps);
		dlog_print(DLOG_INFO, LOG_TAG, "increase_click_cb fps %d interal %f\n",
				ad->fps, ad->interval);
		snprintf(str_buffer, (int) PATH_MAX, "<align=center>%d</align>",
				ad->fps);
		elm_object_text_set(ad->label_fps, str_buffer);
	} else if (obj == ad->decrease_img) {
		ad->fps = ad->fps - 1;
		if (ad->fps < 5)
			ad->fps = 5;
		ad->interval = 1.0 / (ad->fps);
		dlog_print(DLOG_INFO, LOG_TAG, "increase_click_cb fps %d interal %f\n",
				ad->fps, ad->interval);
		snprintf(str_buffer, (int) PATH_MAX, "<align=center>%d</align>",
				ad->fps);
		elm_object_text_set(ad->label_fps, str_buffer);
	} else if (obj == ad->bt_pick) {
		int ret = 0;
		char* mime_type = "image/*";
		app_control_h service = NULL;
		app_control_create(&service);

		if (service == NULL) {
			dlog_print(DLOG_INFO, LOG_TAG,
					"Failed to create app control handler\n");
		}

		app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
		app_control_set_mime(service, mime_type);
		ret = app_control_send_launch_request(service, image_request_callback,
				data);

		app_control_destroy(service);

		if (ret == APP_CONTROL_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "Succeeded to request!\n");
		} else {
			dlog_print(DLOG_INFO, LOG_TAG, "Failed to request! %d \n", ret);
		}
	} else if (obj == ad->bigger_image) {
		ad->scale += 0.1;
		scale(ad, ad->scale);
	} else if (obj == ad->smaller_image) {
		ad->scale -= 0.1;
		scale(ad, ad->scale);
	}

}
static void image_request_callback(app_control_h request, app_control_h reply,
		app_control_result_e result, void *user_data) {
	appdata_s *ad = user_data;
	int len;
	int ret;

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		ret = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED,
				&(ad->value), &len);

		if (ret == APP_CONTROL_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "the path is %s ", *(ad->value));
			ad->status_changed = 1;
//			addImage(ad, *(ad->value), 360, 360, 180, 180);
			//读出来宽度和高度 w,h
			int w, h;
			get_size(*(ad->value), &w, &h);
			fix_ratio(ad, &w, &h);
			dlog_print(DLOG_INFO, LOG_TAG,
					"the image fixed width height is: %d %d\n ", w, h);
			show_image(ad, *(ad->value), w, h,
					ad->width / 2 - w / 2, ad->height / 2 - h / 2, 1);
		} else {
			dlog_print(DLOG_INFO, LOG_TAG, "Failed");
		}
	}
}

void fix_ratio(void *data, int *w, int *h) {
	appdata_s *ad = data;
	if (ad->width * (*h) >= ad->height * (*h)) {
		//w is small,
		float ratio = ((float) (ad->height)) / (*h);
		*h = ad->height;
		*w = (int) (ratio * (*w));
	} else {
		//h is small
		float ratio = ((float) (ad->width)) / (*w);
		*w = ad->width;
		*h = (int) (ratio * (*h));
	}
}
//for scale button

void scale(void *data, float scale) {
	if (scale == 1.0)
		return;
	else if (scale < 0.25)
		return;
	else if (scale > 4)
		return;
	appdata_s *ad = data;
	int w = (int) ((float) (ad->img_width) * scale);
	int h = (int) ((float) (ad->img_height) * scale);
	dlog_print(DLOG_INFO, LOG_TAG,
			"the image scale width height is: %d %d %d %d %f\n ", w, h,
			ad->img_width, ad->img_height, scale);

	//不能立即修改原始位置和宽度，不然点击的时候变化是指数倍。

	int center_x = ad->img_location_x + ad->img_width / 2;
	int center_y = ad->img_location_y + ad->img_height / 2;
	int x = center_x - w / 2;
	int y = center_y - h / 2;
	show_image(ad, *(ad->value), w, h, x, y, 3);
}
void get_size(char * path, int* w, int *h) {
	int n;
	unsigned char *data = stbi_load(path, w, h, &n, 0);
	stbi_image_free(data);
	dlog_print(DLOG_INFO, LOG_TAG, "the image width height is: %d %d\n ", *w,
			*h);
}

