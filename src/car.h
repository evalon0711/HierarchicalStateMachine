/**
 * Simple digital car example
 * k.hiltl 21/04/2022
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
We apply the HSM pattern according to the following recipe:
1. Declare a new class, inheriting from Hsm class (the Car class) 
   done

2. Put into this new class all states (State class instances) and other attributes 
   done

3. Declare an event handler method (member function) for every state.
   Don’t forget to declare event handlers for inherited states, like top, 
   whose behavior you intend to customize.
   done


4. Define the state machine topology (nesting of states) in the new class (the Car class) constructor
   done


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
#include <string>
#include <vector>
#include <iostream>

class Car : public Hsm {
 // date parameters
  unsigned int tsec, tmin, thour, dday, dmonth, dyear=2018;

protected:
  State state_off;

  State state_on;
  //substates of state_on
    State ss_park;
    State ss_drive;
    //substates of substate drive = state(ON)-substate(DRIVE)-substate(idle, drive1, drive2, ReverseGear)
      State drive_ss_idle, drive_ss_drive1, drive_ss_drive2,  drive_ss_reverseGear;

  // 
  State *state_History;

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
  Msg const *state_offHndlr(Msg const *msg);  
  Msg const *state_onHndlr(Msg const *msg); 
  Msg const *ss_parkHndlr(Msg const *msg);
  Msg const *ss_driveHndlr(Msg const *msg);  
  Msg const *drive_ss_idleHndlr(Msg const *msg);
  Msg const *drive_ss_drive1Hndlr(Msg const *msg);
  Msg const *drive_ss_drive2Hndlr(Msg const *msg);
  Msg const *drive_ss_reverseGearHndlr(Msg const *msg);

  /* Standard functions, to show behaviour */
  void individualFunction3();
  void individualFunction1();
  void individualFunction();
  
  
public:  
  /* test trigger parameter*/
  static constexpr int BACK=0;
  static constexpr int SET=1;
  static constexpr int STATUS=2;
  static constexpr int REVERSE=3;


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
  Car_BACK_EVT,/* Adjustments are made by pressing the “mode” button, which increments the chosen quantity by one. */
  Car_SET_EVT, /* Pressing the “set” button switches the car into setting mode.  */
  Car_STATUS_EVT, /* Shall represent in which status the CAR is.  */
  Car_REVERSE_EVT /* Shall represent in which status the CAR is.  */
};

};

/* Εvents */
const Msg carMsg[] = { 
  Car::CarEvents::Car_BACK_EVT,   // Button : change state to previous Event, if implemented
  Car::CarEvents::Car_SET_EVT,    // Button : change state to next state (forward state transition)
  Car::CarEvents::Car_STATUS_EVT,  // Button : shows the status of the current state
  Car::CarEvents::Car_REVERSE_EVT // Button : only needed for reverse gear from idle (-> Reverse or -> Drive1)
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  Create and run the HSM Car as a static function, 
  which shall be placed in main function
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int HSM_Car(void)
{
  Car car;         
  car.onStart();
  while (1)  {
    int i;
    printf("\nSet Event[0=back, 1=forward, 2=status, 3=reverseGear 4>=Exit Car(App)]=");
    scanf("%d", &i);
    if (i < 0 || sizeof(carMsg)/sizeof(Msg) <= i) 
      break;
    car.onEvent(&carMsg[i]);     
  }
  printf("\n\nCar Application finished.Exit.\n\n");
  return 0;

}

/* set the date automatically to test if statemachine works */
static int test_HSM_Car(std::vector<int> steps, std::string result)
{

  Car car;         
  car.onStart();

  printf("\n");
  
  for (auto step : steps)
  {
    car.onEvent(&carMsg[step]); printf("\n");
  }
  
  std::cout << result << std::endl;
  printf("==============================================================\n\n\n\n");

  /*  
    here would be the while (1) loop
  */
  return 0;
}



