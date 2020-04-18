/* 
 * IK & Simple Control for a 3DOF+Mirror+Gripper Arm
 *  Copyright 2010 Michael E. Ferguson
 * 
 *         0  Elbow              Inputs:
 * Grip   / \                        X,Y,Z of end effector
 *   ==__0   \                       Gripper to ground angle
 *     Wrist  0  Shoulder            Gripper opening amount
 *            |  
 *            ^  Base Rotate
 *   Y<-   ___|___
 * 
 * Communications Protocol
 *  0xFF (X+500>>8) (X+500%256) (Y+500>>8) (Y+500%256) (Z>>8) (Z%256) G GA+100 CHK
 *   where:
 *    X,Y,Z are position of end effector in MM
 *    G is the Gripper opening amount (in MM)
 *    GA is gripper angle to ground, in degrees
 */
#include <ax12.h>
#include <BioloidController.h>

BioloidController bioloid = BioloidController(1000000);

int mins[] = {   0,  72,   40, 160,  59,   0, 422};
int maxs[] = {1023, 964, 1023, 854, 951, 984, 564};

/* TODO:
 *  Get to backside with flip over of IK
 *  Speed control of Gripper/Gripper Angle (fancy interpolation)
 * MAYBE:
 *  Speed ramping for X,Y,Z?
 */

#define L0 142L             // MM from ground to shoulder
#define L1 147L             // MM from shoulder to elbow
#define L2 147L             // MM from elbow to wrist
#define L3 120L             // MM from wrist to end
#define G_OFFSET 10L        // MM offset of static side of gripper

#define ROTATE       1
#define SHOULDER     2
#define ELBOW        3
#define WRIST        4 
#define SHOULDER_M   5
#define ELBOW_M      6
#define GRIP         7 

int X, Y, Z, G;              // MM displacement of end effector
float GA;                    // gripper angle
int next_X, next_Y, next_Z, next_G; 
float next_GA;

#define MAX_SPEED_X  5       // = (1/30) * MM/s = ~90mm/s? 
#define MAX_SPEED_Y  5
#define MAX_SPEED_Z  3
#define MAX_SPEED_G  3
#define MAX_SPEED_GA 0.04

/* Convert radians to servo position offset. */
int radToServo(float rads){
  float val = (rads*100)/51 * 100;
  return (int) val;
}

/* Compute Arm IK for 3DOF+Mirrors+Gripper */
void doIK(){
  // first, make this a 2DOF problem... by solving base
  int sol0 = radToServo(atan2(X,Y));
  // remove gripper offset from base
  int t = sqrt(sq((long)X)+sq((long)Y));
  sol0 -= radToServo(atan2((G/2)-G_OFFSET,t));

  // convert to X/Z plane, remove wrist, prepare to solve other DOF           
  long trueX = t - (long)((float)L3*cos(GA));   
  long trueZ = Z - L0 - (long)((float)L3*sin(GA));
            
  long im = sqrt(sq(trueX)+sq(trueZ));        // length of imaginary arm      
  
  float q1 = atan2(trueZ,trueX);              // angle between im and X axis
  long d1 = sq(L1) - sq(L2) + sq((long)im);
  long d2 = 2*L1*im;
  float q2 = acos((float)d1/float(d2));
  q1 = q1 + q2;
  int sol1 = radToServo(q1-1.57);
        
  d1 = sq(L1)-sq((long)im)+sq(L2);
  d2 = 2*L2*L1;
  q2 = acos((float)d1/(float)d2);
  int sol2 = radToServo(3.14-q2);

  // solve for wrist rotate
  int sol3 = radToServo(3.2 + GA - q1 - q2 );

  int servo = 512 - sol0;
  if(servo < maxs[ROTATE-1] && servo > mins[ROTATE-1])
    bioloid.setNextPose(ROTATE, servo);
  else{
    Serial.print("BASE ROTATE FAIL: ");
    Serial.println(servo);
  }
  //Serial.print("S");
  //Serial.print(sol0);
  
  servo = 512 - sol1;
  if(servo < maxs[SHOULDER-1] && servo > mins[SHOULDER-1]){
    bioloid.setNextPose(SHOULDER, servo);
    bioloid.setNextPose(SHOULDER_M, 512+sol1);
  }else{
    Serial.print("SHOULDER FAIL: ");
    Serial.println(servo);
  }
  //Serial.print(",");
  //Serial.print(sol1);
  
  servo = 512 + sol2;
  if(servo < maxs[ELBOW-1] && servo > mins[ELBOW-1]){
    bioloid.setNextPose(ELBOW, servo);
    bioloid.setNextPose(ELBOW_M, 512-sol2);
  }else{
    Serial.print("ELBOW FAIL: ");
    Serial.println(servo);
  }
  //Serial.print(",");
  //Serial.print(sol2);

  servo = 512 - sol3;
  if(servo < maxs[WRIST-1] && servo > mins[WRIST-1])
    bioloid.setNextPose(WRIST, servo);
  else{
    Serial.print("WRIST FAIL: ");
    Serial.println(servo);
  }
  //Serial.print(",");
  //Serial.println(sol3);
  
  // 563 -> 423 = 0->2.25" = 0->60mm (measured from center of hand pad)
  servo = 563-((7*G)/3);
  if(servo < maxs[GRIP-1] && servo > mins[GRIP-1])
    bioloid.setNextPose(GRIP, servo);
  else{
    Serial.print("GRIP FAIL: ");
    Serial.println(servo);
  }
}
        
#define PKT_SIZE 9
int vals[PKT_SIZE];            // temporary values, moved after we confirm checksum
int index;                     // -1 = waiting for new packet
int checksum;
        
void setup(){
  // make our LED output
  pinMode(0,OUTPUT);
  // setup IK
  X = next_X = 0;
  Y = next_Y = (2*L2)/3+L3;
  Z = next_Z = L0+(2*L1)/3;
  G = next_G = 30;
  GA = next_GA = 0;
  
  // setup
  Serial.begin(38400);

  // wait, then check the voltage (LiPO safety)
  delay (1000);
  float voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
  Serial.print ("System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
       
  bioloid.poseSize = 7;
  bioloid.readPose();
  doIK();
  bioloid.interpolateSetup(1000);
  while(bioloid.interpolating > 0){
    bioloid.interpolateStep();
    delay(3);
  }
  index = -1;
}

void loop(){
  // take commands
  while(Serial.available() > 0){
    if(index == -1){         // looking for new packet
      if(Serial.read() == 0xff){
        index = 0;
        checksum = 0;
      }
    }else if(index == 0){
      vals[index] = Serial.read();
      if(vals[index] != 0xff){            
        checksum += vals[index];
        index++;
      }
    }else{
      vals[index] = Serial.read();
      checksum += vals[index];
      index++;
      if(index == PKT_SIZE){ // packet complete
        digitalWrite(0,HIGH-digitalRead(0));
        if(checksum%256 != 255){
          // packet error!
          Serial.println("Error");
        }else{
          next_X = ((vals[0]<<8) + vals[1]) -500;
          next_Y = ((vals[2]<<8) + vals[3]) -500;
          next_Z = (vals[4]<<8) + vals[5];
          next_G = (int) vals[6];
          next_GA = (vals[7]-100) * .0174 ;
        }
        index = -1;
        Serial.flush();
      }
    }
  }
  // update IK if needed
  bioloid.interpolateStep();
  if(bioloid.interpolating == 0){
    // interpolate X,Y,Z
    if(X > next_X + MAX_SPEED_X){
      X -= MAX_SPEED_X;
    }else if(X < next_X - MAX_SPEED_X){
      X += MAX_SPEED_X;
    }else{
      X = next_X;
    }  
    
    if(Y > next_Y + MAX_SPEED_Y){
      Y -= MAX_SPEED_Y;
    }else if(Y < next_Y - MAX_SPEED_Y){
      Y += MAX_SPEED_Y;
    }else{
      Y = next_Y;
    } 
    
    if(Z > next_Z + MAX_SPEED_Z){
      Z -= MAX_SPEED_Z;
    }else if(Z < next_Z - MAX_SPEED_Z){
      Z += MAX_SPEED_Z;
    }else{
      Z = next_Z;
    } 
    
    if(G > next_G + MAX_SPEED_G){
      G -= MAX_SPEED_G;
    }else if(G < next_G - MAX_SPEED_G){
      G += MAX_SPEED_G;
    }else{
      G = next_G;
    } 
    
    if(GA > next_GA + MAX_SPEED_GA){
      GA -= MAX_SPEED_GA;
    }else if(GA < next_GA - MAX_SPEED_GA){
      GA += MAX_SPEED_GA;
    }else{
      GA = next_GA;
    } 
     
    // do the IK update
    doIK();
    bioloid.interpolateSetup(30); 
    //Serial.print("X:");
    //Serial.print(X);
    //Serial.print(",Y:");
    //Serial.print(Y);
    //Serial.print(",Z:");
    //Serial.println(Z);
  }
  // update joints
  bioloid.interpolateStep();
}
