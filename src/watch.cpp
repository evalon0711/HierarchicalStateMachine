/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
To illustrate the use of the HSM pattern,  consider  a  simple  digital  watch.  
The  watch  has  two  buttons—which  generate  external events—and  an  internally  generated
tick event.  
The  different  events  are handled  differently  depending  upon the  mode  of  operation.  
The  basic watch operates as follows:

• In timekeeping mode, the user can toggle  between  displaying  date  or  current  time  by  pressing  the
“mode” button
• Pressing  the  “set”  button  switches  the watch into setting mode. The sequence  of  adjustments  in  this
mode is: hour, minute, day, month.
Adjustments are made by pressing the  “mode”  button,  which  increments the chosen quantity by one.
Pressing  the  “set”  button  while adjusting  month  puts  the  watch back into timekeeping mode
• While  in  setting mode  the  watch ignores tick events
• Upon return to timekeeping mode the watch displays the most recently  selected  information,  that  is,  if
date  was  selected  prior  to  leaving timekeeping mode,  the  watch resumes displaying the date, otherwise 
it displays the current time
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
We apply the HSM pattern according to the following recipe:
1. Declare a new class, inheriting from Hsm class (the Watch class) 
2. Put into this new class all states (State class instances) and other attributes (tsec, tmin, thour, and so on)
3. Declare an event handler method (member function) for every state.
   Don’t forget to declare event handlers for inherited states, like top, 
   whose behavior you intend to customize
4. Define the state machine topology (nesting of states) in the new class (the Watch class) constructor
5. Define events for the state machine (for example, as enumeration).
   You can use event-types starting from 0, because the pre-defined events use the upper limit of the Event type range.
6. Define event handler methods.
   Code entry/exit actions and start-up transitions as response to pre-defined events ENTRY_EVT,EXIT_EVT, and START_EVT, 
   respectively. Provide code for other events using STATE_TRAN() macro for state transitions. Remember to return 0
   (NULL pointer) if you handle the event and the initial message pointer if you don’t
7. Execute the initial start transition by invoking Hsm::onStart()
8. Arrange to invoke Hsm::onEvent() for each incoming event
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <assert.h>
#include <stdio.h>
#include "hsm.h"


class Watch : public Hsm {
 // date parameters
  unsigned int tsec, tmin, thour, dday, dmonth;

protected:
  State state_timekeeping;
  // substates of timekeeping
    State ss_time, ss_date;

  State state_setting;
  // substates of setting
    State ss_hour, ss_minute, ss_day, ss_month;

  // 
  State *state_timekeepingHist;

public:
  Watch();
  /* All Transitions have to defined and created for the state machine. */
  /* Typically this is achieved using a single-level switch statement. Event handlers  communicate  with  the  state
     machine  engine through  a  return  value  of  type  Msg*.
     The semantic is simple: if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns
     (“throws”)  the  message  for  further processing by higher-level states. 
     To be compliant  with  UML  statecharts,  the returned  message  is  the  same  as  the received message, although return of
     a  different  message  type  can  be  considered. As we discuss later, returning
     the  message  provides  a  mechanism similar to “throwing” exceptions.  */
  Msg const *topHndlr(Msg const *msg);  
  Msg const *timekeepingHndlr(Msg const *msg);  
  Msg const *timeHndlr(Msg const *msg);  
  Msg const *dateHndlr(Msg const *msg);  
  Msg const *settingHndlr(Msg const *msg);  
  Msg const *hourHndlr(Msg const *msg);  
  Msg const *minuteHndlr(Msg const *msg);  
  Msg const *dayHndlr(Msg const *msg);  
  Msg const *monthHndlr(Msg const *msg);  

  /* Standard functions, to show behaviour */
  void tick();
  void showTime();
  void showDate();

private:

  static constexpr unsigned int cHoursOnDay=24;
  static constexpr unsigned int cMinutesInHour=60;
  static constexpr unsigned int cSecondsInMinute=60;
  static constexpr unsigned int cMonthInYear=12;
  static constexpr unsigned int cReset0=0;
  unsigned int const cDaysPerMonth[cMonthInYear]={/* Jan, Feb, ... ,Dez */31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  /* if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns (“throws”)  the  message  for  further processing by higher-level states. */
  const Msg* cEventIsProcessed=0;

 
};

// ----------------------------------------------------------------------------------------
// CPP file definitions
// ----------------------------------------------------------------------------------------


enum WatchEvents {
  Watch_MODE_EVT,/* Adjustments are made by pressing the “mode” button, which increments the chosen quantity by one. */
  Watch_SET_EVT, /* Pressing the “set” button switches the watch into setting mode.  */
  Watch_TICK_EVT /*  */
};

// ---  Watch class individual functions  ---
void Watch::showTime() {
  printf("time: %2d:%02d:%02d", thour, tmin, tsec);
}

void Watch::showDate() {
  // todo year is missing
  printf("date: %02d-%02d-0000", dday, dmonth);
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
    printf("Watch:: go to day chaning");
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

/* Εvents */
const Msg watchMsg[] = { 
  Watch_MODE_EVT, // Button
  Watch_SET_EVT, // Button
  Watch_TICK_EVT // trigger of seconds, done manually.

/*  Pressing the “set” button switches
the watch into setting mode. 

The sequence of adjustments in this
mode is: hour, minute, day, month.

Adjustments are made by pressing
the “mode” button, which incre-
ments the chosen quantity by one.

Pressing the “set” button while
adjusting month puts the watch
back into timekeeping mode

• While in setting mode the watch
ignores tick events

• Upon return to timekeeping mode
the watch displays the most recently selected information, that is, if
date was selected prior to leaving
timekeeping mode, the watch
resumes displaying the date, other-
wise it displays the current time */


};



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
  return 0;
}
