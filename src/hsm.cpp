/** hsm.c -- Hierarchical State Machine implementation
 */
#include <assert.h>
#include "hsm.h"

/* Entry/exit actions and default tran-
sitions  are  also  implemented  inside
the  event  handler  in  response  to  the
pre-defined  events  ENTRY_EVT,
EXIT_EVT,  and  START_EVT.  
The  state
machine  engine  generates  and  dis-
patches  these  events  to  appropriate
handlers  upon  state  transitions. 
*/

static Msg const startMsg = { START_EVT };
static Msg const entryMsg = { ENTRY_EVT };
static Msg const exitMsg  = { EXIT_EVT };
#define MAX_STATE_NESTING 8

/* State Ctor...............................................................*/
State::State(char const *n, State *s, EvtHndlr h)
        : name(n), super(s), hndlr(h)
{}

/* Hsm Ctor.................................................................*/
Hsm::Hsm(char const *n, EvtHndlr topHndlr)
        : top("top", 0, topHndlr), name(n) 
{}

/* enter and start the top state............................................*/
void Hsm::onStart() {
    curr = &top;
    next = 0;
    curr->onEvent(this, &entryMsg);
    while (curr->onEvent(this, &startMsg), next) {
        State *entryPath[MAX_STATE_NESTING];
        register State **trace = entryPath;
        register State *s;
        *trace = 0;
        for (s = next; s != curr; s = s->super) {
            *(++trace) = s;                         /* trace path to target */
        }
        while (s = *trace--) {                 /* retrace entry from source */
            s->onEvent(this, &entryMsg);
        }
        curr = next;
        next = 0;
    }
}

/* state machine "engine"...................................................*/
void Hsm::onEvent(Msg const *msg) {
    State *entryPath[MAX_STATE_NESTING];
    register State **trace;
    register State *s;
    for (s = curr; s; s = s->super) {
        source = s;                     /* level of outermost event handler */
        msg = s->onEvent(this, msg);
        if (msg == 0) {                                       /* processed? */
            if (next) {                          /* state transition taken? */
                trace = entryPath;
                *trace = 0;
                for (s = next; s != curr; s = s->super) {
                    *(++trace) = s;                 /* trace path to target */
                }
                while (s = *trace--) {            /* retrace entry from LCA */
                    s->onEvent(this, &entryMsg);
                }
                curr = next;
                next = 0;
                while (curr->onEvent(this, &startMsg), next) {
                    trace = entryPath;
                    *trace = 0;
                    for (s = next; s != curr; s = s->super) {
                        *(++trace) = s; /* record path to target */
                    }
                    while (s = *trace--) { /* retrace the entry */
                        s->onEvent(this, &entryMsg);
                    }
                    curr = next;
                    next = 0;
                }
            }
            break; /* event processed */
        }
    }
}

/* exit current states and all superstates up to LCA .......................*/
void Hsm::exit_(unsigned char toLca) {
    register State *s = curr;
    while (s != source) {
        s->onEvent(this, &exitMsg);
        s = s->super;   
    }
    while (toLca--) {
        s->onEvent(this, &exitMsg);
        s = s->super;
    }
    curr = s;
}

/* 
find # of levels to Least Common Ancestor:

Finding the LCA can be expensive
However,  for  any  given  transition
the  LCA  needs  to  be  calculated  only
once. Method HsmToLCA_()returns the
number  of  levels  from  the  current
state to the LCA rather than a pointer
to the LCA state itself. The former is
the  same  for  all  instances of  a  given
HSM, that is, it is characteristic of the
Hsm (sub)class  rather  than  individual
state machine objects. For this reason
it  can  be  stored  in  a  static variable
shared by all instances. 
*/
unsigned char Hsm::toLCA_(State *target) {
    State *s, *t;
    unsigned char toLca = 0;
    if (source == target) {
        return 1;
    }
    for (s = source; s; ++toLca, s = s->super) {
        for (t = target; t; t = t->super) {
            if (s == t) {
                return toLca;
            }
        }
    }
    return 0;
}
