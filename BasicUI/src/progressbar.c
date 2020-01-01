#include "main.h"
#include "google-authenticator.c"

static Eina_Bool progress_timer_cb(void *data) {
	double value = 0.0;
	Evas_Object *progressbar = data;

	value = eext_circle_object_value_get(progressbar);
	value += 0.1;
	if (value > 30)
		value = 0;
	eext_circle_object_value_set(progressbar, value);

	return ECORE_CALLBACK_RENEW;
}

static void progressbar_del_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info) {
	Ecore_Timer *timer = data;
	ecore_timer_del(timer);
}

static Eina_Bool
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
//	ui_app_exit();
	return EINA_TRUE;
}

static Eina_Bool update_compute(void *data) {
	Evas_Object *ly = data;
	char * se = "abcdefghijklmnopqrstuvwxyz";
	unsigned long c = time(NULL) / 30;
	dlog_print(DLOG_ERROR, LOG_TAG, "%ld test d", time(NULL));
	int res = generateCode(se, c);
//	dlog_print(DLOG_ERROR, LOG_TAG, "%d test d", res);
	char buf[10];
	snprintf(buf, sizeof(buf), "%d", (int) res);
	elm_object_part_text_set(ly, "elm.text.progressbar", buf);
	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool first_compute(void *data) {
	Evas_Object *ly = data;
	char * se = "abcdefghijklmnopqrstuvwxyz";
	unsigned long c = time(NULL) / 30;
	//dlog_print(DLOG_ERROR, LOG_TAG, "%ld test d", time(NULL));
	int res = generateCode(se, c);
//	dlog_print(DLOG_ERROR, LOG_TAG, "%d test d", res);
	char buf[10];
	snprintf(buf, sizeof(buf), "%d", (int) res);
	elm_object_part_text_set(ly, "elm.text.progressbar", buf);

	Ecore_Timer * timer = ecore_timer_add(30, update_compute, ly);
	evas_object_event_callback_add(ly, EVAS_CALLBACK_DEL, progressbar_del_cb,
			timer);
	return ECORE_CALLBACK_CANCEL;
}
void progressbar_cb(void *data, Evas_Object *obj EINA_UNUSED,
		void *event_info EINA_UNUSED) {

	Elm_Object_Item * item = elm_list_selected_item_get((Elm_List*) obj);
	char * label = elm_object_item_text_get(item);

	appdata_s *ad = data;
	Evas_Object *nf = ad->nf;
	Evas_Object *conf = ad->conform;
	Evas_Object *ly;
	Evas_Object *pb;
	Eext_Circle_Surface *circle_surface;

	/* Set a circle surface */
	circle_surface = eext_circle_surface_conformant_add(conf);

	/* Add a layout and push the layout into the naviframe */
	/* EDJ_PATH is folder path, it contains the progressbar_circle.edj file */
	ly = elm_layout_add(nf);

	char pro[PATH_MAX];
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(pro, (int) PATH_MAX, "%s%s", res_path, "edje/pro.edj");
		free(res_path);
	}
	elm_layout_file_set(ly, pro, "eext_progressbar_layout");
	evas_object_show(ly);

	/* Add a progressbar and set a circle surface to render the progressbar */
	pb = eext_circle_object_progressbar_add(ly, circle_surface);
	/*Set a minimum and maximum values */
	eext_circle_object_value_min_max_set(pb, 0.0, 30.0);
	/* Set a progressbar value */
	int c = time(NULL) % 30;
	eext_circle_object_value_set(pb, c);
	/* Set a part content */
	elm_object_part_content_set(ly, "elm.swallow.content", pb);

	Ecore_Timer *progress_timer = ecore_timer_add(0.1, progress_timer_cb, pb);
	evas_object_event_callback_add(pb, EVAS_CALLBACK_DEL, progressbar_del_cb,
			progress_timer);
	evas_object_show(pb);
	update_compute(ly);
	Ecore_Timer * timer2 = ecore_timer_add(30 - c, first_compute, ly);
	evas_object_event_callback_add(pb, EVAS_CALLBACK_DEL, progressbar_del_cb,
			timer2);
	/* Get a progressbar value and set a text */
	Elm_Object_Item *nf_it = elm_naviframe_item_push(nf, label, NULL, NULL, ly, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, ad->win);
}

void add_cb(void *data, Evas_Object *obj EINA_UNUSED,
		void *event_info EINA_UNUSED) {

	Elm_Object_Item * item = elm_list_selected_item_get((Elm_List*) obj);
	char * label = elm_object_item_text_get(item);

	appdata_s *ad = data;
	Evas_Object *nf = ad->nf;
	Evas_Object *conf = ad->conform;
	Evas_Object *ly;
	Evas_Object *pb;
	Eext_Circle_Surface *circle_surface;

	/* Set a circle surface */
	circle_surface = eext_circle_surface_conformant_add(conf);

	/* Add a layout and push the layout into the naviframe */
	/* EDJ_PATH is folder path, it contains the progressbar_circle.edj file */
	ly = elm_layout_add(nf);

	char pro[PATH_MAX];
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(pro, (int) PATH_MAX, "%s%s", res_path, "edje/pro.edj");
		free(res_path);
	}
	elm_layout_file_set(ly, pro, "eext_progressbar_layout");
	evas_object_show(ly);
	Evas_Object *entry;


	entry = elm_entry_add(ly);
	elm_entry_entry_set(entry, "A short text.");
	elm_object_part_content_set(ly, "elm.swallow.content", entry);
//	char buf[10];
//		snprintf(buf, sizeof(buf), "%d", (int) res);
//		elm_object_part_text_set(ly, "elm.text.progressbar", buf);
//	/* Add a progressbar and set a circle surface to render the progressbar */
//	pb = eext_circle_object_progressbar_add(ly, circle_surface);
//	/*Set a minimum and maximum values */
//	eext_circle_object_value_min_max_set(pb, 0.0, 30.0);
//	/* Set a progressbar value */
//	int c = time(NULL) % 30;
//	eext_circle_object_value_set(pb, c);
//	/* Set a part content */
//	elm_object_part_content_set(ly, "elm.swallow.content", pb);


	Elm_Object_Item *nf_it = elm_naviframe_item_push(nf, label, NULL, NULL, ly, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, ad->win);
}
