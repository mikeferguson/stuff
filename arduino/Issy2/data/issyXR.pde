/* This is the general Issy code, 
    with XR-Series hooks. */

#include <XRSeries.h>
#include <ax12.h>
#include <BioloidController.h>
//#include <Commander.h>
#include <SharpIR.h>
#include "poses.h"
#include "issyIK.h"

#define VOLTAGE_READ    7

XRSeries xr = XRSeries();
SharpIR forwardIR = SharpIR(GP2D12,1);
//Commander command = Commander();

/* Power variables */
int vmain = 1200;
unsigned long vtime = 0; // last time we sent out voltages

/* Setup the starting positions of the legs. */
void setupIK(){
    
    //gaits[0].r = 0;
    //gaits[1].r = 0;
    //gaits[2].r = 0;
    //gaits[3].r = 0;
  
    endpoints[RIGHT_FRONT].x = 60; //40
    endpoints[RIGHT_FRONT].y = 90; // 100 //80
    endpoints[RIGHT_FRONT].z = 100; //90
    
    endpoints[RIGHT_REAR].x = -50;  
    endpoints[RIGHT_REAR].y = 90;
    endpoints[RIGHT_REAR].z = 100;     
    
    endpoints[LEFT_FRONT].x = 60;
    endpoints[LEFT_FRONT].y = -90;
    endpoints[LEFT_FRONT].z = 100;  
    
    endpoints[LEFT_REAR].x = -50;
    endpoints[LEFT_REAR].y = -90;
    endpoints[LEFT_REAR].z = 100;  
    
    stepsInGait = 1;
    gaitStep = 0;
}

int smoothIR(){
    int reading = forwardIR.getData();
    reading = reading + forwardIR.getData();
    reading = reading + forwardIR.getData();
    reading = reading + forwardIR.getData();
    reading = reading/4;
    return reading;
}

int getSonar(){
    int reading = analogRead(0)/2;
    reading = reading + analogRead(0)/2;
    reading = reading + analogRead(0)/2;
    reading = reading + analogRead(0)/2;
    reading = reading/4;
    return (reading*10)/4;
}

int myX = 0;
int myY = 0;
float heading = 0;
int Pan = 0;

void turn(int d){
  travelX = 0;
  if(d > 0){
    travelRotZ = 0.207;
  }else{
    travelRotZ = -0.207;
    d = -d;
  } 
  d = d/3;
  while(d > 0){
    doIK();
    bioloid.interpolateSetup(200);
    while(bioloid.interpolating > 0){
      bioloid.interpolateStep();
      delay(3);
    }
    d--;    
  }
  //heading = heading + travelRotZ/4;
}

int explore(){
    SetPosition(14,512);
    if(xr.state[SNR_HD] < 30){
      int i, l, r;
      int d[11];
      // set down, tilt back
      travelX = 0;
      travelRotZ = 0.0;
      bodyRotY = 0.15;
      doIK();
      bioloid.interpolateSetup(150);
      while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
      }
      for(i=-5;i<6;i++){
        // look around
        SetPosition(14,512-(i*25));
        delay(40);
        d[i+5] = smoothIR();
      }  
      // settle down
      bodyRotY = 0.0;
      doIK();
      bioloid.interpolateSetup(150);
      while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
      }
      // set course
      l = 0;
      r = 0;
      for(i=0;i<5;i++){
        l = l + d[i];
      }
      for(i=5;i<11;i++){
        r = r + d[i];
      }      
      if(r > l){
        turn(-60);  
      }else{
        turn(60);
      }
//      // look around
//    if(Pan == 0){
//      if(HEAD_IR < 30)
//        turn(90);
//    }  
//    if(Pan < 6){
//      SetPosition(14,512-(Pan*25));
//    }else{
//      Pan = 0;
//      // take measurement at head right
//      if(HEAD_IR < 30)
//        turn(45);
    }
      
    //Pan++;
    
    travelX = 50;
    travelRotZ = 0.0;
    doIK();
    bioloid.interpolateSetup(150);
    while(bioloid.interpolating > 0){
      bioloid.interpolateStep();
      delay(3);
    }   
    return 0;
}

int walk(){
    // look around
    if(Pan == 0){
      if(HEAD_IR < 30)
        turn(90);
    }  
    if(Pan < 6){
      SetPosition(14,512-(Pan*25));
    }else{
      Pan = 0;
      // take measurement at head right
      if(HEAD_IR < 30)
        turn(45);
    }
      
    Pan++;
    
    travelX = 50;
    travelRotZ = 0.0;
    doIK();
    bioloid.interpolateSetup(150);
    while(bioloid.interpolating > 0){
      bioloid.interpolateStep();
      delay(3);
    }   
    return 0;
}

int testSonar(){
  if(xr.state[SNR_HD] < 30){
    //xr.sysMsg("Too Close");
    xr.sysReading(xr.state[SNR_HD]);
  }  
  delay(30);
  return 0; 
}
    

void setup(){   
    xr.Begin();
    pinMode(0,OUTPUT);
    delay(1000);
    
    // stand up slowly
    bioloid.loadPose(stand);
    bioloid.readPose();    
    /*bioloid.interpolateSetup(1500);
    while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
    }*/
   
    digitalWrite(0,HIGH);
    Serial.println("?Issy Alive");
    delay(2000);
    // bootstrap our IK
    setupIK();    
    gaitSelect(0);
    gaitStep = 0;
    doIK();
    bioloid.interpolateSetup(1000);
    while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
    }
    digitalWrite(0,LOW);
    gaitSelect(0);
    
    // add behaviors
    xr.addBeh("explore",&explore,&defaultPlan,&defaultInit);
    xr.addBeh("walk",&walk,&defaultPlan,&defaultInit);
    xr.addBeh("test sonar",&testSonar,&defaultPlan,&defaultInit);
    xr.sendBehList();
}

void loop(){
    // check battery
    if((millis() - vtime) > 3000){
      int v = analogRead(VOLTAGE_READ);
      // convert V to voltage (since it is scaled), and then add to running average
      vmain = ((vmain * 5)/10) + ((((v * 14)/10) * 5)/10);
      vtime = millis();
      if(vmain < 980){ // 9.8V is fairly conservative, but shutdown!
        xr.sysMsg("Battery Expired!");
        for(v=0;v<13;v++) // kill servos
          Relax(v);
        while(1);
      }else{ // print out voltage 
        Serial.print("?Voltage:");
        Serial.print(vmain/100);
        Serial.print(".");
        Serial.println(vmain%100);
      }
    }
    // update sensors
    xr.state[IR_HD] = smoothIR();
    xr.state[SNR_HD] = getSonar();
    xr.loop();
}

