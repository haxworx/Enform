#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config       Config;

#define CHECK_NONE      0
#define CHECK_ACPI      1
#define CHECK_APM       2
#define CHECK_PMU       3
#define CHECK_SYS_ACPI  4

#define UNKNOWN 0
#define NOSUBSYSTEM 1
#define SUBSYSTEM 2

#define SUSPEND 0
#define HIBERNATE 1
#define SHUTDOWN 2

#define POPUP_DEBOUNCE_CYCLES  2

struct _Config
{
   /* saved * loaded config values */
   int              poll_interval;
   int              alert;	/* Alert on minutes remaining */
   int	            alert_p;    /* Alert on percentage remaining */
   int              alert_timeout;  /* Popup dismissal timeout */
   int              suspend_below;  /* Suspend if battery drops below this level */
   int              suspend_method; /* Method used to suspend the machine */
   int              force_mode; /* force use of batget or hal */
   /* just config state */
   E_Module        *module;
   E_Config_Dialog *config_dialog;
   Eina_List       *instances;
   Ecore_Exe           *batget_exe;
   Ecore_Event_Handler *batget_data_handler;
   Ecore_Event_Handler *batget_del_handler;
   Ecore_Timer         *alert_timer;
   int                  full;
   int                  time_left;
   int                  time_full;
   int                  have_battery;
   int                  have_power;
#ifdef HAVE_EEZE
   Eeze_Udev_Watch     *acwatch;
   Eeze_Udev_Watch     *batwatch;
#endif
#if defined HAVE_EEZE 
   Eina_Bool            fuzzy;
   int                  fuzzcount;
#endif
};

typedef struct _Battery Battery;
typedef struct _Ac_Adapter Ac_Adapter;

struct _Battery
{
   const char *udi;
#if defined HAVE_EEZE 
   Ecore_Poller *poll;
#endif
   Eina_Bool present:1;
   Eina_Bool charging:1;
#if defined HAVE_EEZE 
   double last_update;
   double percent;
   double current_charge;
   double design_charge;
   double last_full_charge;
   double charge_rate;
   double time_full;
   double time_left;
#else
   int percent;
   int current_charge;
   int design_charge;
   int last_full_charge;
   int charge_rate;
   int time_full;
   int time_left;
   const char *type;
   const char *charge_units;
#endif
   const char *technology;
   const char *model;
   const char *vendor;
   Eina_Bool got_prop:1;
};

struct _Ac_Adapter
{
   const char *udi;
   Eina_Bool present:1;
   const char *product;
};

Battery *_battery_battery_find(const char *udi);
Ac_Adapter *_battery_ac_adapter_find(const char *udi);
void _battery_device_update(void);
#ifdef HAVE_EEZE
/* in e_mod_udev.c */
int  _battery_udev_start(void);
void _battery_udev_stop(void);
/* end e_mod_udev.c */
#else
/* in e_mod_dbus.c */
int  _battery_dbus_start(void);
void _battery_dbus_stop(void);
#endif

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

E_Config_Dialog *e_int_config_battery_module(E_Container *con, const char *params __UNUSED__);
    
void _battery_config_updated(void);
extern Config *battery_config;

/**
 * @addtogroup Optional_Gadgets
 * @{
 *
 * @defgroup Module_Battery Battery
 *
 * Shows battery level and current status, may do actions given some
 * thresholds.
 *
 * @}
 */

#endif
