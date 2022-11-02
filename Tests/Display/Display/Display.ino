#include "displaylib.h"

void setup(void) {
  display_setup();
}

void loop(void) {
  delay(10000);
  
  //display_RPM(random(40));

  //display_speed(random(60));
  
  display_temp(random(40));

  display_timer(random(240));
  
  display_battery(random(100));

}
