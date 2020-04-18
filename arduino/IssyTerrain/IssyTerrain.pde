/*
 * Issy2: Terrain Adaptation
 */

#include <ax12.h>
#include <BioloidController.h>
#include <Commander.h>
#include <Motors2.h>
#include "nuke.h"
#include "terrain.h"

Commander command = Commander();
Motors2 guns = Motors2();
float volts[12];
int temps[12];
int servo;  // which servo to measure

void setup(){
  int i;
  // make our LED output
  pinMode(0,OUTPUT);
  // foot sensor LEDs
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  // setup IK
  setupIK();
  //gaitSelect(RIPPLE);
  gaitSelectTerrain();
  // setup serial
  Serial.begin(38400);

  // wait, then check the voltage (LiPO safety)
  delay (1000);
  float voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
  Serial.print ("System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
  //if (voltage < 10.0)
  //    while(1);
        
  // setup voltage monitoring system
  for(servo=0;servo<12;servo++){
    volts[servo] = 12.0;  
  }

  // stand up slowly
  bioloid.poseSize = 12;
  bioloid.readPose();
  doIK();
  bioloid.interpolateSetup(1000);
  while(bioloid.interpolating > 0){
    bioloid.interpolateStep();
    delay(3);
  }
       
  delay(3000);
  Xspeed = 35;
}

void loop(){
  // take commands - Gaits, Guns & Speed
  if(command.ReadMsgs() > 0){
    // Gaits   
    if((command.buttons&WALK_3) && (currentGait!=AMBLE))
      gaitSelect(AMBLE);  
    if((command.buttons&LOOK_4) && (currentGait!=RIPPLE))
      gaitSelect(RIPPLE);
    // Guns
    if((command.buttons&LOOK_TOP) > 0)
      guns.right(128);
    else
      guns.right(0);
    // Speeds
    Xspeed = (SPEED_MULTIPLIER*command.walkV);      // convert from cm/s to mm/s
    if((command.buttons&WALK_TOP) > 0)
      Yspeed = SPEED_MULTIPLIER*command.walkH;      // convert from cm/s to mm/s
    else{
      Rspeed = -command.walkH/50.0;  // convert to rad/s, max 1rad/s
      Yspeed = 0;
    }
    SetPosition(14,512+command.lookV);
    bodyRotZ = ((float)command.lookH)/50.0;  // convert to radians, max 2
  }
  
  // update IK if needed
  if(bioloid.interpolating == 0){
    // setup our next move
    //Serial.print("Step:");
    //Serial.println(step);
    //delay(1000);
    doIK();
    bioloid.interpolateSetup(tranTime);
#ifdef USE_PC
    // get voltage and temperature 
    volts[servo] = (volts[servo]*0.8)+((ax12GetRegister(1+servo,AX_PRESENT_VOLTAGE,1))/50.0);
    temps[servo] = ax12GetRegister(1+servo,AX_PRESENT_TEMPERATURE,1);
    servo++;
    // finished looping?
    if(servo == 12){
      Serial.print("V");
      Serial.println(volts[0]);
      for(servo=0;servo<12;servo++){
        Serial.print("S");
        Serial.print(servo+1);
        Serial.print("T");
        Serial.println(temps[servo]);
      }  
      servo = 0;
    }
#endif
  }
  
  // update joints
  bioloid.interpolateStep();
  doGroundUpdate();
}

