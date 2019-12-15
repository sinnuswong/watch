#if !defined(_VIEW_H)
#define _VIEW_H

#include <Elementary.h>
#include <efl_extension.h>

void view_create_with_size(int width, int height);
void view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_layout_for_part(Evas_Object *parent, char *file_path, char *group_name, char *part_name);
void view_destroy(void);
void view_set_hour(int hour);
void view_set_minute(int minute);
void view_set_second(int second);
void view_start_minute_update(void);
void view_update_display_time(int hour, int minute);
void view_set_ambient_mode(Eina_Bool ambient);
void view_reset_display_state(void);

#endif
