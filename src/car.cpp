
// ----------------------------------------------------------------------------------------
// CPP file definitions
// ----------------------------------------------------------------------------------------

#include "car.h"
#include <assert.h>
#include <stdio.h>
#include "hsm.h"

// ---  Car class individual functions  ---
void Car::showTime() {
  printf("time: %2d:%02d:%02d", thour, tmin, tsec);
}

void Car::showDate() {
  // todo year is missing
  printf("date: %02d-%02d-%04d", dday, dmonth, dyear);
}

void Car::tick() {
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
Msg const *Car::topHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&state_on);
    printf("Car::topHndlr::STATE_START;\n");

    return cEventIsProcessed;
  case Car_TICK_EVT:
    if (++tsec == cMinutesInHour)
      tsec = cReset0;
    printf("Car::top-TICK;");
    showTime();
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
  case START_EVT:
    STATE_START(state_History);
    return cEventIsProcessed;
  case Car_SET_EVT:
    STATE_TRAN(&state_on);
    printf("Car::timekeeping-SET;\n");
    return cEventIsProcessed;
  } 
  return msg;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
General: Handler definition for each state, covers all events 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*  */
Msg const *Car::settingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&ss_park);
    return cEventIsProcessed;

  } 
  return msg;
}


/*  */
Msg const *Car::hourHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_SET_EVT:
    STATE_TRAN(&ss_drive);
    printf("Car::go to hour change");
    return cEventIsProcessed;
  case Car_MODE_EVT:
    if (++thour == cHoursOnDay)
        thour = cReset0;
    printf("Car::hour-SET: hour++: %d", thour);
    return cEventIsProcessed; 
    /* if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns
     (“throws”)  the  message  for  further processing by higher-level states.  */
  } 
  /* While in setting mode the car ignores tick events */
  return msg;
}


/*  */
Msg const *Car::minuteHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_SET_EVT:
    // STATE_TRAN(&ss_day);
    printf("Car:: go to day changing");
    return cEventIsProcessed;
  case Car_MODE_EVT:
    if (++tmin == cMinutesInHour)
        tmin = cReset0;
    printf("Car::min-SET: min++: %d", tmin);
    return cEventIsProcessed; //todo clarify which number shall be used as return value
  } 
  /* While in setting mode the car ignores tick events */
  return msg;
}


/*  */
Msg const *Car::dayHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_SET_EVT:
    // STATE_TRAN(&ss_month);
    printf("Car:: go to month ");
    return cEventIsProcessed;
  case Car_MODE_EVT:
    if (++dday == Car::cDaysPerMonth[dmonth-1]+1) 
      dday = 1;
    printf("Car::day-SET: day++: %d", dday);
    return cEventIsProcessed; //todo clarify which number shall be used as return value
  }
  /* While in setting mode the car ignores tick events */
  return msg;
}

/*  */
Msg const *Car::monthHndlr(Msg const *msg) {
  switch (msg->evt) {
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
  /* While in setting mode the car ignores tick events */
  return msg;
}

/* todo year Handler where the year can be set. */


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
  state_on("ON", &top, (EvtHndlr)&Car::settingHndlr),
  // substates
  ss_park("ON:PARK", &state_on, (EvtHndlr)&Car::hourHndlr),
  ss_drive("ON:DRIVE", &state_on, (EvtHndlr)&Car::minuteHndlr),
  drive_ss_idle("ON:DRIVE:drive_ss_idle", &state_on, (EvtHndlr)&Car::minuteHndlr),
  drive_ss_drive1("ON:DRIVE:drive_ss_drive1", &state_on, (EvtHndlr)&Car::minuteHndlr),
  drive_ss_reverseGear("ON:DRIVE:drive_ss_reverseGear", &state_on, (EvtHndlr)&Car::minuteHndlr),

  // ss_day("day", &state_on, (EvtHndlr)&Car::dayHndlr),
  // ss_month("month", &state_on, (EvtHndlr)&Car::monthHndlr),
  // define members
  tsec(cReset0), tmin(cReset0), thour(cReset0), dday(1), dmonth(1)
{
  state_History = &state_off; 
}




