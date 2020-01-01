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

#if !defined(_VIEW_H)
#define _VIEW_H

#define EDJ_FILE "edje/main.edj"
#define GRP_MAIN "main"

Evas_Object *view_get_window(void);
Evas_Object *view_get_conformant(void);
Evas_Object *view_buddy_get_layout(void);
Evas_Object *view_get_naviframe(void);
void view_buddy_detail_view_layout_destroy(void);

/*
 * Create a view
 */
Eina_Bool view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_conformant_without_indicator(Evas_Object *win);
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data);
Evas_Object *view_create_layout_for_conformant(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data);
Evas_Object *view_create_layout_by_theme(Evas_Object *parent, const char *class_name, const char *group_name, const char *style);
void view_destroy(void);
void view_set_image(Evas_Object *parent, const char *part_name, const char *image_path);
void view_set_text(Evas_Object *parent, const char *part_name, const char *text);
void view_set_color(Evas_Object *parent, const char *part_name, int r, int g, int b, int a);
Evas_Object *view_create_naviframe(Evas_Object *parent);
Elm_Object_Item* view_push_item_to_naviframe(Evas_Object *nf, Evas_Object *item, Elm_Naviframe_Item_Pop_Cb pop_cb, void *user_data);
void view_set_customized_event_callback(Evas_Object *item, char *signal, char *source, Edje_Signal_Cb signal_cb, void *user_data);
void view_set_rotary_event_callback(Evas_Object *obj, Eext_Rotary_Event_Cb rotary_cb, void *user_data);
Evas_Object *view_create_label(Evas_Object *parent);
void view_set_label_text(Evas_Object *parent, const char *part_name, const char *text);
void view_set_label(Evas_Object *parent, const char *part_name);

Evas_Object *view_buddy_create_item_layout(Evas_Object *parent, const char *name, const char *file_path, const char *image_path, const char *default_image_path, void *user_data);
void view_buddy_change_item_font_size(Evas_Object *item, int font_size);
void view_buddy_set_item_visible(Eina_List *item_list, Eina_Bool is_visible);
void view_buddy_create(void);

#endif
