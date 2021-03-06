/******************************************************************************
 * Inverse Kinematics for 4/5/6 DOF Biped
 *  Copyright 2009-2010 Michael E. Ferguson 
 *
 * Auto-Generated by NUKE!
 *   http://arbotix.googlecode.com
 * 
 * 4DOF Version: 
 *    FRONT                                      SIDE
 *   V-----V    4th Level = rotate feet             V
 *   |     |                                        |
 *   =     =    3rd Level = knee (f/b)              0
 *   |     |                                       /
 *   =     =    2nd Level = forward/backward      0   
 *  _0_   _0_   1st Level = Ankle roll           _|_
 * 
 * 
 *         TOP 
 *    |==|     |==|  ^         Major Parameters:
 *    |  |  +  |  |  |         X-offset of each foot from COG
 *    |__|     |__|  X         Angle from parallel to X (+ = clockwise from above)
 *                             Z-height of outside edge of swing foot
 *                   Y-->      Y-shift of COG over planted foot
 * 
 ******************************************************************************
 * 5/6 DOF Version:
 *    FRONT                                      SIDE
 *   V-----V    6th Level = rotate feet            V
 *   O     O    5th Level = body shift           0--=
 *   |     |    4th Level = forward/backward    /
 *   |     |                                   /
 *   =     =    3rd Level = knee (f/b)         0
 *   |     |                                   \
 *   |     |                                    \
 *   0     0    2nd Level = forward/backward     0--=
 *              1st Level = ankle shift
 * 
 *         TOP 
 *    |==|     |==|  ^         Major Parameters:
 *    |  |  +  |  |  |         X-offset of each foot from COG
 *    |__|     |__|  X         Angle from parallel to X (+ = clockwise from above)
 *                             Z-height of outside edge of swing foot
 *                   Y-->      Y-shift of COG over planted foot
 *                             Rotation in the 6DOF model is by forward IK 
 *                                  (it's a parameter of of the gait)
 * 
 * Note: COR = Center-of-Rotation (not quite the center of gravity) 
 */

#define MOVING          ((Xspeed > 2) || (Xspeed < -2) || (Yspeed > 2) || (Yspeed < -2) || (Rspeed > 0.05) || (Rspeed < -0.05))

//#define IK_4_DOF
#define IK_5_DOF
//#define IK_6_DOF

/* 
 * Body Lengths
 */
#define L_ANKLE             31          // distance from ankle roll to ground
#define L_1                 0           // vertical displacement from ankle roll to ankle shift servo
#define L_TIBIA             75          // if this isn't descriptive enough, take up knitting
#define L_FEMUR             75          // same here...
#define L_2                 0           // vertical displacement from hip shift to hip roll servo

#define F_WIDTH             32          // half width of the foot (ankle to edge)
#define F_LEN_FR            51          // horizontal displacement from ankle shift to front of foot
#define F_LEN_RR            68          // horizontal displacement from ankle shift to rear of foot

#define BODY_WIDTH          32          // actually body width /2 (like X_COXA)

/* Servo Definitions */
#define RIGHT_ANKLE_ROLL    1
#define RIGHT_ANKLE         3
#define RIGHT_KNEE          5
#define RIGHT_HIP           7
#define RIGHT_HIP_ROLL      9     
#define RIGHT_ROTATE        11

#define LEFT_ANKLE_ROLL     2
#define LEFT_ANKLE          4
#define LEFT_KNEE           6
#define LEFT_HIP            8
#define LEFT_HIP_ROLL       10
#define LEFT_ROTATE         12

/*
 * Each foot has several parameters 
 */
typedef struct{
  int x;            // X displacement from COR (+=forward)
  int y;            // Y displacement from COR
  int z;            // Z displacement from COR to "center" of foot
  float r;          // rotation from parallel to the X-axis (for 4/6 DOF)
} ik_req_t;

#define LEFT_LEG       0
#define RIGHT_LEG      1

/* Servo ouptut values (output of 4/5/6 dof leg solver). */
typedef struct{
  int rotate;       // top level, rotate feet for turning
  int hip_roll;     // 
  int hip;
  int knee;
  int ankle;
  int ankle_roll;
} ik_sol_t;

/* 
 * IK Engine
 */
extern BioloidController bioloid;
extern ik_req_t endpoints[2];
extern float bodyRotX;                 // body roll (rad) (for 5/6DOF)
extern float bodyRotY;                 // body pitch (rad) 
extern float bodyRotZ;                 // body rotation (rad) (for 4/6DOF)
extern int bodyPosX;                   // body offset (mm)
extern int bodyPosY;                   // body offset (mm)
extern int Xspeed;                     // forward speed (mm/s)
extern int Yspeed;                     // sideward speed (mm/s)
extern float Rspeed;                   // rotation speed (rad/s)

void doIK();
void setupIK();

/* Reaver2 Arms
 * 11 - 205 = straight up and down  (512 = straight out)
 * 12 - 819 = straight up and down  (512 = straight out)
 * 13 - >512 = move arm outward
 * 14 - <512 = move arm outward
 * 15 - <512 = open elbow
 * 16 - >512 = open elbow
 */
 
#define RIGHT_SHOULDER  11
#define LEFT_SHOULDER   12
#define RIGHT_ELBOW     15
#define LEFT_ELBOW      16

#define RIGHT_SHOULDER_CENTER 205-30
#define LEFT_SHOULDER_CENTER  819+30

#include "gaits.h"                     // Pick up our gait definitions

