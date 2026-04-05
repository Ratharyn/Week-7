//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "gate.h"
#include "motor.h"

#include "display.h"



InterruptIn gateOpenLimitSwitch(PG_1);
InterruptIn gateCloseLimitSwitch(PF_7);



static bool gateOpenLimitSwitchState;
static bool gateCloseLimitSwitchState;

static gateStatus_t gateStatus;

//=====[Declarations (prototypes) of private functions]========================

static void gateOpenLimitSwitchCallback();
static void gateCloseLimitSwitchCallback();

//=====[Implementations of public functions]===================================

void gateInit()
{
    gateOpenLimitSwitch.mode(PullUp);
    gateCloseLimitSwitch.mode(PullUp);

    gateOpenLimitSwitch.fall(&gateOpenLimitSwitchCallback);
    gateCloseLimitSwitch.fall(&gateCloseLimitSwitchCallback);

    gateOpenLimitSwitchState = OFF;
    gateCloseLimitSwitchState = ON;
    gateStatus = GATE_CLOSED;
}

void gateOpen()
{
    if ( !gateOpenLimitSwitchState ) {
        motorDirectionWrite( DIRECTION_1 );
        gateStatus = GATE_OPENING;
        gateCloseLimitSwitchState = OFF;
    }
}

void gateClose()
{
    if ( !gateCloseLimitSwitchState ) {
        motorDirectionWrite( DIRECTION_2 );
        gateStatus = GATE_CLOSING;
        gateOpenLimitSwitchState = OFF;
    }
}

gateStatus_t gateStatusRead()
{
    return gateStatus;
}

//=====[Implementations of private functions]==================================

static void gateOpenLimitSwitchCallback()
{
    if ( motorDirectionRead() == DIRECTION_1 ) {
        motorDirectionWrite(STOPPED);
        gateStatus = GATE_OPEN;
        gateOpenLimitSwitchState = ON;
    }
}

static void gateCloseLimitSwitchCallback()
{
    if ( motorDirectionRead() == DIRECTION_2 ) {
        motorDirectionWrite(STOPPED);
        gateStatus = GATE_CLOSED;
        gateCloseLimitSwitchState = ON;
    }
}
void gateState(){
    static gateStatus_t previousStatus = GATE_CLOSED; //creates new varaible, sets a default CLOSED position

    gateStatus_t currentStatus = gateStatusRead();  //we no look at the function which reads the gates current position

    if(currentStatus != previousStatus){ //if  a new state is detected
        switch(currentStatus){
            case GATE_OPEN: 
                printf("Interrupt - Gate Open\r\n"); 
                displayCharPositionWrite (0,3 ); 
                displayStringWrite("Gate: OPEN    ");
                break;
            case GATE_CLOSED: 
                printf("Interrupt - Gate Closed\r\n");
                displayCharPositionWrite (0,3 ); 
                displayStringWrite("Gate: CLOSED   ");
                break;
            case GATE_OPENING:
                printf("GATE OPENING\r\n");
                displayCharPositionWrite (0,3 ); 
                displayStringWrite("Gate: OPENING ");
                break;
            case GATE_CLOSING:
                printf("GATE CLOSING\r\n");
                displayCharPositionWrite (0,3 ); 
                displayStringWrite("Gate: CLOSING ");
                break;

        }
        previousStatus = currentStatus; //update the status
    }
}