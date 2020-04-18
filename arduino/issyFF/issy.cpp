/******************************************************************************
 * Inverse Kinematics for 4/6 legged bots using 3DOF legs
 *
 *                       ISSY:                
 * FRONT VIEW       ^    L_COXA = 52mm         ==0         0==
 *     /\___/\      |    L_FEMUR = 47mm       |  0==[___]==0  |
 *    /       \     Z    L_TIBIA = 125mm      |               |  
 *
 * TOP VIEW
 *    \       /     ^    Y_COXA = 100mm
 *     \_____/      |    X_COXA = 112mm
 *  ___|     |___   X
 *     |_____|
 *     /     \      Y->
 *    /       \
 *****************************************************************************/

#include "issy.h"
#include <WProgram.h>
#include <math.h>

/* min and max positions for each servo */                                          
int mins[] = {172,372, 182,392, 412,572, 372,172, 392,182, 540,452};
int maxs[] = {652,852, 632,842, 712,840, 852,652, 842,632, 857,740};

#ifdef TASK_FF
  #define X_STANCE 60
  #define Y_STANCE 90
#else
  #define X_STANCE 55
  #define Y_STANCE 110
#endif

/* Setup the starting positions of the legs. */
void setupIK(){
  endpoints[RIGHT_FRONT].x = X_STANCE;  // was 60, sim shows 47/52 as optimal
  endpoints[RIGHT_FRONT].y = Y_STANCE;  // was 90, sim shows 110 as optimal
  endpoints[RIGHT_FRONT].z = STD_HEIGHT;
   
  endpoints[RIGHT_REAR].x = -X_STANCE;
  endpoints[RIGHT_REAR].y = Y_STANCE;
  endpoints[RIGHT_REAR].z = STD_HEIGHT;     
    
  endpoints[LEFT_FRONT].x = X_STANCE;
  endpoints[LEFT_FRONT].y = -Y_STANCE;
  endpoints[LEFT_FRONT].z = STD_HEIGHT;  
    
  endpoints[LEFT_REAR].x = -X_STANCE;
  endpoints[LEFT_REAR].y = -Y_STANCE;
  endpoints[LEFT_REAR].z = STD_HEIGHT;  
    
  stepsInGait = 1;
  gaitStep = 0;
}

BioloidController bioloid = BioloidController(1000000);
ik_req_t endpoints[LEG_COUNT];
float bodyRotX = 0;    // body roll
float bodyRotY = 0;    // body pitch
float bodyRotZ = 0;    // body rotation
int bodyPosX = 0;
int bodyPosY = 0;
int gaitStep;
int gaitInSeq;
int gaitLegNo[LEG_COUNT];
ik_req_t gaits[LEG_COUNT];    // gait position
//int NrLiftedPos;	  
int divFactor;	  
int stepsInGait;
int travelX;
int travelY;
float travelRotZ;
int tranTime;
int LIFT_HEIGHT;

/* Convert radians to servo position offset. */
int radToServo(float rads){ 
  float val = (rads*100)/51 * 100;
  return (int) val; 
}

/* Gait Engine */
ik_req_t (*gaitGen)(int leg) = &DefaultGaitGen;

ik_req_t DefaultGaitGen(int leg){
  if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) ){ // are we moving?
    if(/*(NrLiftedPos == 1 || NrLiftedPos == 3) && */(gaitStep == gaitLegNo[leg])){
      // leg up, middle position
      gaits[leg].x = 0;
      gaits[leg].y = 0;
      gaits[leg].z = -LIFT_HEIGHT;
      gaits[leg].r = 0;
    }else if(((gaitStep == gaitLegNo[leg]+1) || (gaitStep == gaitLegNo[leg]-(stepsInGait-1))) && (gaits[leg].z < 0)){
      // leg down position
      gaits[leg].x = travelX/2;
      gaits[leg].y = travelY/2;
      gaits[leg].z = 0;
      gaits[leg].r = travelRotZ/2;
    }else{
      // move body forward
      gaits[leg].x = gaits[leg].x - (travelX/divFactor);
      gaits[leg].y = gaits[leg].y - (travelY/divFactor);
      gaits[leg].z = 0;
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

ik_req_t QuadRipple(int leg){
  if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) ){ // are we moving?
        if(/*(NrLiftedPos == 1 || NrLiftedPos == 3) && */(gaitStep == gaitLegNo[leg])){
            // leg up, middle position
            gaits[leg].x = 0;
            gaits[leg].y = 0;
            gaits[leg].z = -LIFT_HEIGHT;
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
      tranTime = 70;
    else
      tranTime = 10;
    return gaits[leg];
}

ik_req_t FastTurnGen(int leg){
    if( (travelX > 5 || travelX < -5) || (travelY > 5 || travelY < -5) || (travelRotZ > 0.05 || travelRotZ < -0.05) ){ // are we moving?
        if(gaitStep == gaitLegNo[leg]){
            // leg up, middle position
            gaits[leg].x = 0;
            gaits[leg].y = 0;
            gaits[leg].z = -LIFT_HEIGHT;
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
    }else if(GaitType == FAST_TURN){  // 2 Step Fast Turn
        gaitGen = &FastTurnGen;
        gaitLegNo[RIGHT_FRONT] = 0;
        gaitLegNo[LEFT_REAR] = 0;
        gaitLegNo[LEFT_FRONT] = 3;
        gaitLegNo[RIGHT_REAR] = 3;
        gaitStep = 0;
	//NrLiftedPos = 1;	
	stepsInGait = 6;  
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
}

/* Body IK solver: compute where legs should be. */
ik_req_t bodyIK(int X, int Y, int Z, int Xdisp, int Ydisp, float Zrot){
    ik_req_t ans;
    
    float cosB = cos(bodyRotX);
    float sinB = sin(bodyRotX);
    float cosG = cos(bodyRotY);
    float sinG = sin(bodyRotY);
    float cosA = cos(bodyRotZ+Zrot);
    float sinA = sin(bodyRotZ+Zrot);
    
    int totalX = X + Xdisp + bodyPosX; 
    int totalY = Y + Ydisp + bodyPosY; 
    
    ans.x = totalX - int(totalX*cosG*cosA + totalY*sinB*sinG*cosA + Z*cosB*sinG*cosA - totalY*cosB*sinA + Z*sinB*sinA) + bodyPosX;
    ans.y = totalY - int(totalX*cosG*sinA + totalY*sinB*sinG*sinA + Z*cosB*sinG*sinA + totalY*cosB*cosA - Z*sinB*cosA) + bodyPosY;
    ans.z = Z - int(-totalX*sinG + totalY*sinB*cosG + Z*cosB*cosG); //+ Zrot;
    
//    Serial.print("BodyIK:");
//    Serial.print(ans.x);
//    Serial.print(",");
//    Serial.print(ans.y);
//    Serial.print(",");
//    Serial.println(ans.z);
    
    return ans;
}

/* Simple 3dof leg solver. X,Y,Z are the length from the Coxa rotate to the endpoint. 
   calibrated on right front leg?????? */
ik_sol_t legIK(int X, int Y, int Z){
    ik_sol_t ans;    

    // first, make this a 2DOF problem... by solving coxa
    ans.coxa = radToServo(atan2(X,Y));
    int trueX = sqrt(sq(X)+sq(Y)) - L_COXA;
    int im = sqrt(sq(trueX)+sq(Z));    // length of imaginary leg
    
    // get femur angle above horizon...
    float q1 = -atan2(Z,trueX);
    int d1 = sq(L_FEMUR)-sq(L_TIBIA)+sq(im);
    int d2 = 2*L_FEMUR*im;
    float q2 = acos((float)d1/(float)d2);
    ans.femur = radToServo(q1+q2); 
    
    // and tibia angle from femur...
    d1 = sq(L_FEMUR)-sq(im)+sq(L_TIBIA);
    d2 = 2*L_TIBIA*L_FEMUR;
    ans.tibia = radToServo(acos((float)d1/(float)d2));  
   
//    Serial.print("Coxa:");
//    Serial.println(ans.coxa);  
//    Serial.print("Femur:");
//    Serial.println(ans.femur);   
//    Serial.print("Tibia:");
//    Serial.println(ans.tibia);        
    
    return ans;
}

void doIK(){
    int servo;
    ik_req_t req, gait;    
    ik_sol_t sol;
    
    // right front leg
    gait = gaitGen(RIGHT_FRONT);    
    req = bodyIK(endpoints[RIGHT_FRONT].x+gait.x, endpoints[RIGHT_FRONT].y+gait.y, endpoints[RIGHT_FRONT].z+gait.z, X_COXA, Y_COXA, gait.r);
    sol = legIK(endpoints[RIGHT_FRONT].x+req.x+gait.x,endpoints[RIGHT_FRONT].y+req.y+gait.y,endpoints[RIGHT_FRONT].z+req.z+gait.z);
    servo = 512+sol.coxa;
    if(servo < maxs[RF_COXA-1] && servo > mins[RF_COXA-1])
        bioloid.setNextPose(RF_COXA, servo);
    else{
        Serial.print("RF_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 337+sol.femur; //205+sol.femur;
    if(servo < maxs[RF_FEMUR-1] && servo > mins[RF_FEMUR-1])
        bioloid.setNextPose(RF_FEMUR, servo);
    else{
        Serial.print("RF_FEMUR FAIL: ");
        Serial.println(servo);
    }
    //SetPosition(RF_TIBIA,318);    // 90 degrees - 0.57 == 512 - 307 + 113         422
    servo = 729-sol.tibia; //625-
    if(servo < maxs[RF_TIBIA-1] && servo > mins[RF_TIBIA-1])
        bioloid.setNextPose(RF_TIBIA, servo);
    else{
        Serial.print("RF_TIBIA FAIL: ");
        Serial.println(servo);
    }
    
    // right rear leg - MODIFIED FOR ISSYFF     gait = gaitGen(RIGHT_REAR);
    gait = gaitGen(RIGHT_REAR);    
    req = bodyIK(endpoints[RIGHT_REAR].x+gait.x,endpoints[RIGHT_REAR].y+gait.y, endpoints[RIGHT_REAR].z+gait.z, -X_COXA, Y_COXA, gait.r);
    sol = legIK(-endpoints[RIGHT_REAR].x-req.x-gait.x,endpoints[RIGHT_REAR].y+req.y+gait.y,endpoints[RIGHT_REAR].z+req.z+gait.z);
    servo = 819-sol.coxa;   // was 512
    if(servo < maxs[RR_COXA-1] && servo > mins[RR_COXA-1])
        bioloid.setNextPose(RR_COXA, servo);
    else{
        Serial.print("RR_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 687-sol.femur; //819-
    if(servo < maxs[RR_FEMUR-1] && servo > mins[RR_FEMUR-1])
        bioloid.setNextPose(RR_FEMUR, servo);
    else{
        Serial.print("RR_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 295+sol.tibia; // 399+
    if(servo < maxs[RR_TIBIA-1] && servo > mins[RR_TIBIA-1])
        bioloid.setNextPose(RR_TIBIA, servo);
    else{
        Serial.print("RR_TIBIA FAIL: ");
        Serial.println(servo);
    }
    
    // left front leg
    gait = gaitGen(LEFT_FRONT);
    req = bodyIK(endpoints[LEFT_FRONT].x+gait.x,endpoints[LEFT_FRONT].y+gait.y, endpoints[LEFT_FRONT].z+gait.z, X_COXA, -Y_COXA, gait.r);
    sol = legIK(endpoints[LEFT_FRONT].x+req.x+gait.x,-endpoints[LEFT_FRONT].y-req.y-gait.y,endpoints[LEFT_FRONT].z+req.z+gait.z);
    servo = 819-sol.coxa;   // was 512
    if(servo < maxs[LF_COXA-1] && servo > mins[LF_COXA-1])
        bioloid.setNextPose(LF_COXA, servo);
    else{
        Serial.print("LF_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 687-sol.femur;
    if(servo < maxs[LF_FEMUR-1] && servo > mins[LF_FEMUR-1])
        bioloid.setNextPose(LF_FEMUR, servo);
    else{
        Serial.print("LF_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 295+sol.tibia;
    if(servo < maxs[LF_TIBIA-1] && servo > mins[LF_TIBIA-1])
        bioloid.setNextPose(LF_TIBIA, servo);
    else{
        Serial.print("LF_TIBIA FAIL: ");
        Serial.println(servo);
    }

    // left rear leg
    gait = gaitGen(LEFT_REAR);
    req = bodyIK(endpoints[LEFT_REAR].x+gait.x,endpoints[LEFT_REAR].y+gait.y, endpoints[LEFT_REAR].z+gait.z, -X_COXA, -Y_COXA, gait.r);
    sol = legIK(-endpoints[LEFT_REAR].x-req.x-gait.x,-endpoints[LEFT_REAR].y-req.y-gait.y,endpoints[LEFT_REAR].z+req.z+gait.z);
    servo = 512+sol.coxa;
    if(servo < maxs[LR_COXA-1] && servo > mins[LR_COXA-1])
        bioloid.setNextPose(LR_COXA, servo);
    else{
        Serial.print("LR_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 337+sol.femur;
    if(servo < maxs[LR_FEMUR-1] && servo > mins[LR_FEMUR-1])
        bioloid.setNextPose(LR_FEMUR, servo);
    else{
        Serial.print("LR_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 729-sol.tibia;
    if(servo < maxs[LR_TIBIA-1] && servo > mins[LR_TIBIA-1])
        bioloid.setNextPose(LR_TIBIA, servo);
    else{
        Serial.print("LR_TIBIA FAIL: ");
        Serial.println(servo);
    }
    
    gaitStep = (gaitStep+1)%stepsInGait;
}


/* 
 * Some higher level movement
 */

/** causes robot to move x CM (positive = forward) */
void moveX(int x){
  int steps;
  gaitSelect(QUAD_RIPPLE);
  steps = stepsInGait;
  x = x*10;   // cm->mm
  if(x < 0){
    if(-x < TOP_SPEED)
      travelX = x;
    else{
      travelX = -TOP_SPEED;
      steps = steps * (-x/TOP_SPEED);
    }
  }else{
    if(x < TOP_SPEED)
      travelX = x;
    else{
      travelX = TOP_SPEED;
      steps = steps * (x/TOP_SPEED);
    }
    while(steps > 0){
      doIK();
      bioloid.interpolateSetup(tranTime);
      while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
      }
      steps--;    
    }
  }
}

/** turns angle degees (positive = right) */
void turnX(int angle){    
  SetPosition(HEAD_SERVO, 512);
  //gaitSelect(QUAD_RIPPLE_TURNING);
  gaitSelect(FAST_TURN);
  travelX = 0;
  if(angle > 0){
    travelRotZ = -0.207; // 12 deg -0.174; // 10 degrees
  }else{
    travelRotZ = 0.207;
    angle = -angle;
  } 
  angle = (int) (((float)angle/13.0)*(float)stepsInGait);
  while(angle > 0){
    doIK();
    bioloid.interpolateSetup(tranTime);
    while(bioloid.interpolating > 0){
      bioloid.interpolateStep();
      delay(3);
    }
    angle--;
  }
}

/*
 * Tack on our sensory!
 */

SharpIR headIR = SharpIR(GP2D12,PIN_IR);

int getSonar(){
    int reading = analogRead(PIN_SONAR)/2;
    reading = reading + analogRead(PIN_SONAR)/2;
    reading = reading + analogRead(PIN_SONAR)/2;
    reading = reading + analogRead(PIN_SONAR)/2;
    reading = reading/4;
    return (reading*10)/4;
}

int getPhoto(){
    int reading = analogRead(PIN_PHOTO);
    reading = reading + analogRead(PIN_PHOTO);
    reading = reading + analogRead(PIN_PHOTO);
    reading = reading + analogRead(PIN_PHOTO);
    reading = reading/4;
    return reading;
}

