#include "e_mod_system.h"

#if defined(__OpenBSD__)
static const char *_name = NULL;

static void
_e_mixer_openbsd_set(void)
{
  if (!_name)
    _name = eina_stringshare_add("Master");
}

E_Mixer_System *
e_mixer_system_new(const char *name)
{
  _e_mixer_openbsd_set();
  if (!name)
    return NULL;

  if (name == _name || strcmp(name, _name) == 0)
    return (E_Mixer_System *) - 1;
  else
    return NULL;
}

void
e_mixer_system_del(E_Mixer_System * self __UNUSED__)
{
}

int
e_mixer_system_callback_set(E_Mixer_System * self,
			 int (*func) (void *data, E_Mixer_System * self) __UNUSED__,
			 void *data __UNUSED__)
{

  if (!self)
    return 0;

  return 1;
}

Eina_List      *
e_mixer_system_get_cards(void)
{
  _e_mixer_openbsd_set();

  return eina_list_append(NULL, _name);
}

void
e_mixer_system_free_cards(Eina_List * cards)
{
  eina_list_free(cards);
}

const char     *
e_mixer_system_get_default_card(void)
{
  _e_mixer_openbsd_set();

  return eina_stringshare_ref(_name);
}

const char     *
e_mixer_system_get_card_name(const char *card)
{
  _e_mixer_openbsd_set();

  if (card == _name || strcmp(card, _name) == 0)
    return eina_stringshare_ref(_name);
  else
    return NULL;
}

Eina_List      *
e_mixer_system_get_channels(E_Mixer_System * self __UNUSED__)
{
  return eina_list_append(NULL, (void *) -2);
}

void
e_mixer_system_free_channels(Eina_List * channels)
{
  eina_list_free(channels);
}

Eina_List      *
e_mixer_system_get_channels_names(E_Mixer_System * self __UNUSED__)
{
  _e_mixer_openbsd_set();

  return eina_list_append(NULL, _name);
}

void
e_mixer_system_free_channels_names(Eina_List * channels_names)
{
  eina_list_free(channels_names);
}

const char     *
e_mixer_system_get_default_channel_name(E_Mixer_System * self __UNUSED__)
{
  _e_mixer_openbsd_set();

  return eina_stringshare_ref(_name);
}

E_Mixer_Channel *
e_mixer_system_get_channel_by_name(E_Mixer_System * self __UNUSED__, const char *name)
{
  _e_mixer_openbsd_set();

  if (name == _name || strcmp(name, _name) == 0)
    return (E_Mixer_Channel *) - 2;
  else
    return NULL;
}

void
e_mixer_system_channel_del(E_Mixer_Channel * channel __UNUSED__)
{
}

const char     *
e_mixer_system_get_channel_name(E_Mixer_System * self __UNUSED__, E_Mixer_Channel * channel)
{
  if (channel == (E_Mixer_Channel *) - 2)
    return eina_stringshare_ref(_name);
  else
    return NULL;
}

#include <sys/types.h>
#include <sys/ioctl.h>
#ifdef __OpenBSD__
#include <sys/audioio.h>
#endif
#include <string.h>
#include <math.h>

static int
_openbsd_set_master_volumes(int vleft, int vright)
{
  int             i = 0;
  int             ndev = 0;
  mixer_devinfo_t dinfo;

  mixer_ctrl_t   *values = NULL;
  mixer_devinfo_t *infos = NULL;

  int             fd = open("/dev/mixer", O_RDWR);
  if (fd < 0)
    return 0;

  for (ndev = 0;; ndev++) {
    dinfo.index = ndev;
    if (ioctl(fd, AUDIO_MIXER_DEVINFO, &dinfo))
      break;
  }

  infos = calloc(ndev, sizeof(*infos));
  if (!infos)
    return 0;

  for (i = 0; i < ndev; i++) {
    infos[i].index = i;
    if (ioctl(fd, AUDIO_MIXER_DEVINFO, &infos[i]) < 0) {
      --ndev;
      --i;
      continue;
    }
  }

  values = calloc(ndev, sizeof(*values));
  if (!values)
    return 0;

  for (i = 0; i < ndev; i++) {
    values[i].dev = i;
    values[i].type = infos[i].type;
    if (infos[i].type != AUDIO_MIXER_CLASS) {
      values[i].un.value.num_channels = 2;
      if (ioctl(fd, AUDIO_MIXER_READ, &values[i]) < 0) {
        return 0;
      }
    }
  }

  char            name[64] = {0};
  for (i = 0; i < ndev; i++) {
    strlcpy(name, infos[i].label.name, 64);
    if (!strcmp(name, "master")) {
      values[i].un.value.level[0] = vleft;
      values[i].un.value.level[1] = vright;
      if (ioctl(fd, AUDIO_MIXER_WRITE, &values[i]) < 0) {
        return 0;
      }
      break;
    }
  }

  close(fd);

  if (values)
    free(values);
  if (infos)
    free(infos);

  return 1;
}

static int
_openbsd_get_master_volumes(int *lvalue, int *rvalue)
{
  int             i = 0;
  int             ndev = 0;
  mixer_devinfo_t dinfo;

  mixer_ctrl_t   *values = NULL;
  mixer_devinfo_t *infos = NULL;

  int             fd = open("/dev/mixer", O_RDWR);
  if (fd < 0)
    return 0;

  for (ndev = 0;; ndev++) {
    dinfo.index = ndev;
    if (ioctl(fd, AUDIO_MIXER_DEVINFO, &dinfo))
      break;
  }

  infos = calloc(ndev, sizeof(*infos));
  if (!infos)
    return 0;

  for (i = 0; i < ndev; i++) {
    infos[i].index = i;
    if (ioctl(fd, AUDIO_MIXER_DEVINFO, &infos[i]) < 0) {
      --ndev;
      --i;
      continue;
    }
  }

  values = calloc(ndev, sizeof(*values));
  if (!values)
    return 0;

  for (i = 0; i < ndev; i++) {
    values[i].dev = i;
    values[i].type = infos[i].type;
    if (infos[i].type != AUDIO_MIXER_CLASS) {
      values[i].un.value.num_channels = 2;
      if (ioctl(fd, AUDIO_MIXER_READ, &values[i]) < 0) {
        values[i].un.value.num_channels = 1;
        if (ioctl(fd, AUDIO_MIXER_READ, &values[i]) < 0)
          return 0;
      }
    }
  }

  char            name[64] = {0};
  for (i = 0; i < ndev; i++) {
    strlcpy(name, infos[i].label.name, 64);
    if (!strcmp(name, "master")) {
      *lvalue = values[i].un.value.level[0];
      *rvalue = values[i].un.value.level[1];
      break;
    }
  }
  close(fd);

  if (values)
    free(values);
  if (infos)
    free(infos);

  return 1;
}

int
e_mixer_system_get_volume(E_Mixer_System * self, E_Mixer_Channel * channel, int *left, int *right)
{
  (void) self;
  (void) channel;

  int             l = 0;
  int             r = 0;

  _openbsd_get_master_volumes(&l, &r);

  double          avg = (255.0 / 100);

  double          a_left = l / avg;
  double          a_right = r / avg;

  *left = round(a_left);
  *right = round(a_right);

  return 1;
}

int
e_mixer_system_set_volume(E_Mixer_System * self, E_Mixer_Channel * channel, int left, int right)
{
  (void) self;
  (void) channel;
  double          avg = (255.0 / 100);

  double          d_left = avg * left;
  double          d_right = avg * right;

  unsigned int    l_val = round(d_left);
  unsigned int    r_val = round(d_right);

  _openbsd_set_master_volumes(l_val, r_val);

  return 1;
}


int
_openbsd_get_mute(int *is_muted)
{
  int             i = 0;
  int             ndev = 0;
  mixer_devinfo_t dinfo;

  mixer_ctrl_t   *values = NULL;
  mixer_devinfo_t *infos = NULL;

  int             fd = open("/dev/mixer", O_RDWR);
  if (fd < 0)
    return 0;

  for (ndev = 0;; ndev++) {
    dinfo.index = ndev;
    if (ioctl(fd, AUDIO_MIXER_DEVINFO, &dinfo))
      break;
  }

  infos = calloc(ndev, sizeof(*infos));
  if (!infos)
    return 0;

  for (i = 0; i < ndev; i++) {
    infos[i].index = i;
    if (ioctl(fd, AUDIO_MIXER_DEVINFO, &infos[i]) < 0) {
      --ndev;
      --i;
      continue;
    }
  }

  values = calloc(ndev, sizeof(*values));
  if (!values)
    return 0;

  for (i = 0; i < ndev; i++) {
    values[i].dev = i;
    values[i].type = infos[i].type;
    if (infos[i].type != AUDIO_MIXER_CLASS) {
      values[i].un.value.num_channels = 2;
      if (ioctl(fd, AUDIO_MIXER_READ, &values[i]) < 0) {
        values[i].un.value.num_channels = 1;
        if (ioctl(fd, AUDIO_MIXER_READ, &values[i]) < 0)
          return 0;
      }
    }
  }

  char            name[64] = {0};
  for (i = 0; i < ndev; i++) {
    strlcpy(name, infos[i].label.name, 64);
    if (!strcmp(name, "toggle")) {
     *is_muted = values[i].un.value.level[0];
      break;
    }
  }
  close(fd);

  if (values)
    free(values);
  if (infos)
    free(infos);

  return 1;
}

int
e_mixer_system_can_mute(E_Mixer_System * self __UNUSED__, E_Mixer_Channel * channel __UNUSED__)
{
  //int m = 0;
  return 0; // _openbsd_get_mute(&m);
}

int
e_mixer_system_get_mute(E_Mixer_System * self __UNUSED__, E_Mixer_Channel * channel __UNUSED__, int *mute)
{ 
  (void) mute;
  return 0; // _openbsd_get_mute(mute);
}

int
e_mixer_system_set_mute(E_Mixer_System * self __UNUSED__, E_Mixer_Channel * channel __UNUSED__, int mute __UNUSED__)
{
  return 0;
}

int
e_mixer_system_get_state(E_Mixer_System * self __UNUSED__, E_Mixer_Channel * channel __UNUSED__, E_Mixer_Channel_State * state)
{

  e_mixer_system_get_volume(self, channel, &state->left, &state->right);
  state->mute = 0;

  return 1;
}

int
e_mixer_system_set_state(E_Mixer_System * self, E_Mixer_Channel * channel, const E_Mixer_Channel_State * state)
{
  int r;

  if (!state)
    return 0;

  r = e_mixer_system_set_mute(self, channel, state->mute);
  r &= e_mixer_system_set_volume(self, channel, state->left, state->right);
  
  return 1;
}

int
e_mixer_system_has_capture(E_Mixer_System * self __UNUSED__, E_Mixer_Channel * channel __UNUSED__)
{
  return 0;
}

#endif
