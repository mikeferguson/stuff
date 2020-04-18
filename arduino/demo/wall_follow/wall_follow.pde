// Wall Follow Demo - M. Ferguson
// This sketch will follow a right side wall using a single IR sensor.

// My motors/sharpIR library is available at svn.blunderingbotics.com
#include <Motors.h>
#include <SharpIR.h>
Motors drive = Motors();
SharpIR sensor = SharpIR(GP2D12,1);

void setup(){    
    // drive is our motors, set takes left speed, right speed as params
    // we'll drive forward, and a little to the right (toward the wall)
    drive.set(160,140);
}

void loop(){
    // IR sensor returns distance in centimeters
    int val = sensor.getData();
    if(val < 14){
        // if we are way too close, turn away fast
        drive.set(20,200);
    }else if(val < 19){
        // if we are too close, drift away
        drive.set(140,160);
    }else if(val > 25){
        // too far away, turn towards wall more
        drive.set(130,80);
    }else{
        // default, drift towards wall
        drive.set(160,140);
    }
}
