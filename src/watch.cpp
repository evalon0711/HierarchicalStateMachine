
// ----------------------------------------------------------------------------------------
// CPP file definitions
// ----------------------------------------------------------------------------------------

#include "watch.h"
#include <assert.h>
#include <stdio.h>
#include "hsm.h"

// ---  Watch class individual functions  ---
void Watch::showTime() {
  printf("time: %2d:%02d:%02d", thour, tmin, tsec);
}

void Watch::showDate() {
  // todo year is missing
  printf("date: %02d-%02d-%04d", dday, dmonth, dyear);
}

void Watch::tick() {
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

/* TBD: Watch_TICK_EVT, can be used, but makes confsion. Usually first state should be named for user.  */
Msg const *Watch::topHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&state_setting);
    printf("Watch::topHndlr::STATE_START;\n");

    return cEventIsProcessed;
  case Watch_TICK_EVT:
    if (++tsec == cMinutesInHour)
      tsec = cReset0;
    printf("Watch::top-TICK;");
    showTime();
    return cEventIsProcessed;
  } 
  return msg;
}
/* 
Upon return to timekeeping mode the watch displays the most recently selected information, that is, 
if date was selected prior to leaving timekeeping mode, the watch resumes displaying the date, 
otherwise it displays the current time.
*/
Msg const *Watch::timekeepingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(state_timekeepingHist);
    return cEventIsProcessed;
  case Watch_SET_EVT:
    STATE_TRAN(&state_setting);
    printf("Watch::timekeeping-SET;\n");
    return cEventIsProcessed;
  } 
  return msg;
}

/* time Handler covers all events 

*/


Msg const *Watch::timeHndlr(Msg const *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    showTime();
    return cEventIsProcessed;
  case Watch_MODE_EVT:
    STATE_TRAN(&ss_date);
    printf("Watch::go to show date\n");        
    return cEventIsProcessed;
  case Watch_TICK_EVT:
    printf("Watch::time-TICK;\n");        
    tick();
    showTime();
    return cEventIsProcessed;
  } 
  return msg;
}


/*  */
Msg const *Watch::dateHndlr(Msg const *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    showDate();
    return cEventIsProcessed;
  case Watch_MODE_EVT:
    STATE_TRAN(&ss_time);
    printf("Watch::go to show time\n");        
    return cEventIsProcessed;
  
  case Watch_TICK_EVT:
    printf("Watch::date-TICK;\n");        
    tick();
    showDate();
    return cEventIsProcessed; 
  } 
  return msg;
}


/*  */
Msg const *Watch::settingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&ss_hour);
    return cEventIsProcessed;

  } 
  return msg;
}


/*  */
Msg const *Watch::hourHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_minute);
    printf("Watch::go to hour change");
    return cEventIsProcessed;
  case Watch_MODE_EVT:
    if (++thour == cHoursOnDay)
        thour = cReset0;
    printf("Watch::hour-SET: hour++: %d", thour);
    return cEventIsProcessed; 
    /* if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns
     (“throws”)  the  message  for  further processing by higher-level states.  */
  } 
  /* While in setting mode the watch ignores tick events */
  return msg;
}


/*  */
Msg const *Watch::minuteHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_day);
    printf("Watch:: go to day changing");
    return cEventIsProcessed;
  case Watch_MODE_EVT:
    if (++tmin == cMinutesInHour)
        tmin = cReset0;
    printf("Watch::min-SET: min++: %d", tmin);
    return cEventIsProcessed; //todo clarify which number shall be used as return value
  } 
  /* While in setting mode the watch ignores tick events */
  return msg;
}


/*  */
Msg const *Watch::dayHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_month);
    printf("Watch:: go to month ");
    return cEventIsProcessed;
  case Watch_MODE_EVT:
    if (++dday == Watch::cDaysPerMonth[dmonth-1]+1) 
      dday = 1;
    printf("Watch::day-SET: day++: %d", dday);
    return cEventIsProcessed; //todo clarify which number shall be used as return value
  }
  /* While in setting mode the watch ignores tick events */
  return msg;
}

/*  */
Msg const *Watch::monthHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the watch back into timekeeping mode. */
    STATE_TRAN(&state_timekeeping);
    printf("Watch:: go back to timekeeping");
    return cEventIsProcessed;
  case Watch_MODE_EVT:
    if (++dmonth == cMonthInYear+1) 
            dmonth = 1;
    printf("Watch::month-SET: month++: %d", dmonth);
    return cEventIsProcessed; 
  } 
  /* While in setting mode the watch ignores tick events */
  return msg;
}

/* todo year Handler where the year can be set. */


/* Watch Constructor 
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
Watch::Watch() 
: Hsm("Watch", (EvtHndlr)topHndlr),
  //  State
  state_timekeeping("timekeeping", &top, (EvtHndlr)&Watch::timekeepingHndlr),
  // substates
  ss_time("time", &state_timekeeping, (EvtHndlr)&Watch::timeHndlr),
  ss_date("date", &state_timekeeping, (EvtHndlr)&Watch::dateHndlr),
  // State
  state_setting("setting", &top, (EvtHndlr)&Watch::settingHndlr),
  // substates
  ss_hour("hour", &state_setting, (EvtHndlr)&Watch::hourHndlr),
  ss_minute("minute", &state_setting, (EvtHndlr)&Watch::minuteHndlr),
  ss_day("day", &state_setting, (EvtHndlr)&Watch::dayHndlr),
  ss_month("month", &state_setting, (EvtHndlr)&Watch::monthHndlr),
  // define members
  tsec(cReset0), tmin(cReset0), thour(cReset0), dday(1), dmonth(1)
{
  state_timekeepingHist = &ss_time; 
}




