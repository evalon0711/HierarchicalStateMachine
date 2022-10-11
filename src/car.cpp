
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
  /* super State*/
  drive_ss_idle("ON:DRIVE:drive_ss_idle", &ss_drive/* super State*/, (EvtHndlr)&Car::drive_ss_idleHndlr),
  drive_ss_drive1("ON:DRIVE:drive_ss_drive1", &ss_drive/* super State*/, (EvtHndlr)&Car::drive_ss_drive1Hndlr),
  drive_ss_drive2("ON:DRIVE:drive_ss_drive2", &ss_drive/* super State*/, (EvtHndlr)&Car::drive_ss_drive2Hndlr),
  drive_ss_reverseGear("ON:DRIVE:drive_ss_reverseGear", &ss_drive/* super State*/, (EvtHndlr)&Car::drive_ss_reverseGearHndlr),

  // define members
  tsec(cReset0), tmin(cReset0), thour(cReset0), dday(1), dmonth(1)
{
  state_History = &state_off; 
  // state_History = &state_on; //make: *** [Makefile:57: execute] Segmentation fault
}



// ---  Car class individual functions  ---
void Car::individualFunction1() {
  printf("individualFunction1 not implemented");
}

void Car::individualFunction() {
  printf("individualFunction not implemented");
}

void Car::individualFunction3() {
  printf("individualFunction3 not implemented");
}

/*  */

/* TBD: Car_TICK_EVT, can be used, but makes confsion. Usually first state should be named for user.  */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
General: Handler definition for each state, covers all events 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Msg const *Car::topHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_STATUS_EVT:
    printf("Car::top\n");
    individualFunction1();
    return cEventIsProcessed;
  case START_EVT:
    printf("Car::topHndlr::STATE_START;\n\tTOP -> OFF\n");
    STATE_START(&state_off);
    return cEventIsProcessed;
  } 
  return msg;
}

/* ============== STATE OFF and ON ============== */

Msg const *Car::state_offHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Car_STATUS_EVT:
    printf("Car::off\n");
    individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
    printf("OFF -> ON\n");
    STATE_TRAN(&state_on);
    return cEventIsProcessed;

  case Car_BACK_EVT:
    printf("OFF: already off\n");
    return cEventIsProcessed; 

  /*todo what does the START_EVNT, fall back? needed?*/
  case START_EVT:
    // STATE_START(state_History);
    printf("!!! START_EVT: todo not defined here. \n");
    return cEventIsProcessed;

  } 
  return msg;
}

/*  */
Msg const *Car::state_onHndlr(Msg const *msg) {
  switch (msg->evt) {

  case Car_STATUS_EVT:
    printf("Car::ON\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
    printf("ON -> PARK\n");
    STATE_TRAN(&ss_park);
    return cEventIsProcessed;

  case Car_BACK_EVT:
    printf("ON -> OFF");
    STATE_TRAN(&state_off);
    return cEventIsProcessed; 
  
    /* if an event is processed, the event handler returns 0 (NULL pointer); otherwise it returns
     (“throws”)  the  message  for  further processing by higher-level states.  */
  } 
  /* While in setting mode the car ignores tick events */
  return msg;
}

/* ============== STATE ON :: Substates PARK and DRIVE ============== */

/*  */
Msg const *Car::ss_parkHndlr(Msg const *msg) {
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::PARK\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
    printf("PARK -> DRIVE");
    STATE_TRAN(&ss_drive);
    return cEventIsProcessed;

  case Car_BACK_EVT:
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
    printf("Car::DRIVE\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
    printf("DRIVE::Idle");
    STATE_TRAN(&drive_ss_idle);
    return cEventIsProcessed;

  case Car_BACK_EVT:
    printf("DRIVE -> PARK\n");
    STATE_TRAN(&ss_park);
    return cEventIsProcessed; 
  }
  /* While in setting mode the car ignores tick events */
  return msg;
}

/* ============== STATE ON :: Substate DRIVE :: Substate Idle, Drive1, Drive2, ReverseGear ============== */


/*  */
Msg const *Car::drive_ss_idleHndlr(Msg const *msg) {
  
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::DRIVE::IDLE\n");
    // individualFunction1();
    return cEventIsProcessed;

  /* todo here the hsm is back again...*/  
  case Car_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the car back into timekeeping mode. */
    STATE_TRAN(&drive_ss_drive1);
    printf("Car::DRIVE::DRIVE1");
    return cEventIsProcessed;

  case Car_BACK_EVT:
    STATE_TRAN(&ss_drive);
    printf("Car::DRIVE::IDLE");
    return cEventIsProcessed;  

  case Car_REVERSE_EVT:
    STATE_TRAN(&drive_ss_reverseGear);
    printf("Car::DRIVE::REVERSE_GEAR");
    return cEventIsProcessed;  
 } 
  
  return msg;
}

Msg const *Car::drive_ss_drive1Hndlr(Msg const *msg) {
  
  switch (msg->evt) {

    case Car_STATUS_EVT:
    printf("Car::DRIVE:DRIVE1\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the car back into timekeeping mode. */
    STATE_TRAN(&drive_ss_drive2);
    printf("Car::DRIVE:DRIVE2\n");
    return cEventIsProcessed;
  
  case Car_BACK_EVT:
    printf("Car::DRIVE::IDLE");
    STATE_TRAN(&drive_ss_idle);
    return cEventIsProcessed; 
  } 
  
  /* While in setting mode the car ignores tick events */
  return msg;
}

Msg const *Car::drive_ss_drive2Hndlr(Msg const *msg) {
  
  switch (msg->evt) {

    case Car_STATUS_EVT:
    printf("Car::DRIVE:DRIVE2\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
  /* Pressing the “set” button while adjusting month puts the car back into timekeeping mode. */
    printf("Car::DRIVE:DRIVE2 is already set.\n");
    return cEventIsProcessed;
  
  case Car_BACK_EVT:
    printf("Car::DRIVE::IDLE");
    STATE_TRAN(&drive_ss_idle);
    return cEventIsProcessed; 
  } 
  
  /* While in setting mode the car ignores tick events */
  return msg;
}


Msg const *Car::drive_ss_reverseGearHndlr(Msg const *msg) {
  
  switch (msg->evt) {
    case Car_STATUS_EVT:
    printf("Car::DRIVE::REVERSE_GEAR\n");
    // individualFunction1();
    return cEventIsProcessed;

  case Car_SET_EVT:
    STATE_TRAN(&drive_ss_idle);
    printf("Set Event triggers ");
    printf("Car::DRIVE::IDLE");
    return cEventIsProcessed;    

    case Car_BACK_EVT:
    STATE_TRAN(&drive_ss_idle);
    printf("Back Event triggers ");
    printf("Car::DRIVE::IDLE");
    return cEventIsProcessed;    
  } 
  
  /* While in setting mode the car ignores tick events */
  return msg;
}



/* todo year Handler where the year can be set. */