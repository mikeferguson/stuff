/* 
 * Gait Engine
 */ 

#ifndef GAITS_H
#define GAITS_H
/* First time through, we pick up the definitions */

extern ik_req_t gaits[2]; 
extern int pushSteps;           // how much of the cycle our foot is on the ground
extern int stepsInCycle;        // how many steps in a cycle
extern int tranTime;            // time to transition between IK poses
extern int step; 
extern int liftHeight;
extern int currentGait;

#define TYPICAL_SHIFT   15
//F_WIDTH

/* select a gait pattern to use */
void gaitSelect(int gait);
/* find the translation of the endpoints (x,y,z,r) given our gait parameters */
extern int (*gaitGen)();

#define RIPPLE          0       // any other gaits?

#else
/* Second time through we need to pick up implementations */

/* 
 * Gait Engine
 */
ik_req_t gaits[2]; 
int tranTime;
int step; 

int pushSteps;
int stepsInCycle;
int liftHeight = 15;
int currentGait = -1;

/* returns which leg is swing leg */
int defaultGaitGen(){
  int shift = TYPICAL_SHIFT;
  tranTime = 65; //185;
  if(Rspeed != 0)
    Xspeed = 0;
    //  shift = shift/2;
  //  tranTime = 32;
  //}
  Rspeed =0;
  if( MOVING ){
    step = (step+1)%stepsInCycle;
    if(step == 0){                              // left is now the ground leg
      bodyPosY = -shift/2;                        // begin shift to the left
      bodyRotX = -.1;
      gaits[LEFT_LEG].x -= Rspeed + Xspeed/8;     // shift forward
      gaits[RIGHT_LEG].x -= Xspeed/8;            // ''
      //tranTime = tranTime*2;
    }
    else if(step == 1){                           
      bodyPosY = -shift;                          // complete body shift
      gaits[LEFT_LEG].x -= Xspeed/8;              // shift forward
      gaits[RIGHT_LEG].x -= Xspeed/8;
      gaits[RIGHT_LEG].z = -liftHeight;           // lift right foot
      tranTime = 32;
    }
    else if(step == 2 && gaits[RIGHT_LEG].z < 0){
      gaits[LEFT_LEG].x -= Xspeed/8;              // shift forward
      gaits[RIGHT_LEG].x = 0;                     // swing right foot to center
      // y?
      bioloid.setNextPose(RIGHT_SHOULDER,RIGHT_SHOULDER_CENTER);
      bioloid.setNextPose(LEFT_SHOULDER,LEFT_SHOULDER_CENTER);
      tranTime = 32;
    }
    else if(step == 3 && gaits[RIGHT_LEG].z < 0){  
      gaits[RIGHT_LEG].x = Xspeed/2 - Rspeed;     // swing right foot to forward
      gaits[LEFT_LEG].x = -Xspeed/8;              // shift forward
      // y?    
      // swing left arm to forward
      bioloid.setNextPose(RIGHT_SHOULDER,RIGHT_SHOULDER_CENTER-Xspeed);
      bioloid.setNextPose(LEFT_SHOULDER,LEFT_SHOULDER_CENTER-Xspeed);
      tranTime = 32;
    }
    else if(step == 4){            
      bodyPosY = -shift/2;                        // begin shifting back to centers
      bodyRotX = 0;
      gaits[RIGHT_LEG].z = 0;                     // set right foot down
      gaits[LEFT_LEG].x -= Xspeed/8;              // shift forward       
      gaits[RIGHT_LEG].x -= Xspeed/8;                 
      tranTime = 32;
    }
    else if(step == 5){                           // right leg is now the ground leg
      bodyPosY = shift/2;                         // begin shift to the right
      bodyRotX = .1;
      gaits[RIGHT_LEG].x -= Rspeed + Xspeed/8;    // shift forward
      gaits[LEFT_LEG].x -= Xspeed/8;              // ''
      //tranTime = tranTime*2;
    }
    else if(step == 6){  
      bodyPosY = shift;                           // complete body shift
      gaits[RIGHT_LEG].x -= Xspeed/8;             // shift forward
      gaits[LEFT_LEG].x -= Xspeed/8; 
      gaits[LEFT_LEG].z = -liftHeight;            // lift left foot
      tranTime = 32;
    }
    else if(step == 7 && gaits[LEFT_LEG].z < 0){  
      gaits[RIGHT_LEG].x -= Xspeed/8;             // shift forward
      gaits[LEFT_LEG].x = 0;                      // swing left foot to center
      // y?
      bioloid.setNextPose(RIGHT_SHOULDER,RIGHT_SHOULDER_CENTER);
      bioloid.setNextPose(LEFT_SHOULDER,LEFT_SHOULDER_CENTER);
      tranTime = 32;
    }
    else if(step == 8 && gaits[LEFT_LEG].z < 0){  
      gaits[LEFT_LEG].x = Xspeed/2 - Rspeed;      // swing left foot to forward
      gaits[RIGHT_LEG].x = -Xspeed/8;             // shift forward
      // y?    
      // swing right arm to forward
      bioloid.setNextPose(RIGHT_SHOULDER,RIGHT_SHOULDER_CENTER+Xspeed);
      bioloid.setNextPose(LEFT_SHOULDER,LEFT_SHOULDER_CENTER+Xspeed);
      tranTime = 32;
    }else if(step == 9){ 
      bodyPosY = shift/2;                         // begin shifting back to centers
      bodyRotX = 0;
      gaits[LEFT_LEG].z = 0;                      // set left foot down     
      gaits[RIGHT_LEG].x -= Xspeed/8;             // shift forward                
      gaits[LEFT_LEG].x -= Xspeed/8; 
      //tranTime = 32;
    }
  }else{  // stopped
    bodyPosY = 0;
    gaits[RIGHT_LEG].z = 0;
    gaits[LEFT_LEG].z = 0;
    //step = -1;
  }
  if(step < 5){
    return RIGHT_LEG;
  }else{
    return LEFT_LEG;
  }
}

void gaitSelect(int gait){
  stepsInCycle = 10;
  step = -1;
}

int (*gaitGen)() = &defaultGaitGen;

#endif

