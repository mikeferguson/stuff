/* 
 * IK for a 5DOF Biped
 *  Michael E. Ferguson 2009-2010
 * 
 *    FRONT                                      SIDE
 *   O-----O    5th Level = body shift           0--=
 *   |     |    4th Level = forward/backward    /
 *   |     |                                   /
 *   =     =    3rd Level = knee (f/b)         0
 *   |     |                                   \
 *   |     |                                    \
 *   0     0    2nd Level = forward/backward     0--=
 *              1st Level = ankle shift
 * 
 *         TOP 
 *    |==|     |==|  ^         Major Parameters:
 *    |  |  +  |  |  |         X-offset of each foot from COG
 *    |__|     |__|  X         Angle from parallel to X (+ = clockwise from above)
 *                             Z-height of outside edge of swing foot
 *                   Y-->      Y-shift of COG over planted foot
 */

#include <ax12.h>
#include <BioloidController.h>
#include <Commander.h>

/* Setup Notes 
 * You'll have to: setup all the constants at the top of nuke.h
 * Put your neutral values (and check signs) into doIK()
 * The strafing needs some work (needs ramping, and we need to widen the stance for it to work -- might be fine on your bot
 * In general, the transition between turn, walk, strafe isn't so smooth -- I'll probably end up remaking teh gait engine later on
 * Send me your #s for lengths -- so I can check and make sure we don't need to ramp up to longs at this time...
 */

Commander command = Commander();
#include "nuke.h"

int mode = 0;

void setup(){
    int i;
    
    // setup IK    
    setupIK();    
    bodyRotY = -.14;
    // setup serial
    Serial.begin(38400);
    pinMode(0,OUTPUT);

    // wait, then check the voltage (LiPO safety)
    delay (1000);
    float voltage = (ax12GetRegister (8, AX_PRESENT_VOLTAGE, 1)) / 10.0;
    Serial.print ("System Voltage: ");
    Serial.print (voltage);
    Serial.println (" volts.");
    /*if (voltage < 10.0)
        while(1);*/

    gaitSelect(0);
    // set some defaults so that we don't freak out
    bioloid.poseSize = 16;
    bioloid.readPose();
    for(i=0;i<10;i++)
      bioloid.setNextPose(i,512);
    bioloid.setNextPose(RIGHT_SHOULDER,205-50);
    bioloid.setNextPose(LEFT_SHOULDER,819+50);
    bioloid.setNextPose(13,542);//522);
    bioloid.setNextPose(14,482);
    bioloid.setNextPose(RIGHT_ELBOW,512-200);
    bioloid.setNextPose(LEFT_ELBOW,512+200);
    step = -1;
    // stand up slowly
    doIK();
    bioloid.interpolateSetup(750);
    while(bioloid.interpolating > 0){
      bioloid.interpolateStep();
      delay(3);
    }
    
    // tighten up the knees
    for(i=3;i<=8;i++){
      ax12SetRegister2(i,AX_CW_COMPLIANCE_SLOPE,16);
    }
      ax12SetRegister2(1,AX_CW_COMPLIANCE_SLOPE,1);
      ax12SetRegister2(2,AX_CW_COMPLIANCE_SLOPE,1);
      ax12SetRegister2(9,AX_CW_COMPLIANCE_SLOPE,1);
      ax12SetRegister2(10,AX_CW_COMPLIANCE_SLOPE,1);
    
}

//#define MODE_WALK  0
//#define MODE_BODY  1

void loop(){
  // update IK if needed
  if(bioloid.interpolating == 0){
    //if(MOVING)
    //  delay(2000);
    doIK();
    bioloid.interpolateSetup(tranTime);
  }
  // update joints
  bioloid.interpolateStep();
  // take commands
  if(command.ReadMsgs() > 0){
    digitalWrite(0,HIGH-digitalRead(0));
    //if(command.buttons&BUT_R1){
    //  mode = MODE_WALK;
    //}else if(command.buttons&BUT_R2){
    //  mode = MODE_BODY;
    //}    
    //if(mode == MODE_WALK){
      Xspeed = command.walkV/3;
      if((command.buttons&BUT_LT) > 0){
        Yspeed = (3*Yspeed)/4 + (command.walkH/4);
        //Xspeed = Xspeed/4;
        Rspeed = 0;
      }else{
        Yspeed = 0;
        Rspeed = command.walkH/2;  // how we gonna do this?
      }
      bodyRotX = ((float)command.lookH)/250.0;
    //}else{
    //  Xspeed = 0;
    //  Rspeed = 0;
    //  Yspeed = 0;
    //  bodyPosX = command.walkV/2;
    //  bodyPosY = command.walkH/2;
    //  endpoints[LEFT_LEG].z = L_ANKLE+L_TIBIA+(2*L_FEMUR)/3 + command.lookH/2;
    //  endpoints[RIGHT_LEG].z = L_ANKLE+L_TIBIA+(2*L_FEMUR)/3 + command.lookH/2;
    //}
    bodyRotY = -.14 + (((float)command.lookV))/250.0;
    //  bodyRotZ = ((float)command.lookH)/250.0;
  }
}

