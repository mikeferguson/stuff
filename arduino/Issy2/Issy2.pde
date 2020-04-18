/*
 * Issy2: A Mech
 * 
 * Walk joystick controls walking
 * Look joystick controls turret
 * Button R1 resets turret
 * Button R2
 */

#include <ax12.h>
#include <BioloidController.h>
#include <Commander.h>
#include "nuke.h"

Commander command = Commander();
float voltage;
int temps[12];
int servo;  // which servo to measure
int firing;
unsigned long firetime;

int pan;
int tilt;

#define GUN_PORT    13

void setup(){
  int i;
  firing = 0;
  // make our LED output
  pinMode(0,OUTPUT);
  pinMode(GUN_PORT,OUTPUT);
  digitalWrite(GUN_PORT,LOW);
  // setup IK
  setupIK();
  gaitSelect(RIPPLE);
  // setup commander
  command.begin(38400);

  // wait, then check the voltage (LiPO safety)
  delay (1000);
  voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
  Serial.print ("System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
        
  // setup voltage monitoring system
  voltage = 12.0;

  pan = 512;
  tilt = 512;
  // stand up slowly
  bioloid.poseSize = 14;
  bioloid.readPose();
  bioloid.setNextPose(13,pan);
  bioloid.setNextPose(14,tilt);
  doIK();
  bioloid.interpolateSetup(1000);
  while(bioloid.interpolating > 0){
    bioloid.interpolateStep();
    delay(3);
  }
  bioloid.poseSize = 12;  // done using interpolation on pan/tilt
}

void loop(){
  // take commands - Gaits, Guns & Speed
  if(command.ReadMsgs() > 0){
    digitalWrite(0,HIGH-digitalRead(0));
    // Guns
    if(((command.buttons&BUT_LT) > 0) && (firing == 0)){
      digitalWrite(GUN_PORT,HIGH);
      firing = 1;
      firetime = millis() + 1200;
    }else if(((command.buttons&BUT_LT) == 0) && (firing == 1)){
      digitalWrite(GUN_PORT,LOW);
      firing = 0; 
    }    
    // Set Speeds and gaits - y/R speed first
    if((command.buttons&BUT_RT) > 0){   // Right Top button to strafe
      Yspeed = 2*command.walkH;         // max 200mm/s
    }else{
      if(command.buttons&BUT_R3){       // if burst
        Rspeed = -command.walkH/60.0;
      }else{
        Rspeed = -command.walkH/100.0;     // max to 2 rad/s
      }
      Yspeed = 0;
    }
    // Now, x is dependent on turning speed
    if( abs(Rspeed) > 0.35){
      Xspeed = command.walkV;
      gaitSelect(AMBLE);      
    }else if(command.buttons&BUT_R3){  // if burst
      Xspeed = 4*command.walkV;        // max 400mm/s
      gaitSelect(AMBLE);      
    }else{ 
      Xspeed = 2*command.walkV;        // max 200mm/s
      gaitSelect(RIPPLE);      
    }
    // and Turret 
    if(command.lookH > 90){
      pan -= 10;
    }else if(command.lookH > 10){
      pan -= 3;
    }else if(command.lookH < -90){
      pan += 10;
    }else if(command.lookH < -10){
      pan += 3;
    }
    if(pan > 819){
      pan = 819;
    }else if(pan < 307){
      pan = 307;
    } 
    if(command.lookV > 90){
      tilt -= 8;
    }else if(command.lookV > 10){
      tilt -= 2;
    }else if(command.lookV < -90){
      tilt += 8;
    }else if(command.lookV < -50){
      tilt += 2;
    }
    if(tilt > 570){
      tilt = 570;
    }else if(tilt < 375){
      tilt = 375;
    }
    // R1 resets to home position
    if(command.buttons&BUT_R1){
      pan = 512;
      tilt = 512;  
    }
    SetPosition(13,pan);
    SetPosition(14,tilt);    
  }
  
  // update IK if needed
  if(bioloid.interpolating == 0){
    // setup our next move
    doIK();
    bioloid.interpolateSetup(tranTime);
    // get voltage and temperature 
    //voltage = (voltage*0.8)+((ax12GetRegister(1+servo,AX_PRESENT_VOLTAGE,1))/50.0);
    //temps[servo] = ax12GetRegister(1+servo,AX_PRESENT_TEMPERATURE,1);
    //servo++;
    // finished looping?
    //if(servo == 12){
    //  Serial.print("V");
    //  Serial.println(voltage);
    //  for(servo=0;servo<12;servo++){
    //    Serial.print("S");
    //    Serial.print(servo+1);
    //    Serial.print("T");
    //    Serial.println(temps[servo]);
    //  }  
    //  servo = 0;
    //}
  }
  
  // update guns - only fire for a short burst
  if((firing == 1) && (millis() > firetime)){
    digitalWrite(GUN_PORT,LOW);
    firing = 0; 
  }
  
  // update joints
  bioloid.interpolateStep();
}

    // Gaits   
    //if((command.buttons&BUT_R3) && (currentGait!=AMBLE)) gaitSelect(AMBLE); 
    //if((command.buttons&BUT_L4) && (currentGait!=RIPPLE)) gaitSelect(RIPPLE);
