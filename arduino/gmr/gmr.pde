/* GMR code for CNRG 2009 */

#include <ax12.h>
#include <BioloidController.h>
#include <SharpIR.h>
#include <Srf08.h>
#include <Wire.h>
#include <Tpa81.h>
#include "robot.h"
#include "nav.h"

void setup(){	
  int i;
  volatile unsigned char wait; 
  
  // setup start button & LED
  digitalWrite(PIN_START, HIGH);
  pinMode(PIN_PROG_LED,OUTPUT);
  pinMode(PIN_WALL_LED,OUTPUT);     
  // ... and motor system
  motorInit();
  closedInit();
  // ... and sensors (SRF-02 is initialized with I2C)
  uvtronInit(PIN_UV);
  // ... and fan
  pinMode(PIN_FAN,OUTPUT);
  // ... and serial/I2C
  Serial.begin(38400);
  delay(500);
  Serial.println("XR-B7 Online");
  Wire.begin();
  // ... and our map.
  mapInit();
  // ... and center head
  SetPosition(HEAD_SERVO,512);
  // test fan
  FAN_ON;
  delay(25);
  FAN_OFF;

  // Pre-run debug
  wait = digitalRead(PIN_START);

  while(wait){
    // standard delay .. 
    delay(10);
    wait = digitalRead(PIN_START);
    i++;
    if(i > 20){
      digitalWrite(PIN_PROG_LED, HIGH-digitalRead(PIN_PROG_LED));
      digitalWrite(PIN_WALL_LED, HIGH-digitalRead(PIN_WALL_LED));
      i = 0;        
    }
  }
}

// Run Behaviors
void loop(){
  // update odometer
  while(lCount > COUNTS_CM(1) ){
    // odometer is in CM
    odometer = odometer - 1;
    lCount = lCount - COUNTS_CM(1);
  }
  // now run behaviors
  if(arbitrate()>0){
    delay(200);
    plan();
    clearCounters; 
  }
  // pulse LED
  digitalWrite(PIN_PROG_LED, HIGH-digitalRead(PIN_PROG_LED));
}

