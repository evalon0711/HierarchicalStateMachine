/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */

#include "watch.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main() {

  /* creates an test where the date is adapted. */
  test_HSM_Watch();


  return HSM_Watch();
  /* return HSM_NewStateMachine(); */
}
