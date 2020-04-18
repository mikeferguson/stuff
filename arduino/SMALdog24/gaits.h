/******************************************************************************
 * Gaits Auto-Generated by NUKE!
 *   http://arbotix.googlecode.com
 *****************************************************************************/

#define STD_TRANSITION          32
#define MOVING    ((travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05))
//#define MOVING    ((travelX > 0 || travelX < 0) || (travelY > 0 || travelY < 0) || (travelRotZ > 0.0 || travelRotZ < 0.0))

/* Simple, fast, and rough gait. StepsInCycle == leg count * X.
    Legs will make a fast triangular stroke. */
void DefaultGaitSetup(){bodyPosY = 0;};

void ShiftRippleSetup(){
  if( MOVING ) {
    // shift body
    if(step == 2){    // shift off left rear
      bodyPosY = -15;
    }else if(step == 7){
      bodyPosY = 15;  
    }
  }
}

ik_req_t DefaultGaitGen(int leg){
  if( MOVING ){ 
    if(step == gaitLegNo[leg]){
      // leg up, middle position
      gaits[leg].x = 0; //(travelX*pushSteps)/(2*stepsInCycle);
      gaits[leg].y = 0; //(travelY*pushSteps)/(2*stepsInCycle);
      gaits[leg].z = -liftHeight;
      gaits[leg].r = 0; //(travelRotZ*pushSteps)/(2*stepsInCycle);
    }else if(((step == gaitLegNo[leg]+1) || (step == gaitLegNo[leg]-(stepsInCycle-1))) && (gaits[leg].z < 0)){
      // leg down position
      //gaits[leg].x = travelX/2;
      //gaits[leg].y = travelY/2;
      //gaits[leg].z = 0;
      //gaits[leg].r = travelRotZ/2;
      // leg down position                                        NOTE: dutyFactor = pushSteps/StepsInCycle
      gaits[leg].x = (travelX*pushSteps)/(2*stepsInCycle);        // travel/Cycle = speed*cycleTime
      gaits[leg].y = (travelY*pushSteps)/(2*stepsInCycle);        // Stride = travel/Cycle * dutyFactor
      gaits[leg].z = 0;                                           //   = speed*cycleTime*pushSteps/stepsInCycle
      gaits[leg].r = (travelRotZ*pushSteps)/(2*stepsInCycle);     //   we move Stride/2 here
    }else{
      // move body forward
      //gaits[leg].x = gaits[leg].x - (travelX/pushSteps);
      //gaits[leg].y = gaits[leg].y - (travelY/pushSteps);
      //gaits[leg].z = 0;
      //gaits[leg].r = gaits[leg].r - (travelRotZ/pushSteps);
      // move body forward
      gaits[leg].x = gaits[leg].x - (travelX)/stepsInCycle;    // note calculations for Stride above
      gaits[leg].y = gaits[leg].y - (travelY)/stepsInCycle;    // we have to move Stride/pushSteps here
      gaits[leg].z = 0;                                                 //   = speed*cycleTime*pushSteps/stepsInCycle*pushSteps
      gaits[leg].r = gaits[leg].r - (travelRotZ)/stepsInCycle;    //   = speed*cycleTime/stepsInCycle
    }
  }else{ // stopped
    gaits[leg].z = 0;        
  }    
  Serial.print("GaitReq:");
  Serial.print(gaits[leg].x);
  Serial.print(",");
  Serial.print(gaits[leg].y);
  Serial.print(",");
  Serial.print(gaits[leg].z);
  Serial.print(",");
  Serial.println(gaits[leg].r);
  return gaits[leg];
}


/* Geometrically stable gait!
 * leg - 1 (0 mod 4) = shift weight    TT = 100ms
 * leg     (1 mod 4) = lift leg        TT = 65ms
 * leg + 1 (2 mod 4) = shift leg       TT = 65ms
 * leg + 1 (3 mod 4) = plant and push  TT = 200ms
 */
void GeoGaitSetup(){
  if( MOVING ) {
    // compute body position
    tranTime = 100;
    if(step == 0){       // Prepare to move Right Front
      int Y1 = -endpoints[LEFT_FRONT].y - gaits[LEFT_FRONT].y + Y_COXA;
      int Y2 = endpoints[RIGHT_REAR].y + gaits[RIGHT_REAR].y + Y_COXA;
      int X1 = endpoints[LEFT_FRONT].x + gaits[LEFT_FRONT].x + X_COXA;
      int X2 = -endpoints[RIGHT_REAR].x - gaits[RIGHT_REAR].x + X_COXA;
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = 20-SM;  // minimum Stability Margin = 20mm
      bodyRotX = -0.1;
    }else if(step == 4){  // Prepare to move Left Rear
      int Y1 = endpoints[RIGHT_REAR].y + gaits[RIGHT_REAR].y + Y_COXA;
      int Y2 = -endpoints[LEFT_FRONT].y - gaits[LEFT_FRONT].y + Y_COXA;
      int X1 = -endpoints[RIGHT_REAR].x - gaits[RIGHT_REAR].x + X_COXA;
      int X2 = endpoints[LEFT_FRONT].x + gaits[LEFT_FRONT].x + X_COXA;
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = -20-SM;
      bodyRotX = 0.1;
    }else if(step == 8){  // Prepare to move Left Front
      int Y1 = endpoints[RIGHT_FRONT].y + gaits[RIGHT_FRONT].y + Y_COXA;
      int Y2 = -endpoints[LEFT_REAR].y - gaits[LEFT_REAR].y + Y_COXA;
      int X1 = endpoints[RIGHT_FRONT].x + gaits[RIGHT_FRONT].x + X_COXA; 
      int X2 = -endpoints[LEFT_REAR].x - gaits[LEFT_REAR].x + X_COXA; 
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = -20+SM;
      bodyRotX = 0.1;
    }else if(step == 12){  // Prepare to move Right Rear
      int Y1 = -endpoints[LEFT_REAR].y - gaits[LEFT_REAR].y + Y_COXA;
      int Y2 = endpoints[RIGHT_FRONT].y + gaits[RIGHT_FRONT].y + Y_COXA;
      int X1 = -endpoints[LEFT_REAR].x - gaits[LEFT_REAR].x + X_COXA;
      int X2 = endpoints[RIGHT_FRONT].x + gaits[RIGHT_FRONT].x + X_COXA;
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = 20-SM;
      bodyRotX = -0.1;
    }else if((step%4) == 3){
      tranTime = 200;
    }else{
      tranTime = 65;
    }
  }else{
    bodyPosY = 0;
    bodyRotX = 0;
    tranTime = 100;
  }
}

/* Slower, methodically balanced gait. StepsInCycle == leg count * 2.
    Legs will make a fast triangular stroke. */
ik_req_t GeoGaitGen(int leg){
  if( MOVING ){ 
    // are we moving?
    if(step == gaitLegNo[leg]){
      // leg up, rear position
      gaits[leg].z = -liftHeight;
    }else if((step == gaitLegNo[leg] + 1) && gaits[leg].z < 0){
      // leg forward, still up
      gaits[leg].x = travelX/2;
      gaits[leg].y = travelY/2;
      gaits[leg].r = travelRotZ/2;
    }else if((step == gaitLegNo[leg] + 2)){
      gaits[leg].z = 0;
    }else if((step%4) == 3){
      // move body forward
      gaits[leg].x = gaits[leg].x - (travelX/pushSteps);
      gaits[leg].y = gaits[leg].y - (travelY/pushSteps);
      gaits[leg].z = 0;
      gaits[leg].r = gaits[leg].r - (travelRotZ/pushSteps);
    }
  }else{ // stopped
    gaits[leg].z = 0;        
  }    
  /*Serial.print("GaitReq:");
  Serial.print(gaits[leg].x);
  Serial.print(",");
  Serial.print(gaits[leg].y);
  Serial.print(",");
  Serial.print(gaits[leg].z);
  Serial.print(",");
  Serial.println(gaits[leg].r);*/
  return gaits[leg];
}

/* A Smoother Geometric Gait 
 * As we move faster, we tuck the legs in closer, so we don't have to do as much 
 * torso shift 
 *  leg - 1 (0 mod 4) = shift weight    TT = 100ms
 *  leg     (1 mod 4) = lift leg        TT = 65ms
 *  leg + 1 (2 mod 4) = shift leg       TT = 65ms
 *  leg + 1 (3 mod 4) = plant and push  TT = 200ms
 * travelX can go between 0 and 150cm
 */ 
void SmoothGeoSetup(){
  if( MOVING ) {
    // compute body position
    tranTime = 100;
    if(step == 0){       // Prepare to move Right Front
      int Y1 = -endpoints[LEFT_FRONT].y - gaits[LEFT_FRONT].y + Y_COXA;
      int Y2 = endpoints[RIGHT_REAR].y + gaits[RIGHT_REAR].y + Y_COXA;
      int X1 = endpoints[LEFT_FRONT].x + gaits[LEFT_FRONT].x + X_COXA;
      int X2 = -endpoints[RIGHT_REAR].x - gaits[RIGHT_REAR].x + X_COXA;
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = 20-SM;  // minimum Stability Margin = 20mm
      bodyRotX = -0.1;
    }else if(step == 4){  // Prepare to move Left Rear
      int Y1 = endpoints[RIGHT_REAR].y + gaits[RIGHT_REAR].y + Y_COXA;
      int Y2 = -endpoints[LEFT_FRONT].y - gaits[LEFT_FRONT].y + Y_COXA;
      int X1 = -endpoints[RIGHT_REAR].x - gaits[RIGHT_REAR].x + X_COXA;
      int X2 = endpoints[LEFT_FRONT].x + gaits[LEFT_FRONT].x + X_COXA;
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = -20-SM;
      bodyRotX = 0.1;
    }else if(step == 8){  // Prepare to move Left Front
      int Y1 = endpoints[RIGHT_FRONT].y + gaits[RIGHT_FRONT].y + Y_COXA;
      int Y2 = -endpoints[LEFT_REAR].y - gaits[LEFT_REAR].y + Y_COXA;
      int X1 = endpoints[RIGHT_FRONT].x + gaits[RIGHT_FRONT].x + X_COXA; 
      int X2 = -endpoints[LEFT_REAR].x - gaits[LEFT_REAR].x + X_COXA; 
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = -20+SM;
      bodyRotX = 0.1;
    }else if(step == 12){  // Prepare to move Right Rear
      int Y1 = -endpoints[LEFT_REAR].y - gaits[LEFT_REAR].y + Y_COXA;
      int Y2 = endpoints[RIGHT_FRONT].y + gaits[RIGHT_FRONT].y + Y_COXA;
      int X1 = -endpoints[LEFT_REAR].x - gaits[LEFT_REAR].x + X_COXA;
      int X2 = endpoints[RIGHT_FRONT].x + gaits[RIGHT_FRONT].x + X_COXA;
      int SM = ((Y1 + Y2)*X1)/(X1+X2) - Y1;
      bodyPosY = 20-SM;
      bodyRotX = -0.1;
    }else if((step%4) == 3){
      tranTime = 200;
    }else{
      tranTime = 65;
    }
  }else{
    bodyPosY = 0;
    bodyRotX = 0;
    tranTime = 100;
  }
}

// Set the Y position 
void setStance(int stance){
  gaits[RIGHT_FRONT].y = gaits[RIGHT_FRONT].y + (endpoints[RIGHT_FRONT].y -stance);
  endpoints[RIGHT_FRONT].y = stance;
  gaits[RIGHT_REAR].y = gaits[RIGHT_REAR].y + (endpoints[RIGHT_REAR].y -stance);
  endpoints[RIGHT_REAR].y = stance;
  gaits[LEFT_FRONT].y = gaits[LEFT_FRONT].y + (endpoints[LEFT_FRONT].y +stance);
  endpoints[LEFT_FRONT].y = -stance;
  gaits[LEFT_REAR].y = gaits[LEFT_REAR].y + (endpoints[LEFT_REAR].y +stance);
  endpoints[LEFT_REAR].y = -stance;
}

/* Select which gait to run */
void gaitSelect(int GaitType){
  tranTime = STD_TRANSITION;
  bodyPosX = 0;
  if(GaitType == SHIFT_RIPPLE){               // simple ripple with body shift, 10 steps
    gaitGen = &DefaultGaitGen;
    gaitSetup = &ShiftRippleSetup;;
    gaitLegNo[RIGHT_FRONT] = 0;
    gaitLegNo[LEFT_REAR] = 3;
    gaitLegNo[LEFT_FRONT] = 5;
    gaitLegNo[RIGHT_REAR] = 8;
    pushSteps = 8;	
    stepsInCycle = 10;  
    setStance(30);
  }else if(GaitType == RIPPLE){               // simple ripple, 8 steps
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
  }else if(GaitType == RIPPLE_GEO){
    gaitGen = &GeoGaitGen;
    gaitSetup = &GeoGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 1;
    gaitLegNo[LEFT_REAR] = 5;
    gaitLegNo[LEFT_FRONT] = 9;
    gaitLegNo[RIGHT_REAR] = 13;
    pushSteps = 4;	
    stepsInCycle = 16;  
    liftHeight = 50;
  }else if(GaitType == AMBLE){          // fast step, 2 feet at a time
    gaitGen = &DefaultGaitGen;
    gaitSetup = &DefaultGaitSetup;
    gaitLegNo[RIGHT_FRONT] = 0;
    gaitLegNo[LEFT_REAR] = 0;
    gaitLegNo[LEFT_FRONT] = 2;
    gaitLegNo[RIGHT_REAR] = 2;
    pushSteps = 2;	
    stepsInCycle = 4;
    tranTime = 98;  
    setStance(50);
  }
  step = 0;
}

ik_req_t (*gaitGen)(int leg) = &DefaultGaitGen;
void (*gaitSetup)() = &DefaultGaitSetup;


// OLD STUFF // 


/*
ik_req_t QuadRipple(int leg){
  if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) || (WALK_IN_PLACE > 0)){ // are we moving?
        if((gaitStep == gaitLegNo[leg])){
            // leg up, middle position
            gaits[leg].x = 0;
            gaits[leg].y = 0;
            gaits[leg].z = -20;
            gaits[leg].r = 0;
        }else if(((gaitStep == gaitLegNo[leg]+1) || (gaitStep == gaitLegNo[leg]-(stepsInGait-1))) && (gaits[leg].z < 0)){
            //int h = sqrt(sq(endpoints[leg].x) + sq(endpoints[leg].y));
            // leg down position
            gaits[leg].x = travelX/2 ;
            gaits[leg].y = travelY/2 ;
            gaits[leg].z = 0;
            gaits[leg].r = travelRotZ/2;
        }else{
            //int h = sqrt(sq(endpoints[leg].x+gaits[leg].x) + sq(endpoints[leg].y+gaits[leg].y)); 
            // move body forward
            gaits[leg].x = gaits[leg].x - (travelX/divFactor);
            gaits[leg].y = gaits[leg].y - (travelY/divFactor);
            gaits[leg].z = 0;
            gaits[leg].r = gaits[leg].r - (travelRotZ/divFactor);
        }
    }else{ // stopped
        gaits[leg].z = 0;        
    }    
    if(gaitStep%2 == 0)
      tranTime = 60;
    else
      tranTime = 100;
    return gaits[leg];
}

ik_req_t FastTurnGen(int leg){
    if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) ){ // are we moving?
        if(gaitStep == gaitLegNo[leg]){
            // leg up, middle position
            gaits[leg].x = 0;
            gaits[leg].y = 0;
            gaits[leg].z = -20;
            gaits[leg].r = 0;
        }else if(((gaitStep == gaitLegNo[leg]+1) || (gaitStep == gaitLegNo[leg]-(stepsInGait-1))) && (gaits[leg].z < 0)){
            // leg down position
            gaits[leg].x = travelX/2 ;
            gaits[leg].y = travelY/2 ;
            gaits[leg].z = 0;
            gaits[leg].r = travelRotZ/2;
        }else if(gaitStep == 2 or gaitStep == 4){
            //int h = sqrt(sq(endpoints[leg].x+gaits[leg].x) + sq(endpoints[leg].y+gaits[leg].y)); 
            // move body forward
            gaits[leg].x = gaits[leg].x - (travelX/2);
            gaits[leg].y = gaits[leg].y - (travelY/2);
            gaits[leg].z = 0;
            gaits[leg].r = gaits[leg].r - (travelRotZ/2);
        }
    }else{ // stopped
        gaits[leg].z = 0;        
    }    
    if(gaitStep%3 == 0)       // lift
      tranTime = 70;
    else if(gaitStep%3 == 2)  // shift
      tranTime = 10;
    else                      // drop
      tranTime = 10;
    return gaits[leg];
}

ik_req_t DiscoGait(int leg){
  if( travelX > 0 ){
    if(gaitStep == gaitLegNo[leg]){
      // leg up
      gaits[leg].x = gaits[leg].x + 80;
      gaits[leg].z = -20;
    }else if(gaitStep == 4 || gaitStep ==9){
      // push
      gaits[leg].x = gaits[leg].x - 40;   
    }else{
      // nothing   
      gaits[leg].z = 0; 
    }
  }
  return gaits[leg];
}

ik_req_t DiscoSwingGait(int leg){
  if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) ){ // are we moving?
    if(gaitStep == gaitLegNo[leg]){  // 0,3,
      // leg up
      gaits[leg].x = 0;
      gaits[leg].z = -20; 
      tranTime = 30;
    }else if(gaitStep == (gaitLegNo[leg] + 1)){ // 1, 4
      // leg forward
      gaits[leg].x = travelX/2;
      gaits[leg].z = -20; 
      tranTime = 50;
    }else if(gaitStep == 6 || gaitStep == 13){
      // push
      gaits[leg].x = gaits[leg].x - travelX/2;    
      tranTime = 50;
    }else{  // 2, 5
      // nothing   
      gaits[leg].z = 0; 
    }
  } 
      tranTime = 50;
  //tranTime = 1250;
  return gaits[leg];
}

ik_req_t CrazyWalk(int leg){
  if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) ){ // are we moving?
    if(/*(NrLiftedPos == 1 || NrLiftedPos == 3) && /(gaitStep == gaitLegNo[leg])){
      // leg up, middle position
      gaits[leg].x = travelX/4;
      gaits[leg].y = travelY/4;
      gaits[leg].z = -40;
      gaits[leg].r = travelRotZ/4;
    }else if(((gaitStep == gaitLegNo[leg]+1) || (gaitStep == gaitLegNo[leg]-(stepsInGait-1))) && (gaits[leg].z < 0)){
      // leg down position
      gaits[leg].x = travelX/2;
      gaits[leg].y = travelY/2;
      gaits[leg].z = sqrt(sq(160)-sq(gaits[leg].x))-160;
      gaits[leg].r = travelRotZ/2;
    }else{
      // move body forward
      gaits[leg].x = gaits[leg].x - (travelX/divFactor);
      gaits[leg].y = gaits[leg].y - (travelY/divFactor);
      gaits[leg].z = sqrt(sq(160)-sq(gaits[leg].x))-160;
      gaits[leg].r = gaits[leg].r - (travelRotZ/divFactor);
    }
  }else{ // stopped
    gaits[leg].z = 0;        
  }    
//  Serial.print("GaitReq:");
//  Serial.print(gaits[leg].x);
//  Serial.print(",");
//  Serial.print(gaits[leg].y);
//  Serial.print(",");
//  Serial.print(gaits[leg].z);
//  Serial.print(",");
//  Serial.println(gaits[leg].r);
  return gaits[leg];
}


void gaitSelect(int GaitType){
    if (GaitType == QUAD_RIPPLE_TURNING){     // Quad Ripple Turning Gait 8 steps
        gaitGen = &DefaultGaitGen;
        //gaitGen = &QuadRipple;
        gaitLegNo[RIGHT_FRONT] = 4;
        gaitLegNo[LEFT_REAR] = 0;
        gaitLegNo[LEFT_FRONT] = 2;
        gaitLegNo[RIGHT_REAR] = 6;
        gaitStep = 0;
	//NrLiftedPos = 1;	  
	divFactor = 6;	
	stepsInGait = 8; 
        tranTime = 125;
    }else if(GaitType == QUAD_RIPPLE){  // Quad Ripple Gait 8 steps
        gaitGen = &QuadRipple;
        gaitLegNo[RIGHT_FRONT] = 0;
        gaitLegNo[LEFT_REAR] = 2;
        gaitLegNo[LEFT_FRONT] = 4;
        gaitLegNo[RIGHT_REAR] = 6;
        gaitStep = 0;
	//NrLiftedPos = 1;	  
	divFactor = 6;	
	stepsInGait = 8;  
    }else if(GaitType == ROMPLE){  // Discontinuous 3-point Ripple
        gaitGen = &DiscoSwingGait;
        gaitLegNo[RIGHT_FRONT] = 0;
        gaitLegNo[LEFT_REAR] = 3;
        gaitLegNo[LEFT_FRONT] = 7;
        gaitLegNo[RIGHT_REAR] = 10;
        gaitStep = 0;
	stepsInGait = 14;  
    }else{
        gaitLegNo[RIGHT_REAR] = 0; // lower on 1
        gaitLegNo[RIGHT_FRONT] = 2; // lower on 3
        // push on 4
        gaitLegNo[LEFT_REAR] = 5; // lower on 6
        gaitLegNo[LEFT_FRONT] = 7; // lower on 8
        // push on 9
        gaits[RIGHT_REAR].x = -80;
        gaits[RIGHT_FRONT].x = 0;
        gaits[LEFT_FRONT].x = 40;
        gaits[LEFT_REAR].x = -40;
        stepsInGait = 10; 
    }
    gaitGen = &CrazyWalk;
    gaitLegNo[RIGHT_FRONT] = 0;
    gaitLegNo[LEFT_REAR] = 0;
    gaitLegNo[LEFT_FRONT] = 2;
    gaitLegNo[RIGHT_REAR] = 2;   
	divFactor = 2;	
	stepsInGait = 4;  
tranTime = 50;
    
*/