/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
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

/*
 * Create a view
 */
void view_set_watchface_layout(Evas_Object *layout);
void view_set_indicator_module_layout(Evas_Object *layout, int type);
Evas_Object *view_get_watchface_layout(void);
Evas_Object *view_get_battery_module_layout(void);
Evas_Object *view_get_ap_module_layout(void);

void view_set_text(Evas_Object *parent, const char *part_name, const char *text);
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, void *user_data);
void view_send_signal_to_edje(Evas_Object *layout, const char *signal, const char *source);
void view_set_customized_event_callback(Evas_Object *item, char *signal, char *source, Edje_Signal_Cb signal_cb, void *user_data);
void view_rotate_hand(Evas_Object *hand, double degree, Evas_Coord cx, Evas_Coord cy);
void view_rotate_parts(Evas_Object *layout, int type, float degree);
Evas_Object *view_create_parts(Evas_Object *parent, const char *image_path, int x, int y, int w, int h, Eina_Bool is_map);
void view_destroy_base_gui(void);
#endif
