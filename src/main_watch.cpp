/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */

#include "watch.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main() {
  Watch watch;         
  watch.onStart();
  printf("\nThe sequence of adjustments in this mode is: hour, minute, day, month.\n\n");
  for (;;)  {
    int i;
    printf("\nEvent[0=mode,1=set,2=tick]->");
    scanf("%d", &i);
    if (i < 0 || sizeof(watchMsg)/sizeof(Msg) <= i) 
      break;
    watch.onEvent(&watchMsg[i]);     
  }
  printf("\n\nWatch Application finished.Exit.\n\n");
  return 0;
}
