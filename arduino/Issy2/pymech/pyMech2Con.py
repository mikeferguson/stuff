#!/usr/bin/env python

""" 
  PyMech2 Controller
  Copyright (c) 2009,2010 Michael E. Ferguson.  All right reserved.

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

import pygame, time, sys, serial, threading
from pygame.locals import *
from pyMech2Conf import *

# Commander definitions
BUT_R1 = 1
BUT_R2 = 2
BUT_R3 = 4
BUT_L4 = 8
BUT_L5 = 16
BUT_L6 = 32
BUT_RT = 64
BUT_LT = 128

class pyMechCon:
    def __init__(self, ser=None, port="/dev/ttyUSB0", baud=38400):  
        self.forward = 0            # raw forward speed from joystick
        self.turn = 0               # raw turn speed from joystick
        self.pan = 0                # raw pan position from joystick
        self.tilt = 0               # raw tilt position from joystick
        self.buttons = 0          
        self.burst = 0              # run faster?
        self.fire = 0               # fire gun(s)?
        self.strafe = 0             # strafe or turn?
        self.Pan = 128

        self.adjust = 0             # how much to turn
        self.TURRET_ACTIVE = False  # do we move torso or body?
        self.exitcond = False

        if ser != None:
            self.ser = ser
        else:
            try:
                self.ser = serial.Serial()
                self.ser.baudrate = baud
                self.ser.port = port
                self.ser.timeout = 3
                self.ser.open()    
                print "pyMechCon: Port Opened" 
            except:
                print "pyMechCon: Cannot open port"
                sys.exit(0)
        # we want a joystick (we should also have an on screen way to control the mech?) 
        self.stick = pygame.joystick.Joystick(0)
        self.stick.init()

    def sendFullPacket(self):
        # configure output
        Xspeed = 128
        Rspeed = 128
        self.Pan = int((self.Pan * 0.8)+26) 
        #Pan = 128
        self.buttons = BUT_L4                           # ripple is fine
        # if pushing stick all the way to the side, turn in place
        if abs(self.turn) > 0.75 and self.strafe == 0:
            Rspeed = int(100*self.turn) + 128           # max 1rad/s
            self.buttons = BUT_R3                       # Amble for fast turn
        # use body for small turns, if not walking much
        elif abs(self.turn) < 0.3 and abs(self.forward) < 0.3 and self.strafe == 0 and self.TURRET_ACTIVE == False:
            self.Pan = int(20*self.turn) + 128 
            #self.Pan = int(20*self.turn) + 128
        # walking, let's steer        
        else:
            if self.forward > 0:  
                if self.burst > 0:          
                    Xspeed = int(60*self.forward) + 128 # cm/s
                    self.buttons = BUT_R3 
                else:
                    Xspeed = int(40*self.forward) + 128             
            else:
                Xspeed = int(20*self.forward) + 128     
                self.buttons = BUT_R3                   # amble in reverse
            if self.strafe > 0:
                Rspeed = int(30*self.turn) + 128        # strafe, rather than turn
                self.buttons = BUT_RT
            else:
                if self.TURRET_ACTIVE:  
                    self.Pan = int(20*self.turn) + 128
                else:
                    Rspeed = int(50*self.turn) + 128        # max 0.5rad/s
        # tilt servo
        Tilt = int(50*self.tilt) + 128
        # guns
        if self.fire > 0:
            self.buttons = self.buttons + BUT_LT
            self.fire = self.fire - 1
            if self.fire == 0:
                print "end fire"
        # send output
        self.ser.write('\xFF')
        self.ser.write(chr(Tilt))
        self.ser.write(chr(self.Pan))
        self.ser.write(chr(Xspeed))
        self.ser.write(chr(Rspeed))
        self.ser.write(chr(self.buttons))
        self.ser.write(chr(0))
        self.ser.write(chr(255 - ((Xspeed+Rspeed+Tilt+self.Pan+self.buttons)%256)))
        #print "pyMechCon: Status = " , str(Xspeed) , str(int(Rspeed))# , str(int(self.servo))
        #return "pyMechCon: Status = " , str(int(self.forward)) , str(int(self.turn))# , str(int(self.servo))
            
    def thread(self):  
        """ This is the seperate thread, for handling joystick control """
        while not self.exitcond:
            for event in pygame.event.get():
                self.handle(event)
            # now send data!      
            self.sendFullPacket()
            self.buttons = 0                # TODO: THIS WILL NEED TO COME OUT
            time.sleep(0.005) #0.033333) 

    def handle(self, event):
        """ Actually do something with the event. """
        if event.type == pygame.QUIT: 
            # send quit to serial thread?     
            self.exitcond = True               
            sys.exit()
        elif event.type == JOYAXISMOTION:
            if event.axis == FORWARD_AXIS:
                # user has pushed forward on joystick
                if self.TURRET_ACTIVE:
                    self.tilt = -event.value
                else:
                    self.forward = -event.value            
            elif event.axis == TURN_AXIS:
                # joystick has moved to the side
                self.turn = event.value                    
            elif event.axis == ADJUST_AXIS: 
                self.adjust = -15 * (event.value - 1)   
                print "Adjustment value: " + str(self.adjust)
        elif event.type == JOYBUTTONDOWN:
            print event.button
            if event.button == BUTTON_SERVO:
                self.TURRET_ACTIVE = True 
            elif event.button == BUTTON_FIRE:   
                print "fire"
                self.fire = 40
            elif event.button == BUTTON_BURST:
                self.burst = 1
            elif event.button == BUTTON_STRAFE:
                self.strafe = 1
        elif event.type == JOYBUTTONUP: 
            if event.button == BUTTON_SERVO:     
                self.TURRET_ACTIVE = False
            elif event.button == BUTTON_FIRE:   
                print "end fire"
                self.fire = 0
            elif event.button == BUTTON_BURST:
                self.burst = 0
            elif event.button == BUTTON_STRAFE:
                self.strafe = 0

if __name__ == "__main__":
    pygame.init()
    pyCon = pyMechCon()
    t = threading.Thread(target=pyCon.thread)
    t.start()
    try:    
        while True:
            while pyCon.ser.inWaiting > 0: 
                s = pyCon.ser.readline()
                print s, 
            time.sleep(0.01)
    except KeyboardInterrupt:
        sys.exit(0)

