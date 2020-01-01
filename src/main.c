#include <system_settings.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <app.h>
#include <dlog.h>
#include "buddyui.h"
#include "view.h"
#include "data.h"

#define IMAGE_CONTACT_BG "images/wc_contact_bg.png"

typedef struct appdata {
	Eext_Rotary_Event_Direction rotary_direction;
	Ecore_Animator *anim;
	Eina_Bool anim_end;

	int width;
	int height;
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
} appdata_s;
static appdata_s s_info;
static void _change_title_position(void *data);
static void _set_item_property(void *data, int index);
static Eina_Bool _check_last_item_in_list(void);
static Eina_Bool _simple_circle_path_animation(void *data);
static Eina_Bool _rotary_cb(void *data, Evas_Object *obj,
		Eext_Rotary_Event_Info *rotary_info);
static Eina_Bool _detail_view_pop_cb(void *data, Elm_Object_Item *it);
static Eina_Bool _main_view_pop_cb(void *data, Elm_Object_Item *it);
static void _focus_clicked_cb(void *data, Evas_Object *obj,
		const char *emission, const char *source);
static void _init_view(void *data);

static Eina_Bool event_move(void *data, int type, void *ev) {
	Evas_Object *main_view_layout = data;
	appdata_s *ad = &s_info;
	if (!ad->isDown)
		return ECORE_CALLBACK_DONE;

	Ecore_Event_Mouse_Move *event = ev;
	int cur_x = event->root.x;
	int cur_y = event->root.y;

	ad->prex = cur_x;
	ad->prey = cur_y;

	if (ad->anim == NULL) {
		if (cur_x - ad->downX >= 1)
			ad->rotary_direction = EEXT_ROTARY_DIRECTION_CLOCKWISE;
		else if (cur_x - ad->downX <= -1)
			ad->rotary_direction = EEXT_ROTARY_DIRECTION_COUNTER_CLOCKWISE;
		dlog_print(DLOG_ERROR, LOG_TAG,
					"current position event_move ta index %d", cur_x - ad->downX);
		/*
		 * Register animator for simple circle path animation.
		 */
		s_info.anim = ecore_animator_add(_simple_circle_path_animation,
				main_view_layout);
		ad->downX = cur_x;
		ad->downY = cur_y;
	}
	return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool event_down(void *data, int type, void *ev) {
	//appdata_s * ad = data;
	appdata_s *ad = &s_info;
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
	//appdata_s * ad = data;
	appdata_s *ad = &s_info;
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

/**
 * @brief Hook to take necessary actions before main event loop starts.
 * Initialize UI resources and application's data
 * If this function returns true, the main loop of application starts
 * If this function returns false, the application is terminated
 */
static bool app_create(void *user_data) {
	//appdata_s *ad = user_data;
	appdata_s *ad = &s_info;
	Evas_Object *win = NULL;
	Eina_List *item_list = NULL;
	Evas_Object *main_view_layout = NULL;
	Evas_Object *naviframe = NULL;
	int item_count = 0;
	int i = 0;
	char default_img_path[PATH_MAX] = { 0, };
	char edj_path[PATH_MAX] = { 0, };

	data_get_resource_path(EDJ_FILE, edj_path, sizeof(edj_path));
	data_get_resource_path(IMAGE_CONTACT_BG, default_img_path,
			sizeof(default_img_path));

	/*
	 * Create essential objects for UI application.
	 * Window, Conformant, layout
	 */
	view_create();

	/*
	 * Create additional objects for Buddy UI.
	 * Circle surface, Naviframe
	 */
	view_buddy_create();

	/*
	 * Get window object.
	 */
	win = view_get_window();

	/*
	 * Create main layout to display item list.
	 */
	main_view_layout = view_create_layout(win, edj_path, GRP_MAIN, NULL, NULL);
	if (main_view_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a main view layout");
		view_destroy();
		return false;
	}

	/*
	 * Get naviframe.
	 */
	naviframe = view_get_naviframe();

	/*
	 * Register event callback function to display detail view.
	 */
	view_set_customized_event_callback(main_view_layout, "mouse,clicked",
			"img.focus.bg", _focus_clicked_cb, main_view_layout);
	view_push_item_to_naviframe(naviframe, main_view_layout, _main_view_pop_cb,
	NULL);

	/*
	 * Initialize item property(size, name, image path) in Buddy UI.
	 */
	item_count = data_get_item_total_count();
	for (i = 0; i < item_count; i++) {
		Evas_Object *item_layout = NULL;
		item_info_s *item_info = malloc(sizeof(item_info_s));

		item_info->name = data_get_item_name(i);
		item_info->number = data_get_item_number(i);
		item_info->img_path = data_get_item_image(i);
		item_info->w = 20;
		item_info->h = 20;

		/*
		 * Create item layout to display.
		 */
		item_layout = view_buddy_create_item_layout(main_view_layout,
				item_info->name, edj_path, item_info->img_path,
				default_img_path, NULL);
		evas_object_data_set(item_layout, "__ITEM_INFO__", item_info);

		data_push_item_to_list(&item_list, item_layout);
	}
	data_set_item_list(item_list);

	/*
	 * Initialize to display first view.
	 */
	_init_view(main_view_layout);

	/*
	 * Register rotary event callback function.
	 */
	view_set_rotary_event_callback(main_view_layout, _rotary_cb,
			main_view_layout);
	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, event_move,
			main_view_layout);
	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, event_up,
			main_view_layout);
	ad->handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
			event_down, main_view_layout);

	return true;
}

/**
 * @brief This callback function is called when another application.
 * sends the launch request to the application
 */
static void app_control(app_control_h app_control, void *user_data) {
	/* Handle the launch request. */
}

/**
 * @brief This callback function is called each time.
 * the application is completely obscured by another application
 * and becomes invisible to the user
 */
static void app_pause(void *user_data) {
	/* Take necessary actions when application becomes invisible. */
}

/**
 * @brief This callback function is called each time.
 * the application becomes visible to the user
 */
static void app_resume(void *user_data) {
	/* Take necessary actions when application becomes visible. */
}

/**
 * @brief This callback function is called once after the main loop of the application exits.
 */
static void app_terminate(void *user_data) {
	/*
	 * Release all resources.
	 */
	Evas_Object *item = NULL;
	Eina_List *item_list = NULL;

	/*
	 * Get item list.
	 */
	item_list = data_get_item_list();
	EINA_LIST_FREE(item_list, item)
	{
		item_info_s *item_info = evas_object_data_del(item, "__ITEM_INFO__");
		if (item_info == NULL) {
			/*
			 * If item info is NULL, move next item.
			 */
			continue;
		}

		free(item_info->name);
		free(item_info->number);
		free(item_info->img_path);
		free(item_info);
	}

	view_destroy();
}

/**
 * @brief This function will be called when the language is changed.
 */
static void ui_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/* APP_EVENT_LANGUAGE_CHANGED */
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
			&locale);

	if (locale != NULL) {
		elm_language_set(locale);
		free(locale);
	}
	return;
}

int main(int argc, char *argv[]) {
	int ret;

	//appdata_s *ad = (appdata_s *)malloc(sizeof(appdata_s));
	s_info.rotary_direction = EEXT_ROTARY_DIRECTION_CLOCKWISE;
	s_info.anim = NULL;
	s_info.anim_end = EINA_TRUE;
	s_info.width = 360;
	s_info.height = 360;

	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	/*
	 * If you want to handling more events,
	 * please check the application lifecycle guide
	 */
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, NULL);

	ret = ui_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d",
				ret);
	}

	return ret;
}

/**
 * @brief Change 'Buddy' title position by item position.
 * @param[in] user_data Data needed in this function
 */
static void _change_title_position(void *user_data) {
	Evas_Object *main_view_layout = user_data;
	Evas_Object *item = NULL;
	Eina_List *item_list = NULL;
	item_info_s *item_info = NULL;
	int next_index = 0;

	/*
	 * Get item list.
	 */
	item_list = data_get_item_list();

	/*
	 * Get first item of item list.
	 */
	item = eina_list_nth(item_list, 0);

	/*
	 * Get item info of first item.
	 */
	item_info = evas_object_data_get(item, "__ITEM_INFO__");

	/*
	 * Get next index of first item by rotary direction.
	 */
	next_index = data_get_next_index(s_info.rotary_direction,
			item_info->position);

	/*
	 * Send signal to move title position.
	 */
	if (next_index == 6) {
		elm_object_signal_emit(main_view_layout, "change.title.up",
				"txt.title");
	} else if (next_index > 6) {
		elm_object_signal_emit(main_view_layout, "change.title.hide",
				"txt.title");
	} else {
		elm_object_signal_emit(main_view_layout, "change.title.default",
				"txt.title");
	}
}

/**
 * @brief Set item layout property to display.
 * @param[in] user_data Item layout object
 * @param[in] index The order of position
 */
static void _set_item_property(void *user_data, int index) {
	Evas_Object *item = user_data;
	item_info_s *item_info = NULL;
	int x, y, w, h, opacity;

	item_info = evas_object_data_get(item, "__ITEM_INFO__");
	item_info->position = index;

	if (index < 0 || index > 10) {
		evas_object_move(item, 360, 180);
		evas_object_resize(item, 20, 20);
		evas_object_hide(item);
		item_info->w = 20;
		item_info->h = 20;
		return;
	}

	/*
	 * Get item information by index.
	 */
	data_get_position_info(index, &x, &y, &w, &h, &opacity);

	/*
	 * Change item property.
	 * Color, Size, Position.
	 */
	evas_object_color_set(item, opacity, opacity, opacity, opacity);
	evas_object_resize(item, w, h);
	evas_object_move(item, x - (w / 2), y - (h / 2));
	evas_object_show(item);

	item_info->w = w;
	item_info->h = h;
}

/**
 * @brief Check that the item is last in item list.
 */
static Eina_Bool _check_last_item_in_list(void) {
	Evas_Object *item = NULL;
	Eina_List *item_list = NULL;
	item_info_s *item_info = NULL;
	int count = 0;

	/*
	 * Get item list.
	 */
	item_list = data_get_item_list();

	/*
	 * Last item of list is changed by rotary direction.
	 */
	if (s_info.rotary_direction == EEXT_ROTARY_DIRECTION_CLOCKWISE) {
		count = eina_list_count(item_list);
		item = eina_list_nth(item_list, count - 1);
	} else {
		item = eina_list_nth(item_list, 0);
	}

	item_info = evas_object_data_get(item, "__ITEM_INFO__");
	if (item_info->position == 5) {
		return EINA_TRUE;
	}

	return EINA_FALSE;
}

/**
 * @brief Ecore animator callback function for simple circle path animation of item layout.
 * @param[in] user_data It is not used in this function
 */
static Eina_Bool _simple_circle_path_animation(void *user_data) {
	Evas_Object *main_view_layout = user_data;
	Eina_List *item_list = NULL;
	Eina_List *l = NULL;
	Evas_Object *item_layout = NULL;
	static int frame = 0;

	/*
	 * If the item is last, simple circle path animation will be finished.
	 */
	if (_check_last_item_in_list() == EINA_TRUE) {
		frame = 0;
		s_info.anim = NULL;
		s_info.anim_end = EINA_TRUE;
		return ECORE_CALLBACK_CANCEL;
	}

	if (frame > FRAME_MAX) {
		/*
		 * If current frame is over FRAME_MAX, simple circle path animation will be finished.
		 */
		frame = 0;
		s_info.anim = NULL;
		s_info.anim_end = EINA_TRUE;
		return ECORE_CALLBACK_CANCEL;
	} else if (frame == 0) {
		_change_title_position(main_view_layout);
	}
	s_info.anim_end = EINA_FALSE;

	/*
	 * Get item list.
	 */
	item_list = data_get_item_list();
	EINA_LIST_FOREACH(item_list, l, item_layout)
	{
		item_info_s *item_info = NULL;
		int to_deg, to_x, to_y, to_w, to_h, to_opacity;
		int from_deg, from_x, from_y, from_w, from_h, from_opacity;
		int cur_x, cur_y, cur_w, cur_h;
		int index, move_x, move_y, radius, opacity;
		int x_coord, y_coord;
		float deg_delta = 0.0f, angle = 0.0f;

		item_info = evas_object_data_get(item_layout, "__ITEM_INFO__");
		index = item_info->position;

		if (frame == 0) {
			int font_size = data_get_position_font_size(
					data_get_next_index(s_info.rotary_direction, index));
			view_buddy_change_item_font_size(item_layout, font_size);
		} else if (frame == FRAME_MAX) {
			/*
			 * Set item layout property by index.
			 */
			_set_item_property(item_layout,
					data_get_next_index(s_info.rotary_direction, index));

			/*
			 * If index of item layout is 5, set name to display focus area.
			 */
			if (data_get_next_index(s_info.rotary_direction, index) == 5) {
				view_set_label_text(main_view_layout, "sw.focus.txt",
						item_info->name);
				dlog_print(DLOG_DEBUG, LOG_TAG, "item info  %s, %s",
						item_info->name, item_info->img_path);
			}
			continue;
		}

		data_get_position_info(index, &from_x, &from_y, &from_w, &from_h,
				&from_opacity);
		data_get_position_info(
				data_get_next_index(s_info.rotary_direction, index), &to_x,
				&to_y, &to_w, &to_h, &to_opacity);

		/*
		 * Get the current geometry information of item layout.
		 */
		evas_object_geometry_get(item_layout, &cur_x, &cur_y, &cur_w, &cur_h);

		/*
		 * Get degree of item layout.
		 */
		from_deg = data_get_position_degree(index);
		to_deg = data_get_position_degree(
				data_get_next_index(s_info.rotary_direction, index));
		deg_delta = fabs((float) from_deg - (float) to_deg);

		/*
		 * Get size of item layout.
		 */
		if (index < 0 || index > 10) {
			item_info->w = 20;
			item_info->h = 20;
		} else {
			data_get_item_size(&item_info->w, &item_info->h, from_w, from_h,
					to_w, to_h, deg_delta);
		}
		evas_object_resize(item_layout, item_info->w, item_info->h);

		/*
		 * Get position of item layout.
		 */
		if (index < 0 || index > 10) {
			x_coord = 360;
			y_coord = 180;
		} else {
			/*
			 * 1. Calculate angle.
			 */
			angle = data_get_item_angle(from_deg, to_deg, deg_delta, frame);

			/*
			 * 2. Calculate radius.
			 */
			radius = data_get_item_radius(index, s_info.rotary_direction,
					frame);

			/*
			 * 3. Calculate item position.
			 */
			data_get_simple_circle_path(&move_x, &move_y, radius, angle);
			x_coord = move_x - (cur_w / 2);
			y_coord = move_y - (cur_h / 2);
		}
		evas_object_move(item_layout, x_coord, y_coord);

		/*
		 * Get opacity of item layout.
		 */
		opacity = data_get_item_opacity(item_layout, from_opacity, to_opacity,
				deg_delta);
		evas_object_color_set(item_layout, opacity, opacity, opacity, opacity);

		evas_object_show(item_layout);
	}

	frame++;

	return ECORE_CALLBACK_RENEW;
}

/**
 * @brief Rotary object event callback function signature.
 * @param[in] user_data User data to pass to callback function
 * @param[in] obj The object that will receive rotary event
 * @param[in] rotary_info Rotary event information structure
 */
static Eina_Bool _rotary_cb(void *user_data, Evas_Object *obj,
		Eext_Rotary_Event_Info *rotary_info) {
	Evas_Object *main_view_layout = user_data;

	dlog_print(DLOG_DEBUG, LOG_TAG, "Detent detected, obj[%p], direction[%d]",
			obj, rotary_info->direction);

	if (s_info.anim == NULL) {
		s_info.rotary_direction = rotary_info->direction;

		/*
		 * Register animator for simple circle path animation.
		 */
		s_info.anim = ecore_animator_add(_simple_circle_path_animation,
				main_view_layout);
	}

	return ECORE_CALLBACK_PASS_ON;
}

/**
 * @brief The function is called when detail view is popped at naviframe.
 * @param[in] user_data User data to pass to callback function
 * @param[in] it The item that set the callback on
 */
static Eina_Bool _detail_view_pop_cb(void *user_data, Elm_Object_Item *it) {
	Eina_List *item_list = NULL;

	/*
	 * Get item list.
	 */
	item_list = data_get_item_list();

	/*
	 * Show buddy items.
	 */
	view_buddy_set_item_visible(item_list, EINA_TRUE);

	/*
	 * If detail view is hidden, rotary event of main view will be activated.
	 */
	eext_rotary_object_event_activated_set(user_data, EINA_TRUE);

	return EINA_TRUE;
}

/**
 * @brief The function is called when main view is popped at naviframe.
 * @param[in] user_data User data to pass to callback function
 * @param[in] it The item that set the callback on
 */
static Eina_Bool _main_view_pop_cb(void *user_data, Elm_Object_Item *it) {
	ui_app_exit();

	return EINA_TRUE;
}

/**
 * @brief The function is called when focus text is clicked.
 * @param[in] user_data User data to pass to callback function
 * @param[in] obj The object that will receive signal
 * @param[in] emission The signal name
 * @param[in] source The signal source
 */
static void _focus_clicked_cb(void *user_data, Evas_Object *obj,
		const char *emission, const char *source) {
	Eina_List *item_list = NULL;
	Eina_List *l = NULL;
	Evas_Object *item = NULL;
	Evas_Object *main_view_layout = user_data;
	Evas_Object *detail_view_layout = NULL;
	Evas_Object *naviframe = NULL;
	char edj_path[PATH_MAX] = { 0, };

	if (s_info.anim_end == EINA_FALSE) {
		return;
	}

	data_get_resource_path(EDJ_FILE, edj_path, (int) PATH_MAX);

	/*
	 * Get naviframe.
	 */
	naviframe = view_get_naviframe();
	Evas_Object *entry;
	Evas_Object *parent;

	entry = elm_entry_add(naviframe);
	elm_entry_entry_set(entry, "A short text.");
	view_push_item_to_naviframe(naviframe, entry,
				_detail_view_pop_cb, entry);

	/*
	 * Create detail view of item.
	 */
//	detail_view_layout = view_create_layout(naviframe, edj_path, "detail-view",
//	NULL, NULL);
//	view_push_item_to_naviframe(naviframe, detail_view_layout,
//			_detail_view_pop_cb, main_view_layout);
//
//	/*
//	 * Get item list.
//	 */
//	item_list = data_get_item_list();
//
//	/*
//	 * Hide buddy items.
//	 */
//	view_buddy_set_item_visible(item_list, EINA_FALSE);
//
//	EINA_LIST_FOREACH(item_list, l, item)
//	{
//		item_info_s *item_info = evas_object_data_get(item, "__ITEM_INFO__");
//		if (item_info->position == 5) {
//			view_set_text(detail_view_layout, "txt.name", item_info->name);
//			view_set_text(detail_view_layout, "txt.number", item_info->number);
//
//			/*
//			 * If item has a image, display a image at background.
//			 */
////			if (strcmp(item_info->img_path, "NULL")) {
////				view_set_image(detail_view_layout, "sw.bg",
////						item_info->img_path);
////			}
//		}
//	}

	/*
	 * If detail view is showing, rotary event of main view will be deactivated.
	 */
	eext_rotary_object_event_activated_set(main_view_layout, EINA_FALSE);
}

/**
 * @brief Initialize to display first view.
 * @param[in] user_data data needed in this function
 */
static void _init_view(void *user_data) {
	Eina_List *item_list = NULL;
	Eina_List *l = NULL;
	Evas_Object *item_layout = NULL;
	Evas_Object *main_view_layout = user_data;
	int i = 5;

	/*
	 * Initialize variables.
	 */
	s_info.anim_end = EINA_TRUE;

	/*
	 * Get item list.
	 */
	item_list = data_get_item_list();

	EINA_LIST_FOREACH(item_list, l, item_layout)
	{
		if (i == 5) {
			item_info_s *item_info = evas_object_data_get(item_layout,
					"__ITEM_INFO__");
			view_set_label(main_view_layout, "sw.focus.txt");
			view_set_label_text(main_view_layout, "sw.focus.txt",
					item_info->name);
		}

		/*
		 * Set item layout property by index.
		 */
		_set_item_property(item_layout, i);
		view_buddy_change_item_font_size(item_layout,
				data_get_position_font_size(i));
		i--;
	}
}
