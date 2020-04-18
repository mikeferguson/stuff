/******************************************************************************
 * Issy Goes Fire Fighting! - code for CNRG 2009 
 * This file defines IK and sensory for Issy...
 */
 
#ifndef ISSY
#define ISSY

#include <ax12.h>
#include <BioloidController.h>
#include <SharpIR.h>

/******************************************************************************
 * Inverse Kinematics for 4/6 legged bots using 3DOF legs
 *
 *                       ISSY_FF:                
 * FRONT VIEW       ^    L_COXA = 52mm         ==0         0==
 *     /\___/\      |    L_FEMUR = 47mm       |  0==[___]==0  |
 *    /       \     Z    L_TIBIA = 125mm      |               |  
 *
 * TOP VIEW
 *    \       /     ^    Y_COXA = 58mm
 *     \_____/      |    X_COXA = 58mm
 *  ___|     |___   X
 *     |_____|
 *     /     \      Y->
 *    /       \
 *****************************************************************************/

//#define TASK_FF
//#define TASK_WALKER_CHALLENGE
#define TASK_BE_AWESOME

#define LEG_COUNT   4
#ifdef TASK_FF
  #define TOP_SPEED 75
  #define STD_HEIGHT 90
#else
  #define TOP_SPEED   90  // mm/s
  #define STD_HEIGHT  90  // 105 // was 90, sim shows 90 to be optimal, but not if we want high stepping
#endif

/* Body 
 * We assume 4 legs are on the corners of a box defined by X_COXA x Y_COXA 
 * Middle legs for a hexapod can be different Y, but should be halfway in X 
 */
#define X_COXA      29  // MM between front and back legs /2
#define Y_COXA      29  // MM between front/back legs /2

/* Legs */
#define L_COXA      52  // MM distance from coxa servo to femur servo 
#define L_FEMUR     47  // MM distance from femur servo to tibia servo 
#define L_TIBIA     125 // MM distance from tibia servo to foot 
extern int LIFT_HEIGHT;

/* Servo IDs */
#define RF_TIBIA   1
#define RF_FEMUR   3
#define RF_COXA    5

#define LF_TIBIA   2
#define LF_FEMUR   4
#define LF_COXA    6

#define RR_TIBIA   7
#define RR_FEMUR   9
#define RR_COXA    11

#define LR_TIBIA   8
#define LR_FEMUR   10
#define LR_COXA    12

/* A leg position request (output of body calcs, input to simple 3dof solver). */
typedef struct{
    int x;
    int y;
    int z;
    float r;
} ik_req_t;

/* Servo ouptut values (output of 3dof leg solver). */
typedef struct{
    int coxa;
    int femur;
    int tibia;
} ik_sol_t;

/* Actual positions, and indices of array. */
extern ik_req_t endpoints[LEG_COUNT];
#define RIGHT_FRONT    0
#define RIGHT_MIDDLE   4
#define RIGHT_REAR     1
#define LEFT_FRONT     2
#define LEFT_MIDDLE    5
#define LEFT_REAR      3

extern BioloidController bioloid;

/* Parameters for manipulating body position */
extern float bodyRotX;    // body roll
extern float bodyRotY;    // body pitch
extern float bodyRotZ;    // body rotation
extern int bodyPosX;
extern int bodyPosY;

/* Gait Engine */
extern int gaitStep;
extern int gaitLegNo[];
extern ik_req_t gaits[];    // gait position
//int NrLiftedPos;	  
extern int divFactor;	  
extern int stepsInGait;

/* Parameters for gait manipulation */
extern int travelX;
extern int travelY;
extern float travelRotZ;
extern int tranTime;

/* convert radians to a dynamixel servo offset */
int radToServo(float rads);
/* select a gait pattern to use */
void gaitSelect(int GaitType);

/* find the translation of the endpoint (x,y,z) given our gait parameters */
extern ik_req_t (*gaitGen)(int leg);
ik_req_t DefaultGaitGen(int leg);
ik_req_t QuadRippleTurning(int leg);
#define QUAD_RIPPLE_TURNING    1
ik_req_t QuadRipple(int leg);
#define QUAD_RIPPLE            2
#define FAST_TURN              3

/* find the translation of the coxa point (x,y) in 3-space, given our rotations */
ik_req_t bodyIK(int X, int Y, int Z, int Xdisp, int Ydisp, float Zrot);
/* given our leg offset (x,y,z) from the coxa point, calculate servo values */
ik_sol_t legIK(int X, int Y, int Z);
/* ties all of the above together */
void doIK();
/* setup the starting positions of the legs. */
void setupIK();

/* 
 * Some higher level movement
 */

/** causes robot to move x CM (positive = forward) */
void moveX(int x);
/** turns angle degees (positive = right) */
void turnX(int angle);

/* 
 * Tack on our sensory!
 */

/* Analog Port Usage */
#define PIN_SONAR       0
#define PIN_IR          1
#define PIN_PHOTO       2
#define PIN_VOLTAGE     7
/* Digital Port Usage */
#define PIN_LED         0
#define PIN_START       2
#define HEAD_SERVO      14
#define PIN_FAN         28

extern SharpIR headIR;
#define getIR headIR.getSmoothData

int getSonar();
int getPhoto();

/* Schematic for OP599a sensors 
 * 
 *          5V
 *          |
 *          R = 47K
 *  analog__| 
 *          |
 *          C (flat side)
 *          + op599a
 *          E 
 *          |
 *         Gnd
 */

#endif

