/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Elementary.h>
#include <efl_extension.h>
#include <app.h>
#include <dlog.h>
#include "buddyui.h"
#include "view.h"
#include "data.h"

#define LABEL_STYLE_START "<font=Tizen:style=Regular><font_size=36><align=center><color=#FAFAFA><wrap=mixed>"
#define LABEL_STYLE_END "</wrap></color></align></font_size></font>"

static struct view_info {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Eext_Circle_Surface *circle_surface;
	Eina_Bool anim_end;
} s_info = {
	.win = NULL,
	.conform = NULL,
	.layout = NULL,
	.nf = NULL,
	.circle_surface = NULL,
	.anim_end = EINA_FALSE,
};

static void _dummy_callback_cb(void *data, Evas_Object *obj, void *event_info);

/**
 * @brief Get window.
 */
Evas_Object *view_get_window(void)
{
	return s_info.win;
}

/**
 * @brief Get conformant.
 */
Evas_Object *view_get_conformant(void)
{
	return s_info.conform;
}

/**
 * @brief Get main view layout.
 */
Evas_Object *view_buddy_get_layout(void){
	return s_info.layout;
}

/**
 * @brief Get naviframe.
 */
Evas_Object *view_get_naviframe(void)
{
	return s_info.nf;
}

/**
 * @brief Creates essential objects: window, conformant and layout.
 */
Eina_Bool view_create(void)
{
	/* Create the window. */
	s_info.win = view_create_win(PACKAGE);
	if (s_info.win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");
		return EINA_FALSE;
	}

	/* Create the conformant. */
	s_info.conform = view_create_conformant_without_indicator(s_info.win);
	if (s_info.conform == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a conformant");
		return EINA_FALSE;
	}

	/* Show the window after main view is set up. */
	evas_object_show(s_info.win);
	return EINA_TRUE;
}

/**
 * @brief Creates a basic window named package.
 * @param[in] pkg_name Name of the window
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate the window.
	 */
	win = elm_win_util_standard_add(pkg_name, pkg_name);
	elm_win_conformant_set(win, EINA_TRUE);
	elm_win_autodel_set(win, EINA_TRUE);

	/* Rotation setting. */
	if (elm_win_wm_rotation_supported_get(win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(win, "delete,request", _dummy_callback_cb, NULL);

	return win;
}

/**
 * @brief Creates a conformant without indicator for wearable app.
 * @param[in] win The object to which you want to set this conformant
 * Conformant is mandatory for base GUI to have proper size
 */
Evas_Object *view_create_conformant_without_indicator(Evas_Object *win)
{
	/*
	 * Conformant
	 * Create and initialize elm_conformant.
	 * elm_conformant is mandatory for base GUI to have proper size
	 * when indicator or virtual keypad is visible.
	 */
	Evas_Object *conform = NULL;

	if (win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "window is NULL.");
		return NULL;
	}

	conform = elm_conformant_add(win);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, conform);

	evas_object_show(conform);

	return conform;
}

/**
 * @brief Creates a layout to target parent object with edje file.
 * @param[in] parent The object to which you want to add this layout
 * @param[in] file_path File path of the EDJ file will be used
 * @param[in] group_name Name of group in EDJ you want to set to
 * @param[in] cb_function The function called when back event is detected
 * @param[in] user_data The user data to be passed to the callback function
 */
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	/* Create layout using EDC(an edje file). */
	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, file_path, group_name);

	/* Layout size setting. */
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (cb_function)
		eext_object_event_callback_add(layout, EEXT_CALLBACK_BACK, cb_function, user_data);

	evas_object_show(layout);

	return layout;
}

/**
 * @brief Creates and sets a layout to conformant.
 * @param[in] parent Target conformant object
 * @param[in] file_path File path of EDJ will be used
 * @param[in] group_name Group name in EDJ you want to set to layout
 * @param[in] cb_function The function will be called when the back event is detected
 * @param[in] user_data The user data to be passed to the callback functions
 */
Evas_Object *view_create_layout_for_conformant(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	/* Create layout for conformant. */
	if (file_path == NULL)
		layout = view_create_layout_by_theme(parent, "layout", "application", "default");
	else
		layout = view_create_layout(parent, file_path, group_name, cb_function, user_data);

	if (layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "layout is NULL.");
		return NULL;
	}

	elm_object_content_set(parent, layout);

	return layout;
}

/**
 * @brief Creates a layout with theme.
 * @param[in] parent Object to which you want to add this layout
 * @param[in] class_name The class of the group
 * @param[in] group_name Group name in EDJ that you want to set to layout
 * @param[in] style The style to use
 */
Evas_Object *view_create_layout_by_theme(Evas_Object *parent, const char *class_name, const char *group_name, const char *style)
{
	/*
	 * Layout
	 * Create and initialize elm_layout.
	 * view_create_layout_by_theme() is used to create layout by using premade edje file.
	 */
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, class_name, group_name, style);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	return layout;
}

/**
 * @brief Destroys window and frees its resources.
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/**
 * @brief Sets image to given part.
 * @param[in] parent Object has part to which you want to set this image
 * @param[in] part_name Part name to which you want to set this image
 * @param[in] image_path Path of the image file
 */
void view_set_image(Evas_Object *parent, const char *part_name, const char *image_path)
{
	Evas_Object *image = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	image = elm_object_part_content_get(parent, part_name);
	if (image == NULL) {
		image = elm_image_add(parent);
		if (image == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "failed to create an image object.");
			return;
		}

		elm_object_part_content_set(parent, part_name, image);
	}

	if (EINA_FALSE == elm_image_file_set(image, image_path, NULL)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set image: %s", image_path);
		return;
	}

	evas_object_show(image);

	return;
}

/**
 * @brief Sets text to the part.
 * @param[in] parent Object has part to which you want to set text
 * @param[in] part_name Part name to which you want to set the text
 * @param[in] text Text you want to set to the part
 */
void view_set_text(Evas_Object *parent, const char *part_name, const char *text)
{
	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	/* Set text of target part object. */
	elm_object_part_text_set(parent, part_name, text);
}

/**
 * @brief Sets color of the part.
 * @param[in] parent Object has part to which you want to set color
 * @param[in] part_name Name of part to which you want to set color
 * @param[in] r R of RGBA you want to set to the part
 * @param[in] g G of RGBA you want to set to the part
 * @param[in] b B of RGBA you want to set to the part
 * @param[in] a A of RGBA you want to set to the part
 */
void view_set_color(Evas_Object *parent, const char *part_name, int r, int g, int b, int a)
{
	Evas_Object *obj = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	obj = elm_object_part_content_get(parent, part_name);
	if (obj == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get parent.");
		return;
	}

	/* Set color of target part object. */
	evas_object_color_set(obj, r, g, b, a);
}

/**
 * @brief Creates a naviframe and set to parent.
 * @param[in] parent Object to which you want to set naviframe
 * Add callback function will be operated when back key is pressed
 */
Evas_Object *view_create_naviframe(Evas_Object *parent)
{
	/* Naviframe
	 * Create and initialize elm_naviframe.
	 * Naviframe stands for navigation frame.
	 * elm_naviframe is a views manager for applications.
	 */
	Evas_Object *nf = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	nf = elm_naviframe_add(parent);

	elm_object_part_content_set(parent, "elm.swallow.content", nf);
	eext_object_event_callback_add(nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	evas_object_show(nf);

	return nf;
}

/**
 * @brief Pushs item to naviframe.
 * @param[in] nf Naviframe has several views
 * @param[in] item Object will be added to naviframe
 * @param[in] pop_cb Function will be operated when this item is popped from naviframe
 * @param[in] user_data Data passed to the 'pop_cb' function
 * Naviframe make changing of view is easy and effectively
 */
Elm_Object_Item* view_push_item_to_naviframe(Evas_Object *nf, Evas_Object *item, Elm_Naviframe_Item_Pop_Cb pop_cb, void *user_data)
{
	Elm_Object_Item* nf_it = NULL;

	if (nf == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "naviframe is NULL.");
		return NULL;
	}

	if (item == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "item is NULL.");
		return NULL;
	}

	nf_it = elm_naviframe_item_push(nf, NULL, NULL, NULL, item, "empty");

	if (pop_cb != NULL)
		elm_naviframe_item_pop_cb_set(nf_it, pop_cb, user_data);

	return nf_it;
}

/**
 * @brief Sets the function will be called when the appointed signal is occurred.
 * @param[in] item The object triggered the signal
 * @param[in] signal The appointed signal to trigger the function
 * @param[in] source The appointed source that normally indicate the object triggered the event
 * @param[in] signal_cb The function will be called when the signal is detected
 * @param[in] user_data The data passed to the 'signal_cb' function
 */
void view_set_customized_event_callback(Evas_Object *item, char *signal, char *source, Edje_Signal_Cb signal_cb, void *user_data)
{
	elm_object_signal_callback_add(item, signal, source, signal_cb, user_data);
}

/**
 * @brief Registers rotary event callback function.
 * @param[in] obj Object that will receive rotary event
 * @param[in] rotary_cb Function will be operated when rotary event happens
 * @param[in] user_data Data passed to the 'rotary_cb' function
 */
void view_set_rotary_event_callback(Evas_Object *obj, Eext_Rotary_Event_Cb rotary_cb, void *user_data)
{
	if (obj == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	eext_rotary_object_event_activated_set(obj, EINA_TRUE);
	eext_rotary_object_event_callback_add(obj, rotary_cb, user_data);
}

/**
 * @brief Creates a label and set label options.
 * @param[in] parent The object to which you want to add this label
 */
Evas_Object *view_create_label(Evas_Object *parent)
{
	Evas_Object *label = elm_label_add(parent);

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	elm_object_style_set(label, "slide_short");
	elm_label_wrap_width_set(label, 230);
	elm_label_ellipsis_set(label, EINA_TRUE);
	elm_label_slide_duration_set(label, 2);
	elm_label_slide_mode_set(label, ELM_LABEL_SLIDE_MODE_NONE);
	elm_label_slide_go(label);
	evas_object_show(label);

	return label;
}

/**
 * @brief Sets a text to label object.
 * @param[in] parent Object has part to which you want to set text
 * @param[in] part_name Part name to which you want to set text
 * @param[in] text Text you want to set to the part
 */
void view_set_label_text(Evas_Object *parent, const char *part_name, const char *text)
{
	Evas_Object *label = NULL;
	char *markup_text = NULL;
	char buf[256] = { 0, };

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	markup_text = elm_entry_utf8_to_markup(text);
	snprintf(buf, sizeof(buf), "%s%s%s", LABEL_STYLE_START, markup_text, LABEL_STYLE_END);
	free(markup_text);

	label = elm_object_part_content_get(parent, part_name);
	if (label == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "label is NULL.");
		return;
	}

	elm_object_text_set(label, buf);
}

/**
 * @brief Sets a label to given part.
 * @param[in] parent Object has part to which you want to set this label
 * @param[in] part_name Part name to which you want to set this label
 */
void view_set_label(Evas_Object *parent, const char *part_name)
{
	Evas_Object *label = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	label = view_create_label(parent);
	if (label == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "label is NULL.");
		return;
	}

	elm_object_part_content_set(parent, part_name, label);
}

/**
 * @brief Create item layout that will be shown to this app.
 * @param[in] parent The object to which you want to add this layout
 * @param[in] name Text you want to set to item layout
 * @param[in] file_path File path of EDJ
 * @param[in] image_path Path of image file you want to set to item layout
 * @param[in] default_image_path Path of default image file if img_path is NULL, background is set by default image
 * @param[in] user_data Data needed in this function
 */
Evas_Object *view_buddy_create_item_layout(Evas_Object *parent, const char *name, const char *file_path, const char *image_path, const char *default_image_path, void *user_data)
{
	Evas_Object *item_layout = NULL;

	item_layout = view_create_layout(parent, file_path, "list-item", NULL, user_data);

	if (!strcmp(image_path, "NULL")) {
		char buf[128] = { 0, };
		char first_char  = name[0];

		view_set_image(item_layout, "sw.bg", default_image_path);
		view_set_color(item_layout, "sw.bg", 207, 115, 58, 255);

		snprintf(buf, sizeof(buf), "%c", first_char);
		view_set_text(item_layout, "txt.first.char", buf);
	} else {
		view_set_image(item_layout, "sw.bg", image_path);
	}

	return item_layout;
}

/**
 * @brief Change font size of item layout in list.
 * @param[in] item Object that contains text
 * @param[in] font_size Font size you want to set
 */
void view_buddy_change_item_font_size(Evas_Object *item, int font_size)
{
	char buf[128] = { 0, };

	snprintf(buf, sizeof(buf), "change.text.size.%d", font_size);
	elm_object_signal_emit(item, buf, "txt.first.char");
}

/**
 * @brief Set visible state of item layout in list.
 * @param[in] item_list List that contains items
 * @param[in] is_visible Visible state you want to set
 */
void view_buddy_set_item_visible(Eina_List *item_list, Eina_Bool is_visible)
{
	Eina_List *l = NULL;
	Evas_Object *item = NULL;

	EINA_LIST_FOREACH(item_list, l, item) {
		if (is_visible) {
			item_info_s *item_info = evas_object_data_get(item, "__ITEM_INFO__");
			if (item_info->position >= 0 && item_info->position < 11) {
				evas_object_show(item);
			}
		} else {
			evas_object_hide(item);
		}
	}
}

/**
 * @brief Create additional object for buddy.
 */
void view_buddy_create(void)
{
	/*
	 * Eext Circle Surface Creation.
	 */
	s_info.circle_surface = eext_circle_surface_conformant_add(s_info.conform);

	s_info.layout = view_create_layout_for_conformant(s_info.conform, NULL, NULL, NULL, NULL);

	s_info.nf = view_create_naviframe(s_info.layout);
	if (s_info.nf == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a naviframe.");
		evas_object_del(s_info.win);
		s_info.win = NULL;
		return;
	}
}

/**
 * Below functions are static functions.
 * You don't need to modify them.
 */

/**
 * @brief Function will be operated when registered event is triggered.
 * @param[in] data The data to be passed to the callback function
 * @param[in] obj The Evas object handle to be passed to the callback function
 * @param[in] event_info The system event information
 */
static void _dummy_callback_cb(void *data, Evas_Object *obj, void *event_info)
{
	/*
	 * Write your code here for smart callback.
	 */
	ui_app_exit();
}
