/* Xra5 Michael E Ferguson */
#include <Motors.h>
#include <SharpIR.h>
#include <Servo.h>

Motors motors(4,7);
Servo pan;
// Head IR - on a servo
SharpIR hir(GP2D12,0);

// Head servo positions
#define HEAD_LEFT       0
#define HEAD_FORWARD    90
#define HEAD_RIGHT      180

// IR thresholds
#define THRESHOLD       32

void setup(){
    Serial.begin(19200);
    pan.attach(9);
}

/* Robot runs forward on floor, if it approaches an object, it will 
   look left then right to find a way out, if that fails it will spin 90
   degrees to the left and try again. If we find a wall, we follow it. */
void loop(){
    /* set head to forward position */
    pan.write(HEAD_FORWARD);
    /* Highest priority, object in front */
    /*if(hir.getData() < THRESHOLD){
        // Stop!
        motors.set(0,0);
        pan.write(HEAD_LEFT);
        if(hir.getData() > THRESHOLD){
            // go left 
        }else{
            pan.write(HEAD_RIGHT);
            if(hir.getData() > THRESHOLD){
                // go right   
            }else{
                // turn around        
            }            
        }
    }    */
    /* every so often - look left for a wall */
    
    /* all good - go forward */
    motors.set(-128,-128);
    delay(25);
}
