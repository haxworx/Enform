#include "e.h"
#include "e_mod_main.h"

static Eina_List *sthemes = NULL;
static Eina_List *themes = NULL;
static const char *cur_theme = NULL;

static E_Module *conf_module = NULL;
static E_Int_Menu_Augmentation *maug[8] = {0};

/* module setup */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "Settings - Theme"
};

/* menu item add hook */
static void
_e_mod_run_wallpaper_cb(void *data __UNUSED__, E_Menu *m, E_Menu_Item *mi __UNUSED__)
{
   e_configure_registry_call("appearance/wallpaper", m->zone->container, NULL);
}

static void
_e_mod_menu_wallpaper_add(void *data __UNUSED__, E_Menu *m)
{
   E_Menu_Item *mi;

   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, _("Wallpaper"));
   e_util_menu_item_theme_icon_set(mi, "preferences-desktop-wallpaper");
   e_menu_item_callback_set(mi, _e_mod_run_wallpaper_cb, NULL);
}

static void
_e_mod_run_theme_cb(void *data __UNUSED__, E_Menu *m __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   e_configure_registry_call("appearance/theme", m->zone->container, NULL);
}
static int
_sort_cb(const char *a, const char *b)
{
   const char *f1, *f2;

   f1 = ecore_file_file_get(a);
   f2 = ecore_file_file_get(b);
   return e_util_strcasecmp(f1, f2);
}

#define SLASH '/'

void BOO(char *str)
{
	FILE *f = fopen("/home/al/poo.txt", "w+");
	fprintf(f, "here %s\n", str);
	fclose(f);
}

static Eina_List *
check_path_for_themes(const char *path, Eina_List *list)
{
	DIR *d = opendir(path);
	struct dirent *dh = NULL;

	while ((dh = readdir(d)) != NULL) {
		if (!strncmp(dh->d_name, ".", 1)) { continue; }

		if (eina_str_has_extension(dh->d_name, ".edj")) {
			struct stat fstats;
			char file[PATH_MAX] = { 0 };
			snprintf(file, sizeof(file), "%s%c%s", path, SLASH, dh->d_name);
			stat(file, &fstats);
			if (S_ISDIR(fstats.st_mode)) { continue; }
			list = eina_list_append(list, strdup(file));
		}
	}
	closedir(d);
	
	return list;
}


static void
_get_theme_lists(void)
{
	char path[PATH_MAX] = { 0 };
	char spath[PATH_MAX] = { 0 };

	if (themes)
		E_FREE_LIST(themes, free);
	if (sthemes)
		E_FREE_LIST(sthemes, free);

	e_user_dir_concat_static(path, "themes");
	
	themes = check_path_for_themes(path, themes);

	e_prefix_data_concat_static(spath, "data/themes");

	sthemes = check_path_for_themes(spath, sthemes);	

	if (themes)
		themes = eina_list_sort(themes, 0, (Eina_Compare_Cb)_sort_cb);
	if (sthemes)
	        sthemes = eina_list_sort(sthemes, 0, (Eina_Compare_Cb)_sort_cb);
}

static void
_theme_set(void *data, E_Menu *m __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   E_Action *a;

   if (!e_util_strcmp(data, cur_theme)) return;

   e_theme_config_set("theme", data);
   e_config_save_queue();

   a = e_action_find("restart");
   if ((a) && (a->func.go)) a->func.go(NULL, NULL);
}

static void
_item_new(char *file, E_Menu *m)
{
   E_Menu_Item *mi;
   char *name;
   Eina_Bool used;
  
   /* BOGO: */
   /* Fix OpenBSD */ 
   /* ecore_file_file_get problems */
   char *f = strdup(file);

   name = strrchr(f, '/');
   if (!name) return;
   ++name; // skip flash
   char *dot = strrchr(name, '.');
   if (dot) {
     *dot = '\0';
   }

   used = !e_util_strcmp(file, cur_theme);

   //name = (char*)ecore_file_file_get(file);
   // if (!name) return;
   // sfx = strrchr(name, '.');
   //name = strndupa(name, sfx - name);

   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, name);
   if (used)
     e_menu_item_disabled_set(mi, 1);
   else
     e_menu_item_callback_set(mi, _theme_set, file);
   e_menu_item_check_set(mi, 1);
   e_menu_item_toggle_set(mi, used);

   free(f);
}

static void
_e_mod_menu_theme_del(void *d __UNUSED__)
{
  cur_theme = NULL;
}

static void
_e_mod_menu_theme_add(void *data __UNUSED__, E_Menu *m)
{
   E_Menu_Item *mi;
   E_Config_Theme *ct;
   char *file;
   Eina_List *l;

   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, _("Theme"));
   e_util_menu_item_theme_icon_set(mi, "preferences-desktop-theme");
   e_menu_item_callback_set(mi, _e_mod_run_theme_cb, NULL);
   ct = e_theme_config_get("theme");
   if (!ct) return;

   cur_theme = ct->file;   
   m = e_menu_new();
   e_object_del_attach_func_set(E_OBJECT(m), _e_mod_menu_theme_del);
   e_menu_title_set(m, _("Themes"));
   e_menu_item_submenu_set(mi, m);

   _get_theme_lists();
   EINA_LIST_FOREACH(themes, l, file)
     _item_new(file, m);
   if (themes && sthemes)
     e_menu_item_separator_set(e_menu_item_new(m), 1);
   EINA_LIST_FOREACH(sthemes, l, file)
     _item_new(file, m);
}


EAPI void *
e_modapi_init(E_Module *m)
{
   e_configure_registry_category_add("internal", -1, _("Internal"),
                                     NULL, "enform/internal");
   e_configure_registry_item_add("internal/wallpaper_desk", -1, _("Wallpaper"),
                                 NULL, "preferences-system-windows", e_int_config_wallpaper_desk);
   e_configure_registry_item_add("internal/borders_border", -1, _("Border"),
                                 NULL, "preferences-system-windows", e_int_config_borders_border);

   e_configure_registry_category_add("appearance", 10, _("Look"), NULL,
                                     "preferences-look");
   e_configure_registry_item_add("appearance/wallpaper", 10, _("Wallpaper"), NULL,
                                 "preferences-desktop-wallpaper",
                                 e_int_config_wallpaper);
   e_configure_registry_item_add("appearance/theme", 20, _("Theme"), NULL,
                                 "preferences-desktop-theme",
                                 e_int_config_theme);
   e_configure_registry_item_add("appearance/xsettings", 20, _("GTK+/X11 Theme"), NULL,
                                 "preferences-desktop-theme",
                                 e_int_config_xsettings);
   e_configure_registry_item_add("appearance/colors", 30, _("Colors"), NULL,
                                 "preferences-desktop-color",
                                 e_int_config_color_classes);
   e_configure_registry_item_add("appearance/fonts", 40, _("Fonts"), NULL,
                                 "preferences-desktop-font",
                                 e_int_config_fonts);
   e_configure_registry_item_add("appearance/borders", 50, _("Borders"), NULL,
                                 "preferences-system-windows",
                                 e_int_config_borders);
   e_configure_registry_item_add("appearance/transitions", 60, _("Transitions"), NULL,
                                 "preferences-transitions",
                                 e_int_config_transitions);
   e_configure_registry_item_add("appearance/scale", 70, _("Scaling"), NULL,
                                 "preferences-scale",
                                 e_int_config_scale);
   maug[0] =
     e_int_menus_menu_augmentation_add_sorted("config/1", _("Wallpaper"),
                                              _e_mod_menu_wallpaper_add, NULL, NULL, NULL);
   maug[1] =
     e_int_menus_menu_augmentation_add_sorted("config/1", _("Theme"),
                                              _e_mod_menu_theme_add, NULL, NULL, NULL);

   conf_module = m;
   e_module_delayed_set(m, 1);

   _get_theme_lists();

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   E_Config_Dialog *cfd;

   /* remove module-supplied menu additions */
   if (maug[0])
     {
        e_int_menus_menu_augmentation_del("config/1", maug[0]);
        maug[0] = NULL;
     }
   if (maug[1])
     {
        e_int_menus_menu_augmentation_del("config/1", maug[1]);
        maug[1] = NULL;
     }

     if (themes)
     E_FREE_LIST(themes, free);
     if (sthemes)
     E_FREE_LIST(sthemes, free);

   cur_theme = NULL;

   while ((cfd = e_config_dialog_get("E", "appearance/scale")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/transitions")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/borders")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/fonts")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/colors")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "apppearance/theme")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/wallpaper")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/xsettings")))
     e_object_del(E_OBJECT(cfd));

   e_configure_registry_item_del("appearance/scale");
   e_configure_registry_item_del("appearance/transitions");
   e_configure_registry_item_del("appearance/borders");
   e_configure_registry_item_del("appearance/fonts");
   e_configure_registry_item_del("appearance/colors");
   e_configure_registry_item_del("appearance/theme"); 
   e_configure_registry_item_del("appearance/wallpaper");
   e_configure_registry_item_del("appearance/xsettings");
   e_configure_registry_category_del("appearance");

   while ((cfd = e_config_dialog_get("E", "internal/borders_border")))
     e_object_del(E_OBJECT(cfd));
   while ((cfd = e_config_dialog_get("E", "appearance/wallpaper")))
     e_object_del(E_OBJECT(cfd));
   e_configure_registry_item_del("appearance/borders");
   e_configure_registry_item_del("internal/wallpaper_desk");
   e_configure_registry_category_del("internal");

   conf_module = NULL;
   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   return 1;
}
