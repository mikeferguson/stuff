#include <ax12.h>
#include <BioloidController.h>
#include <WProgram.h>
#include <math.h>
#include "nuke.h"

/* min and max positions for each servo */
//int mins[] = {391, 335, 247, 171, 421, 291, 313, 298, 217, 159, 422, 323,   0, 302, 410, 410, 341, 400};
//int maxs[] = {662, 671, 851, 773, 837, 604, 726, 684, 862, 749, 764, 604, 980, 722, 614, 614, 683, 700};
int mins[] = {300, 300, 247, 171, 421, 291, 300, 300, 217, 159, 422, 323,   0, 302, 410, 410, 341, 400};
int maxs[] = {720, 720, 851, 773, 837, 604, 720, 720, 862, 749, 764, 604, 980, 722, 614, 614, 683, 700};

/* IK Engine */
BioloidController bioloid = BioloidController(1000000);
ik_req_t endpoints[LEG_COUNT];
float bodyRotX = 0;             // body roll (rad)
float bodyRotY = 0;             // body pitch (rad)
float bodyRotZ = 0;             // body rotation (rad)
int bodyPosX = 0;               // body offset (mm)
int bodyPosY = 0;               // body offset (mm)
int spinePosY = 0;
int Xspeed;                     // forward speed (mm/s)
int Yspeed;                     // sideward speed (mm/s)
float Rspeed;                   // rotation speed (rad/s)

float twist;                    // spine twist servo position

/* Gait Engine */
int gaitLegNo[LEG_COUNT];       // order to step through legs
ik_req_t gaits[LEG_COUNT];      // gait engine output
int pushSteps;                  // how much of the cycle we are on the ground
int stepsInCycle;               // how many steps in this cycle
int step;                       // current step
int tranTime;
int liftHeight;
float cycleTime;                // cycle time in seconds (adjustment from speed to step-size)

/* Setup the starting positions of the legs. */
void setupIK(){
  endpoints[RIGHT_FRONT].x = 0;
  endpoints[RIGHT_FRONT].y = 80; //100; //115;
  endpoints[RIGHT_FRONT].z = 140; //120; //100;

  endpoints[RIGHT_REAR].x = 0; // -20
  endpoints[RIGHT_REAR].y = 80;
  endpoints[RIGHT_REAR].z = 140;

  endpoints[LEFT_FRONT].x = 0;
  endpoints[LEFT_FRONT].y = -80;
  endpoints[LEFT_FRONT].z = 140;

  endpoints[LEFT_REAR].x = 0;
  endpoints[LEFT_REAR].y = -80;
  endpoints[LEFT_REAR].z = 140;

  twist = 0.0;
  liftHeight = 30;
  stepsInCycle = 1;
  step = 0;
}

#include "gaits.h"

/* Convert radians to servo position offset. */
int radToServo(float rads){
  float val = (rads*100)/51 * 100;
  return (int) val;
}

/* Spine IK solver: computer where spine should be. */
void spineIK(){
  twist = atan2(spinePosY, X_COXA);
  // solve 2 spine servos (only need to check one, they're symmetrical 
  int servo = 512 + radToServo(twist);
  if(servo < maxs[BODY_S1-1] && servo > mins[BODY_S1-1]){
    bioloid.setNextPose(BODY_S1, servo);
    bioloid.setNextPose(BODY_S2, 512 - radToServo(twist));
    // solve tail servos
    bioloid.setNextPose(TAIL_S1, 512 - radToServo(twist));
    bioloid.setNextPose(TAIL_S2, 512 - radToServo(2*twist));
  }else{
    Serial.print("BODY_S1 FAIL: ");
    Serial.println(servo);
  }
}

/* Body IK solver: compute where legs should be. */
ik_req_t bodyIK(int X, int Y, int Z, int Xdisp, int X2disp, int Ydisp, float Zrot, float Brot){
    ik_req_t ans;

    // any twist in the spine shortens it, add back for it...
    int Xspine = X2disp - (int) ( cos(Brot) * (float) X2disp ) + (int) ( sin(Brot) * (float) Ydisp );  
    // any twist in the spine reduces the Y-width, need to add back in for it...
    int Yspine = Ydisp - (int)( cos(Brot) * (float) Ydisp );
    
    // compute X/Y displace with spinal twist
    //int Xspine = (int) ( cos(Brot) * (float) X2disp + sin(Brot) * (float) Ydisp ) - X2disp; 
    //int Yspine = (int) ( cos(Brot) * (float) Ydisp  - sin(Brot) * (float) X2disp ) - Ydisp;
    
    float cosB = cos(bodyRotX);
    float sinB = sin(bodyRotX);
    float cosG = cos(bodyRotY);
    float sinG = sin(bodyRotY);
    float cosA = cos(bodyRotZ+Zrot);
    float sinA = sin(bodyRotZ+Zrot);

    int totalX = X + Xdisp + X2disp + bodyPosX;
    int totalY = Y + Ydisp + bodyPosY;
    
    ans.x = Xspine + totalX - int(totalX*cosG*cosA + totalY*sinB*sinG*cosA + Z*cosB*sinG*cosA - totalY*cosB*sinA + Z*sinB*sinA) + bodyPosX;
    ans.y = Yspine + totalY - int(totalX*cosG*sinA + totalY*sinB*sinG*sinA + Z*cosB*sinG*sinA + totalY*cosB*cosA - Z*sinB*cosA) + bodyPosY;
    ans.z = Z - int(-totalX*sinG + totalY*sinB*cosG + Z*cosB*cosG);

    return ans;
}

/* Simple 3dof leg solver. X,Y,Z are the length from the Coxa rotate to the endpoint. */
ik_sol_t legIK(int X, int Y, int Z){
    ik_sol_t ans;

    // first, make this a 2DOF problem... by solving coxa
    ans.coxa = radToServo(atan2(X,Y));
    long trueX = sqrt(sq((long)X)+sq((long)Y)) - L_COXA;
    long im = sqrt(sq((long)trueX)+sq((long)Z));    // length of imaginary leg

    // get femur angle above horizon...
    float q1 = -atan2(Z,trueX);
    long d1 = sq(L_FEMUR)-sq(L_TIBIA)+sq(im);
    long d2 = 2*L_FEMUR*im;
    float q2 = acos((float)d1/(float)d2);
    ans.femur = radToServo(q1+q2);

    // and tibia angle from femur...
    d1 = sq(L_FEMUR)-sq(im)+sq(L_TIBIA);
    d2 = 2*L_TIBIA*L_FEMUR;
    ans.tibia = radToServo(acos((float)d1/(float)d2)-1.57);

    return ans;
}

void doIK(){
    int servo;
    ik_req_t req, gait;
    ik_sol_t sol;

    gaitSetup();
    spineIK();

    // right front leg
    gait = gaitGen(RIGHT_FRONT);
    req = bodyIK(endpoints[RIGHT_FRONT].x+gait.x, endpoints[RIGHT_FRONT].y+gait.y, endpoints[RIGHT_FRONT].z+gait.z, X_COXA_MID, X_COXA, Y_COXA, gait.r, -twist);
    sol = legIK(endpoints[RIGHT_FRONT].x+req.x+gait.x,endpoints[RIGHT_FRONT].y+req.y+gait.y,endpoints[RIGHT_FRONT].z+req.z+gait.z);
    servo = 512 + sol.coxa - radToServo(twist);
    if(servo < maxs[RF_COXA-1] && servo > mins[RF_COXA-1])
        bioloid.setNextPose(RF_COXA, servo);
    else{
        Serial.print("RF_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 542 + sol.femur;
    if(servo < maxs[RF_FEMUR-1] && servo > mins[RF_FEMUR-1])
        bioloid.setNextPose(RF_FEMUR, servo);
    else{
        Serial.print("RF_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 472 + sol.tibia;
    if(servo < maxs[RF_TIBIA-1] && servo > mins[RF_TIBIA-1])
        bioloid.setNextPose(RF_TIBIA, servo);
    else{
        Serial.print("RF_TIBIA FAIL: ");
        Serial.println(servo);
    }

    // right rear leg
    gait = gaitGen(RIGHT_REAR);
    req = bodyIK(endpoints[RIGHT_REAR].x+gait.x,endpoints[RIGHT_REAR].y+gait.y, endpoints[RIGHT_REAR].z+gait.z, -X_COXA_MID, -X_COXA, Y_COXA, gait.r, twist);
    sol = legIK(-endpoints[RIGHT_REAR].x-req.x-gait.x,endpoints[RIGHT_REAR].y+req.y+gait.y,endpoints[RIGHT_REAR].z+req.z+gait.z);
    servo = 512 - sol.coxa + radToServo(twist);
    if(servo < maxs[RR_COXA-1] && servo > mins[RR_COXA-1])
        bioloid.setNextPose(RR_COXA, servo);
    else{
        Serial.print("RR_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 542 + sol.femur;
    if(servo < maxs[RR_FEMUR-1] && servo > mins[RR_FEMUR-1])
        bioloid.setNextPose(RR_FEMUR, servo);
    else{
        Serial.print("RR_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 472 + sol.tibia;
    if(servo < maxs[RR_TIBIA-1] && servo > mins[RR_TIBIA-1])
        bioloid.setNextPose(RR_TIBIA, servo);
    else{
        Serial.print("RR_TIBIA FAIL: ");
        Serial.println(servo);
    }

    // left front leg
    gait = gaitGen(LEFT_FRONT);
    req = bodyIK(endpoints[LEFT_FRONT].x+gait.x,endpoints[LEFT_FRONT].y+gait.y, endpoints[LEFT_FRONT].z+gait.z, X_COXA_MID, X_COXA, -Y_COXA, gait.r, -twist);
    sol = legIK(endpoints[LEFT_FRONT].x+req.x+gait.x,-endpoints[LEFT_FRONT].y-req.y-gait.y,endpoints[LEFT_FRONT].z+req.z+gait.z);
    servo = 512 - sol.coxa - radToServo(twist);
    if(servo < maxs[LF_COXA-1] && servo > mins[LF_COXA-1])
        bioloid.setNextPose(LF_COXA, servo);
    else{
        Serial.print("LF_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 482 - sol.femur;
    if(servo < maxs[LF_FEMUR-1] && servo > mins[LF_FEMUR-1])
        bioloid.setNextPose(LF_FEMUR, servo);
    else{
        Serial.print("LF_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 552 - sol.tibia;
    if(servo < maxs[LF_TIBIA-1] && servo > mins[LF_TIBIA-1])
        bioloid.setNextPose(LF_TIBIA, servo);
    else{
        Serial.print("LF_TIBIA FAIL: ");
        Serial.println(servo);
    }

    // left rear leg
    gait = gaitGen(LEFT_REAR);
    req = bodyIK(endpoints[LEFT_REAR].x+gait.x,endpoints[LEFT_REAR].y+gait.y, endpoints[LEFT_REAR].z+gait.z, -X_COXA_MID, -X_COXA, -Y_COXA, gait.r, twist);
    sol = legIK(-endpoints[LEFT_REAR].x-req.x-gait.x,-endpoints[LEFT_REAR].y-req.y-gait.y,endpoints[LEFT_REAR].z+req.z+gait.z);
    servo = 512 + sol.coxa + radToServo(twist);
    if(servo < maxs[LR_COXA-1] && servo > mins[LR_COXA-1])
        bioloid.setNextPose(LR_COXA, servo);
    else{
        Serial.print("LR_COXA FAIL: ");
        Serial.println(servo);
    }
    servo = 482 - sol.femur;
    if(servo < maxs[LR_FEMUR-1] && servo > mins[LR_FEMUR-1])
        bioloid.setNextPose(LR_FEMUR, servo);
    else{
        Serial.print("LR_FEMUR FAIL: ");
        Serial.println(servo);
    }
    servo = 552 - sol.tibia;
    if(servo < maxs[LR_TIBIA-1] && servo > mins[LR_TIBIA-1])
        bioloid.setNextPose(LR_TIBIA, servo);
    else{
        Serial.print("LR_TIBIA FAIL: ");
        Serial.println(servo);
    }

    step = (step+1)%stepsInCycle;
}
