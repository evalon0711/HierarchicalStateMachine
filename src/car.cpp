
// ----------------------------------------------------------------------------------------
// CPP file definitions
// ----------------------------------------------------------------------------------------

#include "car.h"
#include <assert.h>
#include <stdio.h>
#include "hsm.h"

/* Car Constructor 
  The
constructor  of  the  concrete  HSM  is
responsible for initialization of all par-
ticipating State objects by setting the
super-state pointers and the event han-
dlers.
 In our approach we
do not distinguish between composite-
states (states containing substates) and
leaf  states.  All  states  are  potentially
composite.


*/
Car::Car() 
: Hsm("Car", (EvtHndlr)topHndlr),
  //  State
  state_off("OFF", &top, (EvtHndlr)&Car::state_offHndlr),
  // State
  state_on("ON", &top, (EvtHndlr)&Car::state_onHndlr),
  // substates
  ss_park("ON:PARK", &state_on, (EvtHndlr)&Car::ss_parkHndlr),
  ss_drive("ON:DRIVE", &state_on, (EvtHndlr)&Car::ss_driveHndlr),
  // substates of drive
  // todo is it possible to get an additional level down? 
  drive_ss_idle("ON:DRIVE:drive_ss_idle", &ss_drive, (EvtHndlr)&Car::drive_ss_idleHndlr),
  drive_ss_drive1("ON:DRIVE:drive_ss_drive1", &ss_drive, (EvtHndlr)&Car::drive_ss_drive1Hndlr),
  drive_ss_reverseGear("ON:DRIVE:drive_ss_reverseGear", &ss_drive, (EvtHndlr)&Car::drive_ss_reverseGearHndlr),

  // define members
  tsec(cReset0), tmin(cReset0), thour(cReset0), dday(1), dmonth(1)
{
  state_History = &state_off; 
  // state_History = &state_on; //make: *** [Makefile:57: execute] Segmentation fault
}



// ---  Car class individual functions  ---
void Car::individualFunction1() {
  printf("time: %2d:%02d:%02d", thour, tmin, tsec);
}

void Car::individualFunction() {
  // todo year is missing
  printf("date: %02d-%02d-%04d", dday, dmonth, dyear);
}

void Car::individualFunction3() {
  /* with each call the time is incremented by one second.  */

  if (++tsec == cSecondsInMinute) {
    tsec = cReset0;
    if (++tmin == cMinutesInHour) {
      tmin = cReset0;
      if (++thour == cHoursOnDay) {
        thour = cReset0;
        if (++dday == cDaysPerMonth[dmonth-1]+1) {
          dday = 1;
          if (++dmonth == cMonthInYear+1) 
            dmonth = 1;
            /* todo increase year, todo add year as well */
        }
      }
    }
  }
}

/*  */

/* TBD: Car_TICK_EVT, can be used, but makes confsion. Usually first state should be named for user.  */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
General: Handler definition for each state, covers all events 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Msg const *Car::topHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_STATUS_EVT:
    printf("Car::top-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case START_EVT:
    printf("Car::topHndlr::STATE_START;\n\tTOP -> OFF\n");
    STATE_START(&state_off);
    return cEventIsProcessed;
  } 
  return msg;
}
/* 
Upon return to timekeeping mode the car displays the most recently selected information, that is, 
if date was selected prior to leaving timekeeping mode, the car resumes displaying the date, 
otherwise it displays the current time.
*/
Msg const *Car::state_offHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_STATUS_EVT:
    printf("Car::off-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case START_EVT:
    // STATE_START(state_History);
    return cEventIsProcessed;
  case Car_SET_EVT:
    printf("OFF -> ON\n");
    STATE_TRAN(&state_on);
    return cEventIsProcessed;
  case Car_MODE_EVT:
    printf("OFF: already off\n");
    return cEventIsProcessed; 

  } 
  return msg;
}



/*  */
Msg const *Car::state_onHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_SET_EVT:
    printf("ON -> PARK\n");
    STATE_TRAN(&ss_park);
    return cEventIsProcessed;
  case Car_STATUS_EVT:
    printf("Car::ON-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_MODE_EVT:
    printf("ON -> OFF");
    STATE_TRAN(&state_off);
    return cEventIsProcessed; 
  
    /* if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns
     (“throws”)  the  message  for  further processing by higher-level states.  */
  } 
  /* While in setting mode the car ignores tick events */
  return msg;
}


/*  */
Msg const *Car::ss_parkHndlr(Msg const *msg) {
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::PARK-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case Car_SET_EVT:
    printf("PARK -> DRIVE");
    STATE_TRAN(&ss_drive);
    return cEventIsProcessed;
  case Car_MODE_EVT:
    printf("PARK -> OFF");
    STATE_TRAN(&state_off);
    return cEventIsProcessed; 
  } 
  /* While in setting mode the car ignores tick events */
  return msg;
}


/*  */
Msg const *Car::ss_driveHndlr(Msg const *msg) {
  
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::DRIVE-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case Car_SET_EVT:
    printf("DRIVE");
    // STATE_TRAN(&ss_month);
    return cEventIsProcessed;
  case Car_MODE_EVT:
    printf("DRIVE -> PARK\n");
    STATE_TRAN(&ss_park);
    return cEventIsProcessed; 
  }
  /* While in setting mode the car ignores tick events */
  return msg;
}




/*  */
Msg const *Car::drive_ss_idleHndlr(Msg const *msg) {
  
  #ifdef LATER
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::IDLE-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case Car_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the car back into timekeeping mode. */
    STATE_TRAN(&state_off);
    printf("Car:: go back to timekeeping");
    return cEventIsProcessed;
  case Car_MODE_EVT:
    if (++dmonth == cMonthInYear+1) 
            dmonth = 1;
    printf("Car::month-SET: month++: %d", dmonth);
    return cEventIsProcessed; 
  } 
  #endif
  /* While in setting mode the car ignores tick events */
  return msg;
}

Msg const *Car::drive_ss_drive1Hndlr(Msg const *msg) {
  #ifdef LATER
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::DRIVE1-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case Car_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the car back into timekeeping mode. */
    STATE_TRAN(&state_off);
    printf("Car:: go back to timekeeping");
    return cEventIsProcessed;
  case Car_MODE_EVT:
    if (++dmonth == cMonthInYear+1) 
            dmonth = 1;
    printf("Car::month-SET: month++: %d", dmonth);
    return cEventIsProcessed; 
  } 
  #endif
  /* While in setting mode the car ignores tick events */
  return msg;
}


Msg const *Car::drive_ss_reverseGearHndlr(Msg const *msg) {
  #ifdef LATER
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::REVERSE-Handler\n");
    // individualFunction1();
    return cEventIsProcessed;
  case Car_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the car back into timekeeping mode. */
    STATE_TRAN(&state_off);
    printf("Car:: go back to timekeeping");
    return cEventIsProcessed;
  case Car_MODE_EVT:
    if (++dmonth == cMonthInYear+1) 
            dmonth = 1;
    printf("Car::month-SET: month++: %d", dmonth);
    return cEventIsProcessed; 
  } 
  #endif
  /* While in setting mode the car ignores tick events */
  return msg;
}



/* todo year Handler where the year can be set. */






