#!/usr/bin/env python

""" 
  Commander.py - Replicates ArbotiX Commander-style packets.
  Copyright (c) 2009 Michael E. Ferguson.  All right reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

class Commander:
    walkV = 0
    walkH = 0
    lookV = 0
    lookH = 0

        

    def __init__():
        pass

    def createPacket():
        s = ord(walkV) + ord(walkH) + ord(lookV) + ord(lookH)
        


void loop(){
    int walk_V = (1023-analogRead(WALK_V)-512)/5 + 128;
    int walk_H = (analogRead(WALK_H)-512)/5 + 128;
    int look_V = (1023-analogRead(LOOK_V)-512)/5 + 128;
    int look_H = (analogRead(LOOK_H)-512)/5 + 128;
 
    //buttons =    
    unsigned char buttons = 0;
    if(digitalRead(BUT_R1) == LOW) buttons += 1;
    if(digitalRead(BUT_R2) == LOW) buttons += 2;
    if(digitalRead(BUT_R3) == LOW) buttons += 4;
    if(digitalRead(BUT_L4) == LOW) buttons += 8;
    if(digitalRead(BUT_L5) == LOW) buttons += 16;
    if(digitalRead(BUT_L6) == LOW) buttons += 32;
    if(digitalRead(BUT_RT) == LOW) buttons += 64;
    if(digitalRead(BUT_LT) == LOW) buttons += 128;
 
    Serial.print(0xff,BYTE);
    Serial.print((unsigned char) walk_V,BYTE);
    Serial.print((unsigned char) walk_H,BYTE);
    Serial.print((unsigned char) look_V,BYTE);
    Serial.print((unsigned char) look_H,BYTE);
    Serial.print(buttons,BYTE);  // buttons
    Serial.print(0,BYTE);        // extra
    Serial.print((unsigned char)(255 - (walk_V+walk_H+look_V+look_H+buttons)%256),BYTE);
    
    if(i > 10){
      digitalWrite(10,HIGH-digitalRead(10));
      i=0;
    }
    i++;
    delay(FRAME_LEN);
