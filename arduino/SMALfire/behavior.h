/********************************************************************
 * behavior code for SMALdog 
 */

#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <SharpIR.h>
 
/********************************************************************
 * Physical and Electrical 
 */
#define HEAD_SERVO      15      
#define HEAD_45_DEG_R   SetPosition(15,358)
#define H_45_DEG_R      358
#define HEAD_15_DEG_R   SetPosition(15,461)
#define H_15_DEG_R      461
#define HEAD_FORWARD    SetPosition(15,512)
#define HEAD_15_DEG_L   SetPosition(15,563)
#define H_15_DEG_L      563
#define HEAD_45_DEG_L   SetPosition(15,665)
#define H_45_DEG_L      665
// analog usage
#define PIN_FRONT_L     0
#define PIN_FRONT_R     1
#define PIN_SONAR       2
#define PIN_PHOTO       3
#define PIN_UV          4
#define PIN_LEFT_IR     6
#define PIN_RIGHT_IR    7
// digital usage
#define PIN_LED         0
#define PIN_FAN         1
#define PIN_START       7

/********************************************************************
 * Reactive Behaviors
 */

// Behavior Status Register - default to 0x00
//  bit 0:      hi = check for left wall
//  bit 1:      hi = check for forward wall
//  bit 2:      hi = check for right wall
//  bit 5:      hi = prefer follow left wall
//  bit 6:      hi = ENTERING_ROOM
//  bit 7:      hi = EXITING_ROOM
extern char BREG;
extern int odometer;                // the metric odometer
    
#define CHECK_LEFT          0x01
#define CHECK_FORWARD       0x02
#define CHECK_RIGHT         0x04
#define FOLLOW_LEFT         0x20
#define ENTERING_ROOM       0x40
#define EXITING_ROOM        0x80

// Bang-bang iterative follow, should be called at least a minimum of 10-15Hz
int followRight(unsigned char dist);
int followLeft(unsigned char dist);

#define FOLLOWING_LEFT  digitalWrite(PIN_WALL_LED, HIGH)
#define FOLLOWING_RIGHT digitalWrite(PIN_WALL_LED, LOW)

// = one run of Arbitration loop, 0 if cruise, >0 otherwise
int arbitrate();
// causes robot to move x CM (positive = forward)
void moveX(int x);
// turns angle degees (positive = right)
void turnX(int angle);

#define TOP_SPEED           75.0

/********************************************************************
 * Mapping and Navigation
 */

// array indexes for directions
#define NORTH               0
#define WEST                1
#define SOUTH               2
#define EAST                3

// node description flags
#define RM_VIS              -2
#define VISITED             -1
#define CLEAR               0
#define IS_HALL             1
#define IS_ROOM             2

// node mapping to array index, nasty looking, but hides details
#define tNodes              13      // total number of nodes
#define nStart              0
#define n1                  1
#define n2                  2
#define n3                  3
#define RM1                 4
#define n4                  5
#define n5                  6
#define RM2                 7
#define n6                  8
#define RM3                 9
#define n7                  10
#define RM4                 11
#define n8                  12      // added for CNRG (as well as tNodes++)
#define mNULL               -1      // null index in map array 

// limited metrics
#define NBHD_SIZE           15      // radius of node nieghborhoods, CM (was 23 on GMR)
#define ROOM_DIST           40      // how far into a room do we go, CM (was 20 on GMR)
#define NO_WALL             55

// node definition
typedef struct{
    signed char dirs[4];            // direction index links
    unsigned char paths[4];         // distance along path (cm)
    signed char flags;              // flags for this node
    signed char dist;               // distance to goal (only used in return)
} mapNode_t; 

// map and state variables
extern mapNode_t navMap[];
extern signed char lheading;        // our heading when we left node (N,W,S,E)
extern signed char lnode;           // this is that last node we left

// function
void PrintHeading(signed char heading, signed char node);
void mapInit();
void mapSpecInit();
int plan();

/********************************************************************
 * Sensory 
 */
extern SharpIR frontLeftIR;
extern SharpIR frontRightIR;
extern SharpIR LeftIR;
extern SharpIR RightIR;

#define getLeftIR frontLeftIR.getSmoothData
#define getRightIR frontRightIR.getSmoothData
#define getLeftSideIR LeftIR.getSmoothData
#define getRightSideIR RightIR.getSmoothData

int getSonar();

/* *******************************************************************
 * Fire Fighting Routines
 */

#define PHOTO_THRESH        128
#define PHOTO_NEAR          15
int getPhoto();

void uvtronInit(char channel);
int uvtronGetData(char channel);
void uvtronClearData(char channel);

#define FAN_ON          digitalWrite(PIN_FAN,HIGH)
#define FAN_OFF         digitalWrite(PIN_FAN,LOW)

// pan for the fire, align body to it
int panForFire();
// the original fight fire. 
void fightFire(int dist);

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
