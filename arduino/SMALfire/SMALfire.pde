/* SMALdog code for Trinity 2010, adpated from Issy's CNRG 2009 code */

#include <ax12.h>
#include <BioloidController.h>
#include <SharpIR.h>
#include "nuke.h"
#include "behavior.h"

/* TODO:
 *  Wall Following Still needs work (too far right
 *  MoveX going too far, TurnX going too far?
 *  Adjust walk height?
 */

int followRight(int);
void setup(){   
    volatile int wait;
    int i;
    // Set up port for start switch
    digitalWrite(PIN_START, HIGH);
    pinMode(PIN_LED,OUTPUT);
    Serial.begin(38400);
    	
    // setup IK    
    setupIK();    
    gaitSelect(RIPPLE_DISCO);  // or RIPPLE_SHIFT or RIPPLE_DISCO
    // stand up slowly
    delay(1000);
    bioloid.poseSize = 12;
    bioloid.readPose();     
    doIK();
    bioloid.interpolateSetup(1000);
    while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
    }
    HEAD_FORWARD;

    // Set up fan & test
    pinMode(PIN_FAN, OUTPUT);
    digitalWrite(PIN_FAN, HIGH);
    delay(25);
    digitalWrite(PIN_FAN, LOW);
    // setup UVTron
    uvtronInit(PIN_UV);

    Serial.println("Wait...");
    wait = digitalRead(PIN_START);
    while(wait){
        // standard delay .. 
        delay(10);
        wait = digitalRead(PIN_START);
        i++;
        if(i > 20){
          //digitalWrite(PIN_LED, HIGH-digitalRead(PIN_LED));
          i = 0;
        } 
        //delay(1000);
        //followRight(30);
        //Serial.println(travelRotZ);
    }
    
    // Start our map
    mapInit();
    PrintHeading(lheading,nStart);
}

void loop(){
  // now run behaviors
  if(arbitrate()>0){
    // let motors wind down
    plan();  
  }
}

