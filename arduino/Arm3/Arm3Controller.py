#!/usr/bin/env python

""" 
  Arm3Controller: test utility for Arm3 demo on arbotiX robocontroller
    (Based on commander.py from pypose distribution)
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

import time, sys, serial
import wx
width = 400
scale = 400.0

INITIAL_X = 0
INITIAL_Y = 220 #147+120
INITIAL_Z = 240 #147+142
INITIAL_G = 30
INITIAL_GA = 0

class Arm3Controller(wx.Frame):
    TIMER_ID = 100

    def __init__(self, parent, ser):  
        wx.Frame.__init__(self, parent, -1, "Arm3Controller", style = wx.DEFAULT_FRAME_STYLE & ~ (wx.RESIZE_BORDER | wx.MAXIMIZE_BOX))
        self.ser = ser    

        sizer = wx.GridBagSizer(10,10)

        # X and Y axis
        self.move = wx.Panel(self,size=(width,width))
        self.move.SetBackgroundColour('WHITE')
        self.move.Bind(wx.EVT_MOTION, self.onMove)  
        self.move.Bind(wx.EVT_LEFT_DOWN, self.onMove)  
        #wx.StaticLine(self.move, -1, (width/2, 0), (1,width), style=wx.LI_VERTICAL)
        #wx.StaticLine(self.move, -1, (0, width/2), (width,1))
        sizer.Add(self.move,(0,0),wx.GBSpan(1,1),wx.EXPAND|wx.ALL,5)
        self.X = INITIAL_X/scale
        self.Y = INITIAL_Y/scale

        # Z and G axis
        zBox = wx.StaticBox(self, -1, 'Z Pos',size=(75,-1))
        zBox.SetFont(wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.BOLD))
        zSizer = wx.StaticBoxSizer(zBox,orient=wx.VERTICAL) 
        self.Zmove = wx.Slider(self, -1, INITIAL_Z, 0, 400, wx.DefaultPosition, (75, width-30), wx.SL_VERTICAL | wx.SL_LABELS | wx.SL_INVERSE)
        zSizer.Add(self.Zmove)
        sizer.Add(zSizer, (0,1),wx.GBSpan(1,1), wx.EXPAND|wx.TOP|wx.RIGHT,5)
        gBox = wx.StaticBox(self, -1, 'Gripper',size=(75,-1))
        gBox.SetFont(wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.BOLD))
        gSizer = wx.StaticBoxSizer(gBox,orient=wx.VERTICAL) 
        self.Gmove = wx.Slider(self, -1, INITIAL_G, 0, 60, wx.DefaultPosition, (75, width-30), wx.SL_VERTICAL | wx.SL_LABELS | wx.SL_INVERSE)
        gSizer.Add(self.Gmove)        
        sizer.Add(gSizer, (0,2),wx.GBSpan(1,1), wx.EXPAND|wx.TOP|wx.RIGHT,5)
        gaBox = wx.StaticBox(self, -1, 'G-Angle',size=(75,-1))
        gaBox.SetFont(wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.BOLD))
        gaSizer = wx.StaticBoxSizer(gaBox,orient=wx.VERTICAL) 
        self.GAmove = wx.Slider(self, -1, INITIAL_GA, -90, 90, wx.DefaultPosition, (75, width-30), wx.SL_VERTICAL | wx.SL_LABELS | wx.SL_INVERSE)
        gaSizer.Add(self.GAmove)        
        sizer.Add(gaSizer, (0,3),wx.GBSpan(1,1), wx.EXPAND|wx.TOP|wx.RIGHT,5)
        self.Z = INITIAL_Z
        self.G = INITIAL_G
        self.GA = INITIAL_GA

        # timer for output
        self.timer = wx.Timer(self, self.TIMER_ID)
        self.timer.Start(33)
        wx.EVT_CLOSE(self, self.onClose)
        wx.EVT_TIMER(self, self.TIMER_ID, self.onTimer)

        # bind the panel to the paint event
        wx.EVT_PAINT(self, self.onPaint)
        self.dirty = 1
        self.onPaint()

        self.SetSizerAndFit(sizer)
        self.Show(True)

    def onClose(self, event):
        self.timer.Stop()
        self.Destroy()

    def onMove(self, event=None):
        if event.LeftIsDown():        
            pt = event.GetPosition()
            if pt[0] > 0 and pt[0] < width and pt[1] > 0 and pt[1] < width:
                self.X = (pt[0]-(width/2.0))/(width/2.0)
                self.Y = ((width/2.0)-pt[1])/(width/2.0)
                #print self.X,self.Y
                self.onPaint()        
        #else:
        #    self.X = 0
        #    self.turn = 0
        #    pass

    def onPaint(self, event=None):
        # this is the wx drawing surface/canvas
        dc = wx.PaintDC(self.move)
        dc.Clear()
        # draw crosshairs
        dc.SetPen(wx.Pen("black",1))
        dc.DrawLine(width/2, 0, width/2, width)
        dc.DrawLine(0, width/2, width, width/2)
        dc.SetPen(wx.Pen("red",2))
        # draw arm        
        dc.DrawLine(width/2, width/2, (width/2) + self.X*(width/2), (width/2) - self.Y*(width/2))
        dc.SetBrush(wx.Brush('red', wx.SOLID))
        dc.SetPen(wx.Pen("black",2))
        dc.DrawCircle((width/2) + self.X*(width/2), (width/2) - self.Y*(width/2), 5)    
        
    def onTimer(self, event=None):
        # configure output
        X = int(self.X * scale) + 500
        Y = int(self.Y * scale) + 500
        Z = int(self.Zmove.GetValue())
        G = self.Gmove.GetValue()
        GA = self.GAmove.GetValue()
        #print X,Y,Z,G
        self.sendPacket(X, Y, Z, G, GA)
        self.timer.Start(50)
        
    def sendPacket(self, X, Y, Z, G, GA):
        # send output
        self.ser.write('\xFF')
        # XH, XL, YH, YL, ZH, ZL, G, GA, CHK
        self.ser.write(chr(X>>8))
        self.ser.write(chr(X%256))
        self.ser.write(chr(Y>>8))
        self.ser.write(chr(Y%256))
        self.ser.write(chr(Z>>8))
        self.ser.write(chr(Z%256))
        self.ser.write(chr(G))
        self.ser.write(chr(GA+100))
        self.ser.write(chr(255 - (((X>>8)+(X%256)+(Y>>8)+(Y%256)+(Z>>8)+(Z%256)+G+(GA+100))%256) ) )
        while self.ser.inWaiting() > 0:        
            print self.ser.readline().rstrip()       
        #while self.ser.inWaiting() > 0:
        #    k = self.ser.readline().rstrip()
        #    try:
        #        x = ord(k)
        #        print x
        #    except:
        #        print k
            
if __name__ == "__main__":
    # Arm3Controller.py <serialport>
    ser = serial.Serial()
    ser.baudrate = 38400
    ser.port = sys.argv[1]
    ser.timeout = 0.5
    ser.open()
    
    app = wx.PySimpleApp()
    frame = Arm3Controller(None, ser)
    app.MainLoop()

