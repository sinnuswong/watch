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

void view_set_text(Evas_Object *parent, const char *part_name, const char *text);
void view_rotate_hand(Evas_Object *hand, double degree, Evas_Coord cx, Evas_Coord cy);
Evas_Object *view_create_watch_window(int width, int height);
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, void *user_data);
Evas_Object *view_create_bg(Evas_Object *win, const char *image_path, int width, int height);
Evas_Object *view_chronograph_create_layout(Evas_Object *parent, const char *file_path);
void view_chronograph_create_parts(const char *image_path, int position_x, int position_y, int size_w, int size_h, parts_type_e type);
Evas_Object *view_create_parts(Evas_Object *parent, const char *image_path, int position_x, int position_y, int size_w, int size_h);

Eina_Bool view_chronograph_update_time(void *data);
void view_chronograph_set_date(watch_time_h watch_time);
void view_chronograph_animator_thaw(void);
void view_chronograph_animator_freeze(void);
void view_chronograph_animator_add(void);
void view_destroy_base_gui(void);
#endif
