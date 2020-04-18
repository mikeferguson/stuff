#include <ax12.h>
#include <BioloidController.h>
#include <WProgram.h>
#include <math.h>
#include "nuke.h"

/* min and max positions for each servo */
int mins[] = {342, 386, 234, 423, 500, 159, 419, 104, 375, 388};
int maxs[] = {640, 664, 598, 789, 921, 524, 883, 596, 622, 618};

/* 
 * IK Engine
 */
BioloidController bioloid = BioloidController(1000000);
ik_req_t endpoints[2];
float bodyRotX;                 // body roll (rad) (for 5/6DOF)
float bodyRotY;                 // body pitch (rad) 
float bodyRotZ;                 // body rotation (rad) (for 4/6DOF)
int bodyPosX;                   // body offset (mm)
int bodyPosY;                   // body offset (mm)

int Xspeed;                     // forward speed (mm/s)
int Yspeed;                     // sideward speed (mm/s)
float Rspeed;                   // rotation speed (rad/s)

/* Parameters passed from ground leg to swing leg */
int gLength;   // length of YZ-projected ground-side leg
float bLength; // length of YZ-projected body
float ankle;   // on-ground ankle
int projX;     //

/* 
 * Configure stuff at startup
 */
void setupIK(){
    /* Leg Parameters */
    endpoints[LEFT_LEG].y = -(BODY_WIDTH+5);
    endpoints[RIGHT_LEG].y = BODY_WIDTH+5;
    endpoints[LEFT_LEG].z = L_ANKLE+L_TIBIA+(5*L_FEMUR)/6;
    endpoints[RIGHT_LEG].z = L_ANKLE+L_TIBIA+(5*L_FEMUR)/6;
    /* Body Parameters */
    bodyPosX = 0;
    bodyPosY = 0;
    bodyRotX = 0;
    bodyRotY = 0;
    bodyRotZ = 0;   // 4/6 only
    /* Gait Parameters */   
    step = 0;  
    Xspeed = 0;
    Yspeed = 0;     // no good on 4DOF
    Rspeed = 0;     
}

#include "gaits.h"

/* Convert radians to servo position offset. */
int radToServo(float rads){ 
  float val = (rads*100)/51 * 100;
  return (int) val; 
}

/* The ground leg is presumed to be completely flat on the ground
 * we then compute the leg parameters to locate the COR at the correct
 * location:
 *  x,y,z defines the offset of the foot COR from the body COR. 
 *  rG is the Z rotation of the leg (for 4/6DOF only!)
 *  rX is the body rotation about the X axis (for 5/6DOF only!)
 */
ik_sol_t groundLegIK(int x, int y, int z, float rG, float rX){
    ik_sol_t ans;
#if defined (IK_4_DOF) || defined (IK_6_DOF)
    // Get ROTATE - this one is easy
    //ans.rotate = radToServo(rG);
    //projX = (int)(sin(rG) * L_WIDTHf/2);
    //int projY = (int)(cos(rG) * L_WIDTHf/2);   
#endif
    // get ROLL servos
    int trueZ = z-L_ANKLE;
    ans.ankle_roll = radToServo(atan2(y,trueZ));    
    ans.hip_roll = ans.ankle_roll + radToServo(rX);
    // now a 2D problem, in the X,Z plane
    trueZ = sqrt(sq(y)+sq(trueZ))-L_1-L_2;
    int im = sqrt(sq(trueZ)+sq(x));     // length of imaginary leg in X-Z plane
    // get femur angle from horizontal...
    float q1 = atan2(x,trueZ);
    int d1 = sq(L_FEMUR)-sq(L_TIBIA)+sq(im);
    int d2 = 2*L_FEMUR*im;
    float q2 = acos((float)d1/(float)d2);
    ans.hip = radToServo(q1+q2);
    // and tibia angle from femur...
    d1 = sq(L_FEMUR)-sq(im)+sq(L_TIBIA);
    d2 = 2*L_TIBIA*L_FEMUR;
    ans.knee = radToServo(acos((float)d1/(float)d2));
    // and ankle angle...       (614 = 180 deg in AX-12 rotation)
    ans.ankle = 614 - ans.knee - ans.hip;
    ans.hip = ans.hip - radToServo(bodyRotY);
    // clean it up a bit (knee is actually, knee - 180deg)
    ans.knee = ans.knee - 614;

    //Serial.println("Ground IK:");
#if defined (IK_4_DOF) || defined (IK_6_DOF)
    //Serial.print("hipRotate:");Serial.println(ans.rotate);
#endif
    //Serial.print("roll/hip:"); Serial.print(ans.hip_roll);Serial.print(",");Serial.println(ans.hip);
    //Serial.print("knee:"); Serial.println(ans.knee);
    //Serial.print("roll/ankle:"); Serial.print(ans.ankle_roll);Serial.print(",");Serial.println(ans.ankle);    
    
    return ans;      
}

/* A swinging leg is a bit more free:
 *  x,y,z defines the offset of the foot COR from the body COR. 
 *  rG is the Z rotation of the leg (for 4/6DOF only!)
 *  rX is the body rotation about the X axis (for 5/6DOF only!)
 *  fXr is the foot rotation about X
 *  fYr is the foot rotation about Y
 */
ik_sol_t swingLegIK(int x, int y, int z, float rG, float rX, float fXr, float fYr){
    ik_sol_t ans;
#if defined (IK_4_DOF) || defined (IK_6_DOF)
    // Get ROTATE - this one is easy
    //ans.rotate = radToServo(rG);
    //projX = (int)(sin(rG) * L_WIDTHf/2);
    //int projY = (int)(cos(rG) * L_WIDTHf/2);   
#endif
    // get ROLL servos
    int trueZ = z-(cos(fXr)*L_ANKLE);
    ans.ankle_roll = radToServo(atan2(y,trueZ));    
    ans.hip_roll = ans.ankle_roll + radToServo(rX);
    ans.ankle_roll += radToServo(fXr); 
    // now a 2D problem, in the X,Z plane
    trueZ = sqrt(sq(y)+sq(trueZ))-L_1-L_2;
    int im = sqrt(sq(trueZ)+sq(x));     // length of imaginary leg in X-Z plane
    // get femur angle from horizontal...
    float q1 = atan2(x,trueZ);
    int d1 = sq(L_FEMUR)-sq(L_TIBIA)+sq(im);
    int d2 = 2*L_FEMUR*im;
    float q2 = acos((float)d1/(float)d2);
    ans.hip = radToServo(q1+q2);
    // and tibia angle from femur...
    d1 = sq(L_FEMUR)-sq(im)+sq(L_TIBIA);
    d2 = 2*L_TIBIA*L_FEMUR;
    ans.knee = radToServo(acos((float)d1/(float)d2));
    // and ankle angle...       (614 = 180 deg in AX-12 rotation)
    ans.ankle = 614 - ans.knee - ans.hip;   
    // clean it up a bit (knee is actually, knee - 180deg)
    ans.knee = ans.knee - 614;
    ans.hip = ans.hip - radToServo(bodyRotY);
    // add foot Y rotation
    ans.ankle += radToServo(fYr);

    //Serial.println("Swing IK:");
#if defined (IK_4_DOF) || defined (IK_6_DOF)
    //Serial.print("hipRotate:");Serial.println(ans.rotate);
#endif
    //Serial.print("roll/hip:"); Serial.print(ans.hip_roll);Serial.print(",");Serial.println(ans.hip);
    //Serial.print("knee:"); Serial.println(ans.knee);
    //Serial.print("roll/ankle:"); Serial.print(ans.ankle_roll);Serial.print(",");Serial.println(ans.ankle);    
    
    return ans; 
}

/* 
 * Compute legs at this time
 */
void doIK(){
    ik_sol_t left;
    ik_sol_t right;
    int servo;

    //*****************************************************
    // compute IK
    int swingLeg = gaitGen();
    // rest of body IK?
    int dz = (int)(sin(bodyRotX)*(float)BODY_WIDTH);
    int dy = (int)(cos(bodyRotX)*(float)BODY_WIDTH-BODY_WIDTH);
    if(swingLeg == RIGHT_LEG){
      // left side is on ground
      left = groundLegIK(endpoints[LEFT_LEG].x-bodyPosX+gaits[LEFT_LEG].x, -endpoints[LEFT_LEG].y+bodyPosY-BODY_WIDTH-gaits[LEFT_LEG].y+dy, endpoints[LEFT_LEG].z+gaits[LEFT_LEG].z+dz, gaits[LEFT_LEG].r,-bodyRotX);
      right = swingLegIK(endpoints[RIGHT_LEG].x-bodyPosX+gaits[RIGHT_LEG].x, endpoints[RIGHT_LEG].y-bodyPosY-BODY_WIDTH+gaits[RIGHT_LEG].y+dy, endpoints[RIGHT_LEG].z+gaits[RIGHT_LEG].z-dz, gaits[RIGHT_LEG].r,bodyRotX,0,0); 
    }else{
      right = groundLegIK(endpoints[RIGHT_LEG].x-bodyPosX+gaits[RIGHT_LEG].x, endpoints[RIGHT_LEG].y-bodyPosY-BODY_WIDTH+gaits[RIGHT_LEG].y+dy, endpoints[RIGHT_LEG].z+gaits[RIGHT_LEG].z-dz, gaits[RIGHT_LEG].r,bodyRotX); 
      left = swingLegIK(endpoints[LEFT_LEG].x-bodyPosX+gaits[LEFT_LEG].x, -endpoints[LEFT_LEG].y+bodyPosY-BODY_WIDTH-gaits[LEFT_LEG].y+dy, endpoints[LEFT_LEG].z+gaits[LEFT_LEG].z+dz, gaits[LEFT_LEG].r,-bodyRotX,0,0);
    }
    
    //*****************************************************
    // Left leg output
#if defined (IK_4_DOF) || defined (IK_6_DOF)
    servo = 512 + left.rotate;
    if(servo < maxs[LEFT_ROTATE-1] && servo > mins[LEFT_ROTATE-1]){
      bioloid.setNextPose(LEFT_ROTATE, servo);
    }else{
      Serial.print("LEFT_ROTATE FAIL: ");
      Serial.println(servo);
    }
#endif
    servo = 472 - left.hip; // positive hip means move forward (farther away from 512)
    if(servo < maxs[LEFT_HIP-1] && servo > mins[LEFT_HIP-1]){
      bioloid.setNextPose(LEFT_HIP, servo);
    }else{
      Serial.print("LEFT_HIP FAIL: ");
      Serial.println(servo);
    }
#if defined (IK_5_DOF) || defined (IK_6_DOF)
    servo = 512 + left.hip_roll; // positive roll = move outward
    if(servo < maxs[LEFT_HIP_ROLL-1] && servo > mins[LEFT_HIP_ROLL-1]){
      bioloid.setNextPose(LEFT_HIP_ROLL, servo);
    }else{
      Serial.print("LEFT_HIP_ROLL FAIL: ");
      Serial.println(servo);
    }
#endif
    servo = 452 + left.knee;
    if(servo < maxs[LEFT_KNEE-1] && servo > mins[LEFT_KNEE-1]){
      bioloid.setNextPose(LEFT_KNEE, servo);
    }else{
      Serial.print("LEFT_KNEE FAIL: ");
      Serial.println(servo);
    }
#if defined (IK_5_DOF) || defined (IK_6_DOF)
    servo = 547 + left.ankle; // positive ankle = move farther forward, farther away from 512
    if(servo < maxs[LEFT_ANKLE-1] && servo > mins[LEFT_ANKLE-1]){
      bioloid.setNextPose(LEFT_ANKLE, servo);
    }else{
      Serial.print("LEFT_ANKLE FAIL: ");
      Serial.println(servo);
    }   
#endif
    servo = 512 + left.ankle_roll;  // positive roll = move inward
    if(servo < maxs[LEFT_ANKLE_ROLL-1] && servo > mins[LEFT_ANKLE_ROLL-1]){
      bioloid.setNextPose(LEFT_ANKLE_ROLL, servo);
    }else{
      Serial.print("LEFT_ANKLE_ROLL FAIL: ");
      Serial.println(servo);
    }   
        
    //*****************************************************
    // Right leg output
#if defined (IK_4_DOF) || defined (IK_6_DOF)
    servo = 512 + right.rotate;
    if(servo < maxs[RIGHT_ROTATE-1] && servo > mins[RIGHT_ROTATE-1]){
      bioloid.setNextPose(RIGHT_ROTATE, servo);
    }else{
      Serial.print("RIGHT_ROTATE FAIL: ");
      Serial.println(servo);
    }
#endif
    servo = 552 + right.hip; // positive hip means move forward (farther away from 512)
    if(servo < maxs[RIGHT_HIP-1] && servo > mins[RIGHT_HIP-1]){
      bioloid.setNextPose(RIGHT_HIP, servo);
    }else{
      Serial.print("RIGHT_HIP FAIL: ");
      Serial.println(servo);
    }
#if defined (IK_5_DOF) || defined (IK_6_DOF)
    servo = 512 - right.hip_roll;  // positive roll = move outward
    if(servo < maxs[RIGHT_HIP_ROLL-1] && servo > mins[RIGHT_HIP_ROLL-1]){
      bioloid.setNextPose(RIGHT_HIP_ROLL, servo);
    }else{
      Serial.print("RIGHT_HIP_ROLL FAIL: ");
      Serial.println(servo);
    }
#endif
    servo = 572 - right.knee;
    if(servo < maxs[RIGHT_KNEE-1] && servo > mins[RIGHT_KNEE-1]){
      bioloid.setNextPose(RIGHT_KNEE, servo);
    }else{
      Serial.print("RIGHT_KNEE FAIL: ");
      Serial.println(servo);
    }
#if defined (IK_5_DOF) || defined (IK_6_DOF)
    servo = 477 - right.ankle; // positive ankle = move farther forward, farther away from 512
    if(servo < maxs[RIGHT_ANKLE-1] && servo > mins[RIGHT_ANKLE-1]){
      bioloid.setNextPose(RIGHT_ANKLE, servo);
    }else{
      Serial.print("RIGHT_ANKLE FAIL: ");
      Serial.println(servo);
    }
#endif
    servo = 512 - right.ankle_roll; // positive roll = move inward
    if(servo < maxs[RIGHT_ANKLE_ROLL-1] && servo > mins[RIGHT_ANKLE_ROLL-1]){
      bioloid.setNextPose(RIGHT_ANKLE_ROLL, servo);
    }else{
      Serial.print("RIGHT_ANKLE_ROLL FAIL: ");
      Serial.println(servo);
    }   
}
