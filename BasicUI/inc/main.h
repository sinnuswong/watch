
#define __basicui_H__

#if !defined(PACKAGE)
#define PACKAGE "org.sinnus.basicui"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "basicui"

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <dlog.h>
#include <efl_extension.h>

#define TIZEN_ENGINEER_MODE

#define EDJ_FILE "edje/basicuiwithedc.edj"
#define GRP_MAIN "progessbar_layout"

#define ICON_PATH "images"


typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *fb;
	Evas_Object *nf;
} appdata_s;

char ELM_DEMO_EDJ[PATH_MAX];
char ICON_DIR[PATH_MAX];

static void app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max);
void progressbar_cb(void *data, Evas_Object *obj, void *event_info);
void add_cb(void *data, Evas_Object *obj EINA_UNUSED,
		void *event_info EINA_UNUSED);

