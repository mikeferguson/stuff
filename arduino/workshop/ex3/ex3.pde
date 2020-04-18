/********************************************************
 * An Introduction to Embedded Controllers with Robots 
 * EX 3: Blink an LED
 * Copyright 2009 Michael E Ferguson
 */
 
void setup(){		// this is called once
  pinMode(9,OUTPUT);
}

void loop(){		// while(1){loop();};
  digitalWrite(9,HIGH);
  delay(500); 	        // this halts the proc
  digitalWrite(9,LOW);
  delay(500);	        // for half a sec.
}
