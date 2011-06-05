#include <allegro5/allegro.h>
#include <string.h>
#include <stdio.h>
#include "global.h"
#include "gamepad.h"


static void read_config(VCONTROLLER * this, const char *config_path)
{
   int i;
   char tmp[64];

   ALLEGRO_CONFIG *c = al_load_config_file(config_path);
   if (!c) c = al_create_config();

   for (i = 0; i < 3; i++) {
      snprintf(tmp, sizeof(tmp), "button%d", i);
      ((int *)(this->private_data))[i] = get_config_int(c, "GAMEPAD", tmp, 0);

      if (((((int *)(this->private_data))[i] >> 8) & 255) >= al_get_num_joysticks())
         ((int *)(this->private_data))[i] = 0;
   }

   al_destroy_config(c);
}


static void write_config(VCONTROLLER * this, const char *config_path)
{
   int i;
   char tmp[64];

   ALLEGRO_CONFIG *c = al_load_config_file(config_path);
   if (!c) c = al_create_config();

   for (i = 0; i < 3; i++) {
      snprintf(tmp, sizeof(tmp), "button%d", i);
      set_config_int(c, "GAMEPAD", tmp, ((int *)(this->private_data))[i]);
   }

   al_save_config_file(config_path, c);
   al_destroy_config(c);
}


static void poll(VCONTROLLER * this)
{
   (void)this;
// FIXME
#if 0
   int i;

   int *private_data = (int *)(this->private_data);

   ALLEGRO_JOYSTICK_STATE joy = al_get_joystick_state();

   for (i = 0; i < 3; i++) {
      if (private_data[i] & 1) {
         /* read a button */
         this->button[i] =
            joy[(private_data[i] >> 8) & 255].button[private_data[i] >> 16].b;
      } else {
         /* read an axis */
         this->button[i] =
            (private_data[i] & 2) ?
            joy[(private_data[i] >> 8) & 255].
            stick[(private_data[i] >> 16) & 255].
            axis[(private_data[i] >> 24)].
            d1 : joy[(private_data[i] >> 8) & 255].
            stick[(private_data[i] >> 16) & 255].
            axis[(private_data[i] >> 24)].d2;
      }
   }
#endif
}


static int calibrate_button(VCONTROLLER * this, int i)
{
   (void)this;
   (void)i;
// FIXME
#if 0
   int stickc, axisc, joyc, buttonc;

   poll_joystick();
   joyc = al_get_num_joysticks();
   while (joyc--) {
      /* check axes */
      stickc = joy[joyc].num_sticks;
      while (stickc--) {
         axisc = joy[joyc].stick[stickc].num_axis;
         while (axisc--) {
            if (joy[joyc].stick[stickc].axis[axisc].d1) {
               ((int *)(this->private_data))[i] =
                  4 | 2 | (joyc << 8) | (stickc << 16) | (axisc << 24);
               return 1;
            }

            if (joy[joyc].stick[stickc].axis[axisc].d2) {
               ((int *)(this->private_data))[i] =
                  4 | (joyc << 8) | (stickc << 16) | (axisc << 24);
               return 1;
            }
         }
      }

      /* check buttons */
      buttonc = joy[joyc].num_buttons;
      while (buttonc--) {
         if (joy[joyc].button[buttonc].b) {
            ((int *)(this->private_data))[i] =
               4 | 1 | (joyc << 8) | (buttonc << 16);
            return 1;
         }
      }
   }
#endif
   return 0;
}

static const char *get_button_description(VCONTROLLER * this, int i)
{
   int *private_data = (int *)(this->private_data);
   static char RetMessage[1024];

   if (!private_data[i])
      return "Unassigned";

   if (private_data[i] & 1) {
      snprintf(RetMessage, sizeof(RetMessage), "Pad %d Button %d",
                (private_data[i] >> 8) & 255, private_data[i] >> 16);
      return RetMessage;
   } else {
      snprintf(RetMessage, sizeof(RetMessage),
                "Pad %d Stick %d Axis %d (%s)",
                (private_data[i] >> 8) & 255,
                (private_data[i] >> 16) & 255, private_data[i] >> 24,
                (private_data[i] & 2) ? "-" : "+");
      return RetMessage;
   }
}


VCONTROLLER *create_gamepad_controller(const char *config_path)
{
   int i;
   VCONTROLLER *ret = malloc(sizeof(VCONTROLLER));

   ret->private_data = malloc(3 * sizeof(int));
   for (i = 0; i < 3; i++) {
      ret->button[i] = 0;
      ((int *)(ret->private_data))[i] = 0;
   }
   ret->poll = poll;
   ret->calibrate_button = calibrate_button;
   ret->get_button_description = get_button_description;
   ret->read_config = read_config;
   ret->write_config = write_config;

   read_config(ret, config_path);

   return ret;
}