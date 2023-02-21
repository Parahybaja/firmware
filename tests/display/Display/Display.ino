#include "displaylib.h"

void setup(void) {
  display_setup();
  display_sponsors();
}

void loop(void) {
  delay(5000);
  
  display_RPM(random(40));

  display_speed(random(60));
  
  display_temp(random(40));

  display_timer(random(240));
  
  display_battery(random(100));

}
