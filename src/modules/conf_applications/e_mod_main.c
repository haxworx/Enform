#include "e.h"
#include "e_mod_main.h"

/* actual module specifics */

static E_Module *conf_module = NULL;

/* module setup */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "Settings - Applications"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   e_configure_registry_category_add("applications", 20, _("Apps"), NULL,
                                     "preferences-applications");
   e_configure_registry_item_add("applications/new_application", -1,
                                 _("Create Application Launcher"), NULL,
                                 "preferences-applications-add",
                                 e_int_config_apps_add);
/*
   e_configure_registry_item_add("applications/personal_applications", 20,
                                 _("Personal Application Launchers"), NULL,
                                 "preferences-applications-personal",
                                 e_int_config_apps_personal);
*/
   e_configure_registry_item_add("applications/favorite_applications", 30,
                                 _("Favorite Applications"), NULL,
                                 "user-bookmarks",
                                 e_int_config_apps_favs);
/*
   e_configure_registry_item_add("applications/screen_lock_applications", 45,
                                 _("Screen Lock Applications"), NULL,
                                 "preferences-applications-screen-lock",
                                 e_int_config_apps_desk_lock);
   e_configure_registry_item_add("applications/screen_unlock_applications", 46,
                                 _("Screen Unlock Applications"), NULL,
                                 "preferences-applications-screen-unlock",
                                 e_int_config_apps_desk_unlock);
*/
   e_configure_registry_item_add("applications/restart_applications", 50,
                                 _("Restart Applications"), NULL,
                                 "preferences-applications-restart",
                                 e_int_config_apps_restart);
   e_configure_registry_item_add("applications/startup_applications", 60,
                                 _("Startup Applications"), NULL,
                                 "preferences-applications-startup",
                                 e_int_config_apps_startup);
   e_configure_registry_item_add("applications/default_applications", 70,
                                 _("Default Applications"), NULL,
                                 "preferences-desktop-default-applications",
                                 e_int_config_defapps);
/*
   e_configure_registry_item_add("applications/desktop_environments", 80,
                                 _("Desktop Environments"), NULL,
                                 "preferences-desktop-environments",
                                 e_int_config_deskenv);
*/
   e_configure_registry_category_add("internal", -1, _("Internal"), NULL,
                                     "enform/internal");

   conf_module = m;
   e_module_delayed_set(m, 1);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   e_configure_registry_item_del("applications/favorite_applications");
   e_configure_registry_item_del("applications/new_application");
   e_configure_registry_item_del("applications/personal_applications");
   e_configure_registry_item_del("applications/restart_applications");
   e_configure_registry_item_del("applications/startup_applications");
   e_configure_registry_item_del("applications/default_applications");
   e_configure_registry_item_del("applications/desktop_environments");
   e_configure_registry_category_del("applications");
   conf_module = NULL;
   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   return 1;
}

