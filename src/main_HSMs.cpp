/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */

#include "watch.h"
#include "car.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main() {

  /* -------------------------------------------------------------------------------------------- */

  /* creates an test where the date is adapted. */
  // test_HSM_Watch();
  // return HSM_Watch();

  /* -------------------------------------------------------------------------------------------- */
  std::vector<int> car_off_reverse_off = { Car::SET,Car::SET,Car::SET,Car::SET,Car::REVERSE,Car::SET,Car::BACK,Car::BACK,Car::BACK};
  std::string car_off_reverse_off_result="You should see the reverse gear and that the car is off again.";
  test_HSM_Car(car_off_reverse_off,car_off_reverse_off_result);

  std::vector<int> car_off_drive2_off = { Car::SET,Car::SET,Car::SET,Car::SET,/*drive-idle*/Car::SET,Car::SET,Car::BACK,Car::BACK,Car::BACK,Car::BACK};
  std::string car_off_drive2_off_result="You should see the drive2 and that the car is off again.";

  test_HSM_Car(car_off_drive2_off,car_off_drive2_off_result);

  
  return HSM_Car();


  /* return HSM_NewStateMachine(); */
}
