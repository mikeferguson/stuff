/******************************************************************************
 * Cerebellum Motor Controller (Version A.0)
 * Copyright 2009-2010 Vanadium Labs LLC
 *
 * This PID loop runs at ~30hz
 *****************************************************************************/ 

#include <WProgram.h>
#include <math.h>

/* PID Parameters */
int CountsPerMM;             // encoder ticks per mm of travel
int bWidth;                  // base width in mm
int Kp;
int Kd;
int Ki;
int Ko;                      // output scale
int maxAccel;                // maximum acceleration per frame (counts)

/* PID modes */
unsigned int PIDmode;
#define PID_NONE        0
#define PID_SPEED       1
#define PID_DISTANCE    2
#define PID_BOTH        3

#define FRAME_RATE      33   // frame rate in millis (30Hz)
#define FRAMES          30
unsigned long f_time;        // last frame

#define MAXOUTPUT       255  

/* Setpoint Info For a Motor */
typedef struct{
  // These 2 are set up by user
  long Endpoint;             // desired distance to travel (in counts)
  int  VelocitySetpoint;     // desired average speed to travel (in counts/frame)
  // 
  long Encoder;              // actual reading
  long PrevEnc;              // last reading
  int  Velocity;             // current desired average speed (counts/frame), taking ramping into accound
  int PrevErr;
  int Ierror;   
  int output;                // last motor setting
  long rampdown;             // how long it will take to slow down
} SetPointInfo;

SetPointInfo left, right;

/* Initialize PID parameters to something known */
void setupPID(){
  // Default values for the PR-MINI
  CountsPerMM = 26;
  bWidth = 144;
  Kp = 25;
  Kd = 30;
  Ki = 0;
  Ko = 100;
  maxAccel = 50;
}

/* Linearly Ramp Velocity TO VelocitySetpoint */
void Ramp(SetPointInfo * x){
  if(x->Velocity < x->VelocitySetpoint){
    x->Velocity += maxAccel;
    if( x->Velocity > x->VelocitySetpoint)
      x->Velocity = x->VelocitySetpoint;
  }else{
    x->Velocity -= maxAccel;
    if( x->Velocity < x->VelocitySetpoint)
      x->Velocity = x->VelocitySetpoint;
  }  
}

/* PID control of motor speed */
void DoPid(SetPointInfo * p){
  int Perror, output;
  
  Perror = p->Velocity - (p->Encoder-p->PrevEnc);
          
  // Derivative error is the delta Perror
  output = (Kp*Perror + Kd*(Perror - p->PrevErr) + Ki*p->Ierror)/Ko;
  p->PrevErr = Perror;
  p->PrevEnc = p->Encoder;
  Serial.print(output);
  Serial.print(",");
  output += p->output;   
  // Accumulate Integral error *or* Limit output.
  // Stop accumulating when output saturates
  if (output >= MAXOUTPUT)
    output = MAXOUTPUT;
  else if (output <= -MAXOUTPUT)
    output = -MAXOUTPUT;
  else
    p->Ierror += Perror;
  
  p->output = output;
}

/* This is called by the main loop, does a X HZ PID loop. */
void updatePID(){
  if(PIDmode & PID_SPEED){  // otherwise, just return
    unsigned long j = millis();
    if(j > f_time){
      // update encoders
      left.Encoder = Encoders.left;
      right.Encoder = Encoders.right;
      // check endpoints (do we need to start ramping down)
      if(PIDmode & PID_DISTANCE){
        if( (abs(left.Endpoint) - abs(left.Encoder)) < abs(left.rampdown) ){
          left.VelocitySetpoint = 0;
          if(abs(left.Velocity) < 50 && abs(right.Velocity) < 50)
            PIDmode = 0;  
        }
        if( (abs(right.Endpoint) - abs(right.Encoder)) < abs(right.rampdown) ){
          right.VelocitySetpoint = 0; 
          if(abs(left.Velocity) < 50 && abs(right.Velocity) < 50)
            PIDmode = 0;  
        }
      }
      // update velocity setpoints (ramping to velocity)
      Ramp(&left);
      Ramp(&right);
      Serial.print(left.Velocity);
      Serial.print(",");
      
      // do PID update on PWM
      DoPid(&left);
      DoPid(&right);   
      Serial.print(left.PrevEnc);
      Serial.print(",");
      Serial.print(left.PrevErr);
      Serial.print(",");
      Serial.println(left.output);
      Serial.print(",");
      
      if(PIDmode > 0){
        drive.set(left.output, right.output);
      }else{
        drive.set(0,0);
      }
      // update timing
      f_time = j + FRAME_RATE;
    }
  }
}

/******************************************************************************
 * Functions to setup the PID system
 *****************************************************************************/ 

void clearEncoders(){
  PIDmode = 0;
  left.Encoder = 0;
  right.Encoder = 0;
  left.output = 0;
  right.output = 0;
  Encoders.Reset();  
}

/* X is distance in mm */
void setupXmovement(int x){
  PIDmode = 0;
  left.Endpoint = x * CountsPerMM;
  right.Endpoint = x * CountsPerMM;
  left.Velocity = 0;
  right.Velocity = 0;

  if(x > 0){ 
    left.VelocitySetpoint = (abs(left_speed)*CountsPerMM)/FRAMES;
    right.VelocitySetpoint = left.VelocitySetpoint;
  }else{
    left.VelocitySetpoint = -(abs(left_speed)*CountsPerMM)/FRAMES;
    right.VelocitySetpoint = left.VelocitySetpoint;
  }
  left.rampdown = (left.VelocitySetpoint/maxAccel);
  right.rampdown = (right.VelocitySetpoint/maxAccel);

  clearEncoders();
  Serial.print("Left Endpoint:");
  Serial.println(left.Endpoint);
  //Serial.print("Right Endpoint:");
  //Serial.println(right.Endpoint);
  Serial.print("Left VelocitySetpoint:");
  Serial.println(left.VelocitySetpoint);
  //Serial.print("Right VelocitySetpoint:");
  //Serial.println(right.VelocitySetpoint);
  PIDmode = PID_BOTH;
}

void setupTurn(int rad){
  // TODO 
}



///** our actual PID calculations **/
//void pidUpdate(){
//  unsigned long j = millis();
//  if(j > f_time){
//    
//    // regulate left side
//    if(PIDmode & PID_SPEED){
//      // update counters
//      long left = -lCount;
//      lCount = Encoders.left;    // total count
//      left += lCount;            // this frame
//      
//      long dError = abs(LeftStopPoint - lCount);  // + err = move forward
//      int error = LeftSetpoint - left;
//        
//      if((PIDmode & PID_DISTANCE) && (dError < (2*CountsPerUnit))){
//        // we are near the end point, ramp down
//        if(dError < (2*CountsPerUnit)){
//          drive.left(0);
//        }else{
//          //
//          //motorLeft((((float)LeftStopPoint - lDist)/11) * (float) LeftSpeed);    
//          //drive.left();
//        }      
//      }else{
//        // just regulate for speed
//        if(LeftSetpoint > 0)
//          error = LeftSetpoint - left;    // difference in counts
//        else{
//          error = LeftSetpoint + left;    // difference in counts         
//        }
//        error = error/50;                 // KP = 1/50
//        if(error > 20) error = 20;        // capped proportional only 
//        if(error < -20) error = -20;
//        drive.left(drive.getLeft() + error);
//      }
//    }
//    
//    // regulate right side
//    if(PIDmode & PID_SPEED){
//      // update counters
//      long right = -rCount;
//      rCount = Encoders.right;    // total count
//      right += rCount;            // this frame
//      
//      // errors?
//      
//      if((PIDmode & PID_DISTANCE) && (dError < ??)){
//        // we are near the end point, ramp down
//        if(dError < ?){
//          drive.left(0);
//          if(drive.getLeft() == 0){
//            moving = 0;
//            PIDmode = PID_NONE;
//          }
//        }else{
//          / regulate right side (for distance first, the speed)     
//        if((PIDmode & PID_DISTANCE) && ((rDist + 10) > RightStopPoint)){
//            // regulate for distance
//            if(rDist +3 > RightStopPoint){
//                brakeRight(); // motorRight(0);
//                status &= (255 - RIGHT_MOVING);     
//                if((status & LEFT_MOVING) == 0)    
//                    PIDmode = 0;  
//            }else     
//                motorRight((((float)RightStopPoint - rDist)/11) * (float) RightSpeed);
//          motorLeft((((float)LeftStopPoint - lDist)/11) * (float) LeftSpeed);    
//          drive.left();
//        }      
//      }else{
//        // just regulate for distance
//        if(RightSetpoint > 0)
//          error = RightSetpoint - right;    // difference in counts
//        else
//          error = RightSetpoint + right;    // difference in counts
//        // capped proportional only 
//        error = error/50;       // KP = 1/50
//        if(error > 20) error = 20;
//        if(error < -20) error = -20;
//        motorRight(error + RightSpeed);   
//      }
//    }
//
//    f_time = j + FRAME_RATE;
//  }
//}


