/******************************************************************************
 * Pons Sensory Controller (Version A.0)
 * Copyright 2009 Vanadium Labs LLC
 *
 * The Pons connects via USB (19k2) with a very basic serial interface:
 * PC PACKET STRUCTURE: 
 *   255 255 inst checksum
 *   checksum = 255 - (inst + params)%256
 * INSTRUCTIONS & (PARAMETERS):
 *   0x01 Identify -- sends back "Pons! VA.0\n"
 *   0x02 Start Sending Data
 *   0x03 Stop Sending Data
 * PONS PACKET STRUCTURE:
 *   255 index sonar left_ir right_ir head_ir checksum (index is which sonar)
 *
 * WIRING CONNECTIONS: (for roboduino board)
 *   Analog 0 - Left IR
 *   Analog 1 - Right IR
 *   Analog 2 - Head IR
 *   Digital 0/1 - Used by serial port
 *   Digital 2 - Front Sonar
 *   Digital 4 - Left Sonar
 *   Digital 7 - Back Sonar
 *   Digital 8 - Right Sonar
 *****************************************************************************/ 

#include <SharpIR.h>
#include <Srf05.h>

// mode == number of bytes recieved
#define MODE_WAITING    0x00
#define MODE_GOTFF      0x01
#define MODE_GOT2ND     0x02
#define MODE_INSTR      0x03
#define MODE_DATA       0x04

// IR Sensors
SharpIR leftIR(GP2D12,0);
SharpIR rightIR(GP2D12,1);
SharpIR headIR(GP2Y0A02YK,2);
// Sonar Sensors
Srf05 frontSonar(2);
Srf05 leftSonar(4);
Srf05 backSonar(7);
Srf05 rightSonar(12);

int index;
unsigned long time;
int mode;
boolean sending;
int reading;
int checksum;
int inst;
unsigned long count;

void setup(){
    Serial.begin(19200);
    index = 0;
    mode = MODE_WAITING;
    time = millis();
    sending = false;
}

void loop(){
    // Process serial loop
    while(Serial.available() > 0){
        if(mode == MODE_WAITING){       // start of new packet
            if(Serial.read() == 255)
                mode = MODE_GOTFF;
        }else if(mode == MODE_GOTFF){   // we require 2 0xFF to start
            if(Serial.read() == 255)
                mode = MODE_GOT2ND;
            else
                mode = MODE_WAITING;
        }else if(mode == MODE_GOT2ND){  // now we get instruction
            inst = Serial.read();
            mode = MODE_INSTR;
        }else{
            if((Serial.read() + inst) == 255){
                if(inst == 1)               // send who we are!
                    Serial.print("Pons! VA.0\n");
                else if(inst == 2)
                    sending = true;
                else 
                    sending = false;
            }
            mode = MODE_WAITING;
        }
    }
    if(sending){
        // wait for remainder of 50ms to pass
        while(millis() < time + 50);
        time = millis();
        // Now get a reading, send the full packet
        if(index == 0){
            reading = frontSonar.getData();
        }else if(index == 1){
            reading = leftSonar.getData();
        }else if(index == 2){
            reading = backSonar.getData();
        }else if(index == 3){
            reading = rightSonar.getData();
        }
        if(reading > 254)
            reading = 254;
        checksum = reading;
        // send a packet: 255 index sonar left_ir right_ir head_ir checksum
        Serial.print(255,BYTE);
        Serial.print(index,BYTE);
        Serial.print(reading,BYTE);
            reading = leftIR.getData();
            checksum += reading;
        Serial.print(reading,BYTE);
            reading = rightIR.getData();
            checksum += reading;
        Serial.print(reading,BYTE);
            reading = headIR.getData();    
            if(reading > 254) reading = 254;
            checksum += reading;
        Serial.print(reading,BYTE);
            checksum = checksum%256;
        Serial.print(checksum,BYTE);
        index++;
        index = (index%4);    
    }
}
