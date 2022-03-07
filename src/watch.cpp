/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */
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
    return 0;
  case Watch_TICK_EVT:
    if (++tsec == cMinutesInHour)
      tsec = cReset0;
    printf("Watch::top-TICK;");
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
    printf("Watch::timekeeping-SET;\n");
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
    printf("Watch::time-DATE;\n");        
    return 0;
  case Watch_TICK_EVT:
    printf("Watch::time-TICK;\n");        
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
    printf("Watch::date-DATE;\n");        
    return 0;
  case Watch_TICK_EVT:
    printf("Watch::date-TICK;\n");        
    tick();
    showDate();
    return 0;
  } 
  return msg;
}


/*  */
Msg const *Watch::settingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&ss_hour);
    return 0;

  } 
  return msg;
}


/*  */
Msg const *Watch::hourHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_minute);
    printf("Watch::hour-SET;");
    return 0;
  case Watch_MODE_EVT:
    if (++thour == cHoursOnDay)
        thour = cReset0;
    printf("Watch::hour-SET: hour++: %d", thour);
    return 0; //todo clarify which number shall be used as return value
  } 
  return msg;
}


/*  */
Msg const *Watch::minuteHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_day);
    return 0;
  case Watch_MODE_EVT:
    if (++tmin == cMinutesInHour)
        tmin = cReset0;
    printf("Watch::min-SET: min++: %d", tmin);
    return 0; //todo clarify which number shall be used as return value
  } 
  return msg;
}


/*  */
Msg const *Watch::dayHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&ss_month);
    printf("Watch::day-SET;");
    return 0;
  case Watch_MODE_EVT:
    if (++dday == Watch::cDaysPerMonth[dmonth-1]+1) 
      dday = 1;
    printf("Watch::day-SET: day++: %d", dday);
    return 0; //todo clarify which number shall be used as return value
  }
  
  return msg;
}

/*  */
Msg const *Watch::monthHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&state_timekeeping);
    printf("Watch::month-SET;");
    return 0;
  case Watch_MODE_EVT:
    if (++dmonth == cMonthInYear+1) 
            dmonth = 1;
    printf("Watch::month-SET: hour++: %d", dmonth);
    return 0; //todo clarify which number shall be used as return value
  } 
  return msg;
}

/* todo year Handler where the year can be set. */


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
  tsec(cReset0), tmin(cReset0), thour(cReset0), dday(1), dmonth(1)
  /* cDaysPerMonth{ 
    // Jan, Feb, .............................,Dez
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
    }
 */
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
    printf("\nEvent[0=mode,1=set,2=tick]->");
    scanf("%d", &i);
    if (i < 0 || sizeof(watchMsg)/sizeof(Msg) <= i) 
      break;
    watch.onEvent(&watchMsg[i]); 
  }
  return 0;
}
