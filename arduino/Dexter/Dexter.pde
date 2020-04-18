/*
 * Auto-Generated by NUKE!
 *   http://arbotix.googlecode.com
 */

#include <ax12.h>
#include <BioloidController.h>
#include <Commander.h>
#include "nuke.h"

Commander command = Commander();
int multiplier;

#define RIPPLE_SPEED    1
#define AMBLE_SPEED     3
#define TRIPOD_SPEED    4
#define TOP_SPEED       6

void setup(){
    pinMode(0,OUTPUT);
    // setup IK
    setupIK();
    gaitSelect(RIPPLE);
    // setup serial
    Serial.begin(38400);

    // wait, then check the voltage (LiPO safety)
    delay (1000);
    float voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
    Serial.print ("System Voltage: ");
    Serial.print (voltage);
    Serial.println (" volts.");
    if (voltage < 10.0)
        while(1);

    // stand up slowly
    bioloid.poseSize = 18;
    bioloid.readPose();
    doIK();
    bioloid.interpolateSetup(1000);
    while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
    }
    multiplier = RIPPLE_SPEED;
}

void loop(){
  // take commands
  if(command.ReadMsgs() > 0){
    digitalWrite(0,HIGH-digitalRead(0));
    // select gaits
    if(command.buttons&BUT_R1){ gaitSelect(RIPPLE_SMOOTH); multiplier=RIPPLE_SPEED;}
    if(command.buttons&BUT_R2){ gaitSelect(AMBLE_SMOOTH); multiplier=AMBLE_SPEED;}
    if(command.buttons&BUT_R3){ gaitSelect(RIPPLE); multiplier=RIPPLE_SPEED;}
    if(command.buttons&BUT_L4){ gaitSelect(AMBLE); multiplier=AMBLE_SPEED;}
    if(command.buttons&BUT_L5){ gaitSelect(TRIPOD); multiplier=TRIPOD_SPEED;}
    if(command.buttons&BUT_L6){ gaitSelect(TRIPOD); multiplier=TOP_SPEED;}
    // set movement speed
    Xspeed = multiplier*(command.walkV);
    if((command.buttons&BUT_LT) > 0)
      Yspeed = multiplier*(command.walkH/2);
    else
      Rspeed = -(command.walkH)*multiplier/300.0;
    bodyRotY = (((float)command.lookV))/250.0;
    if((command.buttons&BUT_RT) > 0)
      bodyRotX = ((float)command.lookH)/250.0;
    else
      bodyRotZ = ((float)command.lookH)/250.0;
  }
  
  // if our previous interpolation is complete, recompute the IK
  if(bioloid.interpolating == 0){
    doIK();
    bioloid.interpolateSetup(tranTime);
  }
  
  // update joints
  bioloid.interpolateStep();
  
}

