/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */
#include <assert.h>
#include <stdio.h>
#include "hsm.h"

#define KGRN "\x1B[32m"

class Watch : public Hsm {
  int tsec, tmin, thour, dday, dmonth;
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
};

// ----------------------------------------------------------------------------------------
// CPP file definitions
// ----------------------------------------------------------------------------------------

/*  */
enum WatchEvents {
  Watch_MODE_EVT,
  Watch_SET_EVT,
  Watch_TICK_EVT
};

// ---  Watch class individual functions  ---
void Watch::showTime() {
  printf("time: %2d:%02d:%02d", thour, tmin, tsec);
}

void Watch::showDate() {
  printf("date: %02d-%02d", dmonth, dday);
}

void Watch::tick() {
  static int const month[] = { 
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
  };
  if (++tsec == 60) {
    tsec = 0;
    if (++tmin == 60) {
      tmin = 0;
      if (++thour == 24) {
        thour = 0;
        if (++dday == month[dmonth-1]+1) {
          dday = 1;
          if (++dmonth == 13) 
            dmonth = 1;
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
    return 0;
  case Watch_TICK_EVT:
    if (++tsec == 60)
      tsec = 0;
    printf("%sWatch::top-TICK;",KGRN);
    showTime();
    return 0;
  } 
  return msg;
}
/*  */
Msg const *Watch::timekeepingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(state_timekeepingHist);
    return 0;
  case Watch_SET_EVT:
    STATE_TRAN(&state_setting);
    printf("%sWatch::timekeeping-SET;\n",KGRN);
    return 0;
  } 
  return msg;
}

/* time Handler covers all events */
Msg const *Watch::timeHndlr(Msg const *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    showTime();
    return 0;
  case Watch_MODE_EVT:
    STATE_TRAN(&ss_date);
    printf("%sWatch::time-DATE;\n",KGRN);        
    return 0;
  case Watch_TICK_EVT:
    printf("%sWatch::time-TICK;\n",KGRN);        
    tick();
    showTime();
    return 0;
  } 
  return msg;
}


/*  */
Msg const *Watch::dateHndlr(Msg const *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    showDate();
    return 0;
  case Watch_MODE_EVT:
    STATE_TRAN(&ss_time);
    printf("%sWatch::date-DATE;\n",KGRN);        
    return 0;
  case Watch_TICK_EVT:
    printf("%sWatch::date-TICK;\n",KGRN);        
    tick();
    showDate();
    return 0;
  default: // for debugging reasons
    printf("Watch::dataHndlr: Event %d does not work \n",msg->evt);

  } 
  return msg;
}


/*  */
Msg const *Watch::settingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&ss_hour);
    return 0;
  default: // for debugging reasons
    printf("Watch::settingHndlr: Event %d does not work \n",msg->evt);

  } 
  return msg;
}


/*  */
Msg const *Watch::hourHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_minute);
    printf("%sWatch::hour-SET;",KGRN);
    return 0;
  default: // for debugging reasons
    printf("Watch::hourHndlr: Event %d does not work \n",msg->evt);


  } 
  return msg;
}


/*  */
Msg const *Watch::minuteHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_day);
    return 0;
  default: // for debugging reasons
    printf("Watch::minuteHndlr: Event %d does not work \n",msg->evt);
  } 
  return msg;
}


/*  */
Msg const *Watch::dayHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_month);
    printf("%sWatch::day-SET;",KGRN);
    return 0;
  default: // for debugging reasons
    printf("Watch::dayHndlr: Event %d does not work \n",msg->evt);
  } 
  return msg;
}

/*  */
Msg const *Watch::monthHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&state_timekeeping);
    printf("%sWatch::month-SET;",KGRN);
    return 0;
  default: // for debugging reasons
    printf("Watch::monthHndlr: Event %d does not work \n",msg->evt);
  } 
  return msg;
}


/* Watch Constructor */
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
  tsec(0), tmin(0), thour(0), dday(1), dmonth(1)
{
  state_timekeepingHist = &ss_time; 
}

/* Εvents */
const Msg watchMsg[] = { 
  Watch_MODE_EVT, // Button
  Watch_SET_EVT, // Button
  Watch_TICK_EVT // trigger of seconds, done manually.
};



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main() {
  Watch watch;         
  watch.onStart();
  for (;;)  {
    int i;
    printf("\nEvent[0=mode,1=set,2=tick]<-");
    scanf("%d", &i);
    if (i < 0 || sizeof(watchMsg)/sizeof(Msg) <= i) 
      break;
    watch.onEvent(&watchMsg[i]); 
  }
  return 0;
}
