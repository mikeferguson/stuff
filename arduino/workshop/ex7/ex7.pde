/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX 7: Using the SharpIR Library
 * Copyright 2009 Michael E Ferguson
 */
 
#include <SharpIR.h>
// This is a C++ class instance
SharpIR frontIR = SharpIR(GP2D120,1);

void setup(){		// this is called once
	Serial.begin(38400);
}

void loop(){		// while(1){loop();};
	Serial.println(frontIR.getData());
	delay(200);	
}

// SharpIR is located in
// /usr/arduino/hardware/libraries/
