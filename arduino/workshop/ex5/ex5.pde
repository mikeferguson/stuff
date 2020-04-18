/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX 5: Pullups and Switches
 * Copyright 2009 Michael E Ferguson
 */

// Wire switch on breadboard, one side 
// to GND, the other to D13
void setup(){		// this is called once
  pinMode(9,OUTPUT);
  pinMode(14,INPUT);
  digitalWrite(14,HIGH);
}

void loop(){		// while(1){loop();};
  digitalWrite(9,digitalRead(14));
  delay(25);	
}
