// Encoder Demo - M. Ferguson
// This sketch controls the speed of our motors. Only the code
// for the left channel is shown here.

// My motors library is available at svn.blunderingbotics.com
#include <Motors.h>
Motors drive = Motors();

// ms for each frame, this gives us 5Hz update rate
#define FRAME_LEN    200 
unsigned long last;  // last time our loop ran, to do 5hz update

// left/right actual values, used by our ISR to count ticks
volatile int lCount = 0;
volatile int rCount = 0;

// left/right desired values
int lSet;
int rSet;

// motor speeds
int lSpeed;
int rSpeed;

// PID tuning parameters
int kP;
int trial = 0;

// enable our interrupts
void setup(){
    Serial.begin(38400);
    // Encoder clock output is tied to Digital2, call ISR function named left.
    attachInterrupt(0, left, RISING);
    lSet = 20;          // we'll try to go about 50rpm
    last = millis();   
    lSpeed = 0;
    kP = 20;            // trial 0 - kP too low, residual offset
}

// main loop
void loop(){
    while(millis() < last + FRAME_LEN);
    int error = lSet - lCount;
    lCount = 0;
    last = millis();
    int nextSpeed = (kP * error)/100 + lSpeed;
    drive.set(nextSpeed,0);
    lSpeed = nextSpeed;
    Serial.print(nextSpeed);
    Serial.print(",");
    Serial.println(error);
    
    // our demo code: change our kP over time
    if(millis() > 30000){
        // we are done
        drive.set(0,0);
        while(1);
    }else if(millis() > 20000 && trial == 1){
        // trial 3 - just right! 
        drive.set(0,0);
        kP = 100;
        lSpeed = 0;
        trial = 2;
    }else if(millis() > 10000 && trial == 0){
        // trial 2 - kP is too high, oscillates
        drive.set(0,0);
        kP = 1500;
        lSpeed = 0;
        trial = 1;
    }
}

// only regulating left speed right now...
void left(){
    // we want this to be fast, so we'll avoid digitalRead()
    // digital 8 is the direction channel, but that is actually AVR PB0, so:
    if(PINB&0x01)
        lCount++;
    else
        lCount--;
}
