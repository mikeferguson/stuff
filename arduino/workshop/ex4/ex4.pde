/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX4: First attempt at switches
 * Copyright 2009 Michael E Ferguson
 */

void setup(){		// this is called once
  pinMode(14,INPUT);    // connect a switch between A0 and ground
  Serial.Begin(38400);
}

void loop(){		// while(1){loop();};
  if(digitalRead(14) == LOW){
    Serial.println(“Switch Pushed.”);
    delay(500);
  }	
}			// WHY DOESN'T THIS WORK?
