/******************************************************************************
 * Gaits Auto-Generated by NUKE!
 *   http://arbotix.googlecode.com
 *****************************************************************************/

/* Standard Transition time should be of the form (k*BIOLOID_FRAME_LENGTH)-1
 *  for maximal accuracy. BIOLOID_FRAME_LENGTH = 33ms, so good options include:
 *   32, 65, 98, etc...
 */
#define STD_TRANSITION          65

/* Simple calculations at the beginning of a cycle. */
void DefaultGaitSetup(){
    // nothing!
}

/* Simple, fast, and rough gait. StepsInCycle == leg count.
    Legs will make a fast triangular stroke. */
ik_req_t DefaultGaitGen(int leg){
  if( MOVING ){
    // are we moving?
    if(step == gaitLegNo[leg]){
      // leg up, middle position
      gaits[leg].x = 0;
      gaits[leg].y = 0;
      gaits[leg].z = -liftHeight;
      gaits[leg].r = 0;
    }else if(((step == gaitLegNo[leg]+1) || (step == gaitLegNo[leg]-(stepsInCycle-1))) && (gaits[leg].z < 0)){
      // leg down position                                           NOTE: dutyFactor = pushSteps/StepsInCycle
      gaits[leg].x = (Xspeed*cycleTime*pushSteps)/(2*stepsInCycle);     // travel/Cycle = speed*cycleTime
      gaits[leg].y = (Yspeed*cycleTime*pushSteps)/(2*stepsInCycle);     // Stride = travel/Cycle * dutyFactor
      gaits[leg].z = 0;                                                 //   = speed*cycleTime*pushSteps/stepsInCycle
      gaits[leg].r = (Rspeed*cycleTime*pushSteps)/(2*stepsInCycle);     //   we move Stride/2 here
    }else{
      // move body forward
      gaits[leg].x = gaits[leg].x - (Xspeed*cycleTime)/stepsInCycle;    // note calculations for Stride above
      gaits[leg].y = gaits[leg].y - (Yspeed*cycleTime)/stepsInCycle;    // we have to move Stride/pushSteps here
      gaits[leg].z = 0;                                                 //   = speed*cycleTime*pushSteps/stepsInCycle*pushSteps
      gaits[leg].r = gaits[leg].r - (Rspeed*cycleTime)/stepsInCycle;    //   = speed*cycleTime/stepsInCycle
    }
  }else{ // stopped
    gaits[leg].z = 0;
  }
  return gaits[leg];
}

void AmbleGaitSetup(){
  bodyPosX = Xspeed/20; 
}

void gaitSelect(int GaitType){
  tranTime = STD_TRANSITION;
  cycleTime = 0;
  bodyPosX = 0;
  currentGait = GaitType;
  if(GaitType == RIPPLE){               // simple ripple, 8 steps
    gaitGen = &DefaultGaitGen;
    gaitSetup = &DefaultGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 0;
    gaitLegNo[LEFT_REAR] = 2;
    gaitLegNo[LEFT_FRONT] = 4;
    gaitLegNo[RIGHT_REAR] = 6;
    pushSteps = 6;
    stepsInCycle = 8;
  }else if(GaitType == RIPPLE_LEFT){    // simple ripple for left turns
    gaitGen = &DefaultGaitGen;
    gaitSetup = &DefaultGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 4;
    gaitLegNo[LEFT_REAR] = 0;
    gaitLegNo[LEFT_FRONT] = 2;
    gaitLegNo[RIGHT_REAR] = 6;
    pushSteps = 6;
    stepsInCycle = 8;
  }else if(GaitType == RIPPLE_RIGHT){   // simple ripple for right turns
    gaitGen = &DefaultGaitGen;
    gaitSetup = &DefaultGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 2;
    gaitLegNo[LEFT_REAR] = 6;
    gaitLegNo[LEFT_FRONT] = 4;
    gaitLegNo[RIGHT_REAR] = 0;
    pushSteps = 6;
    stepsInCycle = 8;
  }else if(GaitType == AMBLE){
    gaitGen = &DefaultGaitGen;
    gaitSetup = &AmbleGaitSetup; //&DefaultGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 0;
    gaitLegNo[LEFT_REAR] = 0;
    gaitLegNo[LEFT_FRONT] = 2;
    gaitLegNo[RIGHT_REAR] = 2;
    pushSteps = 2;
    stepsInCycle = 4;
    tranTime = 98;
  }else if(GaitType == SMOOTH_AMBLE){
    gaitGen = &DefaultGaitGen;
    gaitSetup = &DefaultGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 0;
    gaitLegNo[LEFT_REAR] = 1;
    gaitLegNo[LEFT_FRONT] = 2;
    gaitLegNo[RIGHT_REAR] = 3;
    pushSteps = 2;
    stepsInCycle = 4;
    tranTime = 98;
  }
  if(cycleTime == 0)
    cycleTime = (stepsInCycle*tranTime)/1000.0;
  step = 0;
}

ik_req_t (*gaitGen)(int leg) = &DefaultGaitGen;
void (*gaitSetup)() = &DefaultGaitSetup;
