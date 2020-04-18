/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX 6: Analog and IR
 * Copyright 2009 Michael E Ferguson
 */

// Connect a Sharp IR sensor to A1
void setup(){		// this is called once
  Serial.begin(38400);
}

void loop(){		// while(1){loop();};
  Serial.println(analogRead(1));
  delay(200);	
}			// why no pinMode?
