/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX 2: Hello World
 * Copyright 2009 Michael E Ferguson
 */

void setup(){		// this is called once
  Serial.begin(38400);
}

void loop(){		// while(1){loop();};
  Serial.println("Hello World!");
  delay(2000);	
}
