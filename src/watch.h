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
  unsigned int tsec, tmin, thour, dday, dmonth, dyear=2018;

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

public:
enum WatchEvents {
  Watch_MODE_EVT,/* Adjustments are made by pressing the “mode” button, which increments the chosen quantity by one. */
  Watch_SET_EVT, /* Pressing the “set” button switches the watch into setting mode.  */
  Watch_TICK_EVT /*  */
};

};

/* Εvents */
const Msg watchMsg[] = { 
  Watch::WatchEvents::Watch_MODE_EVT, // Button
  Watch::WatchEvents::Watch_SET_EVT, // Button
  Watch::WatchEvents::Watch_TICK_EVT // trigger of seconds, done manually.

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  Create and run the HSM Watch as a static function, 
  which shall be placed in main function
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int HSM_Watch(void)
{
Watch watch;         
  watch.onStart();
  printf("\nThe sequence of adjustments in this mode is: hour, minute, day, month.\n\n");
  while (1)  {
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

/* set the date automatically to test if statemachine works */
static int test_HSM_Watch(int hour=7,
                          int minute=7,
                          int day=5,
                          int month=12
)
{

  Watch watch;         
  watch.onStart();

  constexpr int MODE=0;
  constexpr int SET=1;
  constexpr int TICK=2;
  printf("\n");
  // go to setting into hour
  // watch.onEvent(&watchMsg[MODE]);
  
  for (int h=0; h<hour; h++)
  {
    watch.onEvent(&watchMsg[MODE]); printf("\n");
  }
  watch.onEvent(&watchMsg[SET]); printf("\n");

  for (int m=0; m<minute; m++)
  {
    watch.onEvent(&watchMsg[MODE]); printf("\n");
  }
  watch.onEvent(&watchMsg[SET]);

  
  for (int m=0; m<day-1; m++)
  {
    watch.onEvent(&watchMsg[MODE]); printf("\n");
  }
  watch.onEvent(&watchMsg[SET]);

  for (int m=0; m<month-1; m++)
  {
    watch.onEvent(&watchMsg[MODE]); printf("\n");
  }
  watch.onEvent(&watchMsg[SET]);printf("\n");
  watch.onEvent(&watchMsg[TICK]);printf("\n");
    watch.onEvent(&watchMsg[MODE]); printf("\n");

  // printf("\nThe sequence of adjustments in this mode is: hour, minute, day, month.\n\n");
  printf("\n\nThis is the test function\n\n");
  while (1)  {
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



