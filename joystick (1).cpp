#include "joystick.h"
#include "motor.h"
#include "gate.h"

AnalogIn joyY(A2);
InterruptIn JoyPress(D2);

static bool emergencystop = false; 
static void emergencystopcallback(){
    emergencystop = !emergencystop; //toggle function
}

void JoystickInit() {
JoyPress.mode(PullUp); 
JoyPress.fall(&emergencystopcallback);//"gateOpenLimitSwitch.fall(&gateOpenLimitSwitchCallback);""
}//maps the toggle to the joystick button

bool emergencystopchecker(){
    return emergencystop;
}

// https://os.mbed.com/teams/ELEC2645-201516/code/Joystick/docs/tip/Joystick_8cpp_source.html (52-71)
int JoystickPosition(){ //converts analogue output of stick and returns a static value
    float yValue = joyY.read();

    if (yValue > 0.8f) { //not using 0 and 1 to give a bit of a buffer, less twitchy response
        return 1;
    }
    else if (yValue < 0.2f) {
        return 2;
    }
    else if (yValue > 0.4f && yValue < 0.6f) {
        return 3; //this will be used to stop movement when stick is neutral
    }
    return 0;
}

void JoystickUpdate(){
    int JoystickState = JoystickPosition(); 
    static int lastposition = 0;

    if (emergencystop){
        motorDirectionWrite(STOPPED);
        return;
    }

    else if (JoystickState == 1) {
        if (gateStatusRead() != GATE_OPEN) {
            gateOpen();
            printf("Gate Opening!\r\n");
            lastposition = 1;
        } 
    } 
    else if (JoystickState == 2) {
        if (gateStatusRead() != GATE_CLOSED){
            gateClose();
        }
            printf("Gate Closing\r\n"); 
            lastposition = 2;
        
    }
    else {
        motorDirectionWrite(STOPPED);
        printf("Movement Stopped\r\n");
        lastposition = 0;
    }
}