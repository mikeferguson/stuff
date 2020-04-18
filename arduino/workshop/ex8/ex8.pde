/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX 8: Using the Motors Library
 * Copyright 2009 Michael E Ferguson
 */
 
#include <Motors.h>
Motors drive = Motors();

void setup(){		// this is called once
  Serial.begin(38400);
  // drive.set(left speed, right speed)
  drive.set(200,200);
  delay(2000);
  drive.set(-200,200);
  delay(2000);
  drive.set(0,0);
}

void loop(){}

// Motors is located in
// /usr/arduino/hardware/libraries/
