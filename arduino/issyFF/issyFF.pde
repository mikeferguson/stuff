/* Issy code for CNRG 2009 */
 
 /* TODO:
  * Need a better wall following (we run into it, we drift away from it...
  *    it's basically mayhem)
  */
  
 /* Twas the Night before CNRG
  * and all through the code,
  * not a bug was untouched,
  * or at least, these are the ones I fixed:
  *  1. Walk height was WAY too high for TASK_FF, back to 90mm:
  *     a. this allowed panForFire to be replaced, with a pan while turning function 
  *  2. Removed Flipdometer
  *  3. Longer time for panning
  *  4. Stop following walls when odometer < 0. (MAJOR CHANGE)
  * 
  * For TASK_WALKER_CHALLENGE:
  *  1. Increased TOP_SPEED
  *  2. Added forward turn code   
  
// 3 positions: left, center, right
SetPosition(14,664);
delay(250);
while(1){
  // Check LEFT
  SetPosition(14, 664);
  moveX(10);
  if(getIR() < 40) travelRotZ = -0.15;
  // Check CENTER
  SetPosition(14, 360);
  moveX(10);
  if(getIR() < 40){
       
  }
  // Check RIGHT
  SetPosition(14, 512);
  moveX(10);
  if(getIR() < 40) travelRotZ = 0.15;
}
  */

#include <ax12.h>
#include <BioloidController.h>
#include <SharpIR.h>
#include <Commander.h>
#include "poses.h"
#include "issy.h"
#include "nav.h"

int count = 0;
/* Power variables */
int vmain = 1200;
unsigned long vtime = 0; // last time we sent out voltages

Commander command = Commander();

void setup(){   
    volatile int wait;
    int i;
    // Set up port for start switch
    digitalWrite(PIN_START, HIGH);
    pinMode(PIN_LED,OUTPUT);
    	
    // setup IK    
    setupIK();    
    gaitSelect(QUAD_RIPPLE);
    gaitStep = 0;
    // stand up slowly
    delay(100);
    bioloid.loadPose(stand);
    bioloid.readPose();       
     bodyRotY=-0.03; 
    doIK();
    bioloid.interpolateSetup(1000);
    while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
    }
    LIFT_HEIGHT=20;
    //BREG = FOLLOW_LEFT;
    //panForFire();

    // Set up fan & test
    pinMode(PIN_FAN, OUTPUT);
    digitalWrite(PIN_FAN, HIGH);
    delay(25);
    digitalWrite(PIN_FAN, LOW);

    Serial.begin(38400);
    Serial.println("Wait...");
    wait = digitalRead(PIN_START);
    while(wait){
      // standard delay .. 
      delay(10);
      wait = digitalRead(PIN_START);
      i++;
      if(i > 20){
        digitalWrite(PIN_LED, HIGH-digitalRead(PIN_LED));
        i = 0;        
      }
      //Serial.println(getIR());
      //delay(200);
    }

    // Start our map
#ifdef TASK_FF
    mapInit();
    PrintHeading(lheading,nStart);
#endif
#ifdef TASK_WALKER_CHALLENGE
    SetPosition(14,664); //360);
    moveX(30);    // we need to move forward into the arena to pick up the wall
#endif
}

unsigned long Htime = 0;
int in_turn = 0;

#define WALK_DISTANCE  9

void loop(){
  // check battery
  /*if((millis() - vtime) > 3000){
    int v = analogRead(PIN_VOLTAGE);
    // convert V to voltage (since it is scaled), and then add to running average
    vmain = ((vmain * 5)/10) + ((((v * 14)/10) * 5)/10);
    vtime = millis();
    if(vmain < 980){ // 9.8V is fairly conservative, but shutdown!
      Serial.println("EXPIRED!");
      //xr.sysMsg("Battery Expired!");
      for(v=0;v<13;v++) // kill servos
        Relax(v);
      while(1);
    }else{ // print out voltage 
      Serial.print("?Voltage:");
      Serial.print(vmain/100);
      Serial.print(".");
      Serial.println(vmain%100);
    }
  }*/

#ifdef TASK_FF
  // now run behaviors
  if(arbitrate()>0){
    // let motors wind down
    plan();  
  }
#endif

#ifdef TASK_WALKER_CHALLENGE
  if(Htime == 3){
    // Head is facing forward take reading
    if(getIR() < 30){
      // turn?  
    }
    SetPosition(14,664); //360);
    travelRotZ = 0;
    moveX(WALK_DISTANCE);
    Htime = 0;
  }else{
    // Head to side
    //if(getIR() > NO_WALL){
      // look sideways
      //SetPosition(14,819); //204);
      //moveX(WALK_DISTANCE);
      //if(getIR() > NO_WALL){
        //turnX(-75); 
      //}else{
      //  SetPosition(14,664); //360);
      //  moveX(WALK_DISTANCE); 
      //}
    //}
    followLeft(40);  //followRight(40);
    if(Htime == 2){
      SetPosition(14,512);
    }
    moveX(WALK_DISTANCE);
    //Htime++;
  }

//  // update IK if needed
//  if(bioloid.interpolating == 0){
//  #ifdef TASK_WALKER_CHALLENGE
//    if(Htime > 10){
//      SetPosition(HEAD_SERVO,512);
//      delay(250);
//      if(getIR() < 30){
//        // turn away
//        turnX(-75);
//      }
//      Htime=0;
//    }
//    followRight(40);
//    if(getIR() > 70){
//      // walk forward and turn
//      if(in_turn == 0) moveX(25);
//      travelRotZ = -0.207;
//      travelX = 0;
//      in_turn = 1;
//      Htime = 0;
//    }else{
//      travelX = TOP_SPEED;
//      in_turn = 0;
//    }
//    Htime++;
//    // follow wall, or turn...???
//  #endif
//    doIK();
//    bioloid.interpolateSetup(50);
//  }
//  // update joints
//  bioloid.interpolateStep();
#endif
  
#ifdef TASK_BE_AWESOME  
  // update IK if needed
  if(bioloid.interpolating == 0){
    doIK();
    bioloid.interpolateSetup(50);
  }
  // update joints
  bioloid.interpolateStep();
  // get commands
  if(command.ReadMsgs() > 0){
    travelX = (command.walkV);
    if(travelX > TOP_SPEED)
      travelX = TOP_SPEED;
    if((command.buttons&WALK_TOP) > 0)
      travelY = (command.walkH);
    else
      travelRotZ = -(command.walkH)/375.0;
    bodyRotY = (((float)command.lookV))/375.0;
    if((command.buttons&LOOK_TOP) > 0)
      bodyRotX = ((float)command.lookH)/375.0; 
    else
      bodyRotZ = ((float)command.lookH)/375.0;
    if(millis() > Htime + 1000){
      if((command.buttons&WALK_3) > 0){
        LIFT_HEIGHT++;
        Htime = millis();
      }
      if((command.buttons&LOOK_4) > 0){
        LIFT_HEIGHT--;  
        Htime = millis();
      }
    }
  }
#endif
}

