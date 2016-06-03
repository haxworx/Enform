#if 0

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sensors.h>

#include "e.h"
#include "e_mod_main.h"

static Eina_Bool _battery_openbsd_battery_update_poll(void *data);
static void      _battery_openbsd_battery_update();

extern Eina_List *device_batteries;
extern double init_time;

Battery *bat = NULL;

int *bat_mib = NULL;

int
_battery_openbsd_start(void)
{
   int mib[] = {CTL_HW, HW_SENSORS, 0, 0, 0};
   int devn;
   struct sensordev snsrdev;
   size_t sdlen = sizeof(struct sensordev);

   for (devn = 0;; devn++) {
        mib[2] = devn;
        if (sysctl(mib, 3, &snsrdev, &sdlen, NULL, 0) == -1)
          {
             if (errno == ENXIO)
               continue;
             if (errno == ENOENT)
               break;
          }

        if (!strcmp("acpibat0", snsrdev.xname))
          {
             if (!(bat = E_NEW(Battery, 1)))
               return 0;
             bat->udi = eina_stringshare_add("acpibat0"),
             bat_mib = malloc(sizeof(int) * 5);
             if (!bat_mib) return 0;
             bat_mib[0] = mib[0];
             bat_mib[1] = mib[1];
             bat_mib[2] = mib[2];
             bat->technology = eina_stringshare_add("Unknown");
             bat->model = eina_stringshare_add("Unknown");
             bat->vendor = eina_stringshare_add("Unknown");
             bat->last_update = ecore_time_get();
             bat->poll = ecore_poller_add(ECORE_POLLER_CORE,
                                          battery_config->poll_interval,
                                          _battery_openbsd_battery_update_poll, NULL);
             device_batteries = eina_list_append(device_batteries, bat);
         }
   }

   _battery_openbsd_battery_update();

   init_time = ecore_time_get();

   return 1;
}

void
_battery_openbsd_stop(void)
{
   if (bat)
     {
	eina_stringshare_del(bat->udi);
        eina_stringshare_del(bat->technology);
        eina_stringshare_del(bat->model);
        eina_stringshare_del(bat->vendor);
        ecore_poller_del(bat->poll);
	free(bat_mib);
        free(bat);
     }
}

static Eina_Bool
_battery_openbsd_battery_update_poll(void *data)
{
   data = data; // BOGO shut up compiler
   _battery_openbsd_battery_update();
   return EINA_TRUE;
}

static void
_battery_openbsd_battery_update()
{
   double _time = 0;
   struct sensor s;
   size_t slen = sizeof(struct sensor);

   /* update the poller interval */
   ecore_poller_poller_interval_set(bat->poll,
                                    battery_config->poll_interval);
   double current_charge = 0;
   double last_full_charge = 0;

   if (bat)
   { 

   /* last full capacity */
   bat_mib[3] = 7;
   bat_mib[4] = 0;

   if (sysctl(bat_mib, 5, &s, &slen, NULL, 0) != -1)
     {
       last_full_charge = (double)s.value;
     }
   
   /* remaining capcity */
   bat_mib[3] = 7;
   bat_mib[4] = 3;

   if (sysctl(bat_mib, 5, &s, &slen, NULL, 0) != -1)
     {
        current_charge = (double)s.value;
     }
   
    /* This *should* workaround a bug */
   if (current_charge == 0 || last_full_charge == 0)
     {
        bat_mib[3] = 8;
        bat_mib[4] = 0;

        if (sysctl(bat_mib, 5, &s, &slen, NULL, 0) != -1)
	  last_full_charge = (double)s.value;
		
	bat_mib[3] = 8;
        bat_mib[4] = 3; 
 	
	if (sysctl(bat_mib, 5, &s, &slen, NULL, 0) != -1)
	  current_charge = (double)s.value;
     } 
   
   bat->got_prop = 1;
   _time = ecore_time_get();

   bat->last_update = _time;
   bat->current_charge = current_charge;

   double percent_double = 100 * (current_charge / last_full_charge);
   if (current_charge >= last_full_charge) 
     {
       percent_double = 100; // another bug workaround
     }

   bat->percent = (int) percent_double;
   _battery_device_update();
   }
}

#endif
