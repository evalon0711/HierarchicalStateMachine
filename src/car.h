/**
 * Simple digital car example
 * k.hiltl 21/04/2022
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
We apply the HSM pattern according to the following recipe:
1. Declare a new class, inheriting from Hsm class (the Car class) 
2. Put into this new class all states (State class instances) and other attributes (tsec, tmin, thour, and so on)
3. Declare an event handler method (member function) for every state.
   Don’t forget to declare event handlers for inherited states, like top, 
   whose behavior you intend to customize
4. Define the state machine topology (nesting of states) in the new class (the Car class) constructor
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


class Car : public Hsm {
 // date parameters
  unsigned int tsec, tmin, thour, dday, dmonth, dyear=2018;

protected:
  State state_off;

  State state_on;
  // substates of setting
    State ss_hour, ss_minute, ss_day, ss_month;

  // 
  State *state_timekeepingHist;

public:
  Car();
  /* All Transitions have to defined and created for the state machine. */
  /* Typically this is achieved using a single-level switch statement. Event handlers  communicate  with  the  state
     machine  engine through  a  return  value  of  type  Msg*.
     The semantic is simple: if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns
     (“throws”)  the  message  for  further processing by higher-level states. 
     To be compliant  with  UML  statecharts,  the returned  message  is  the  same  as  the received message, although return of
     a  different  message  type  can  be  considered. As we discuss later, returning
     the  message  provides  a  mechanism similar to “throwing” exceptions.  */

  /* declare event handlers for inherited states, like top, whose behavior you intend to customize */
  Msg const *topHndlr(Msg const *msg);  
  /* state_off */
  Msg const *state_offHndlr(Msg const *msg);  
  
  // Msg const *timeHndlr(Msg const *msg);  
  /* for substate date */
  // Msg const *dateHndlr(Msg const *msg);  
  /* for state setting */
  Msg const *settingHndlr(Msg const *msg);  
  /* for substate hour */
  Msg const *hourHndlr(Msg const *msg);  
  /* for substate minute */
  Msg const *minuteHndlr(Msg const *msg);  
  /* for substate minute */
  Msg const *dayHndlr(Msg const *msg);  
  /* for substate month */
  Msg const *monthHndlr(Msg const *msg);  

  /* Standard functions, to show behaviour */
  void tick();
  void showTime();
  void showDate();
  
  /* Consts */
  static constexpr int MODE=0;
  static constexpr int SET=1;
  static constexpr int TICK=2;

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
enum CarEvents {
  Car_MODE_EVT,/* Adjustments are made by pressing the “mode” button, which increments the chosen quantity by one. */
  Car_SET_EVT, /* Pressing the “set” button switches the car into setting mode.  */
  Car_TICK_EVT /*  */
};

};

/* Εvents */
const Msg carMsg[] = { 
  Car::CarEvents::Car_MODE_EVT, // Button
  Car::CarEvents::Car_SET_EVT, // Button
  Car::CarEvents::Car_TICK_EVT // trigger of seconds, done manually.

/*  Pressing the “set” button switches
the car into setting mode. 

The sequence of adjustments in this
mode is: hour, minute, day, month.

Adjustments are made by pressing
the “mode” button, which incre-
ments the chosen quantity by one.

Pressing the “set” button while
adjusting month puts the car
back into timekeeping mode

• While in setting mode the car
ignores tick events

• Upon return to timekeeping mode
the car displays the most recently selected information, that is, if
date was selected prior to leaving
timekeeping mode, the car
resumes displaying the date, other-
wise it displays the current time */
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  Create and run the HSM Car as a static function, 
  which shall be placed in main function
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int HSM_Car(void)
{
Car car;         
  car.onStart();
  printf("\nThe sequence of adjustments in this mode is: hour, minute, day, month.\n\n");
  while (1)  {
    int i;
    printf("\nEvent[0=mode,1=set,2=tick]->");
    scanf("%d", &i);
    if (i < 0 || sizeof(carMsg)/sizeof(Msg) <= i) 
      break;
    car.onEvent(&carMsg[i]);     
  }
  printf("\n\nCar Application finished.Exit.\n\n");
  return 0;

}

/* set the date automatically to test if statemachine works */
static int test_HSM_Car(int hour=7,
                          int minute=7,
                          int day=5,
                          int month=12
)
{

  Car car;         
  car.onStart();

  printf("\n");
  
  for (int h=0; h<hour; h++)
  {
    car.onEvent(&carMsg[Car::MODE]); printf("\n");
  }
  car.onEvent(&carMsg[Car::SET]); printf("\n");

  for (int m=0; m<minute; m++)
  {
    car.onEvent(&carMsg[Car::MODE]); printf("\n");
  }
  car.onEvent(&carMsg[Car::SET]); printf("\n");

  
  for (int m=0; m<day-1; m++)
  {
    car.onEvent(&carMsg[Car::MODE]); printf("\n");
  }
  car.onEvent(&carMsg[Car::SET]); printf("\n");

  for (int m=0; m<month-1; m++)
  {
    car.onEvent(&carMsg[Car::MODE]); printf("\n");
  }
  // leave setting status
  car.onEvent(&carMsg[Car::SET]); printf("\n");
  car.onEvent(&carMsg[Car::TICK]);printf("\n");
  // show date by clicking mode button
  car.onEvent(&carMsg[Car::MODE]); printf("\n");

  // printf("\nThe sequence of adjustments in this mode is: hour, minute, day, month.\n\n");
  printf("\n\nThis is the test function, if 05-12-2018 07:07:01 succeed \n\n");
  printf("==============================================================\n\n\n\n");

  /*  
    here would be the while (1) loop
  */
  return 0;
}



