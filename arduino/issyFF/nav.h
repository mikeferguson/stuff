/* Issy Navigation Module */

#ifndef ISSY_NAV
#define ISSY_NAV

#include "issy.h"

// Behavior Status Register - default to 0x00
//  bit 0:      hi = check for left wall
//  bit 1:      hi = check for forward wall
//  bit 2:      hi = check for right wall
//  bit 4:      hi = flip wal following after 20cm
//  bit 5:      hi = follow left wall
//  bit 6:      hi = ENTERING_ROOM
//  bit 7:      hi = EXITING_ROOM
extern char BREG;
extern int odometer;        // the metric odometer
extern int flipdometer;
    
#define CHECK_LEFT          0x01
#define CHECK_FORWARD       0x02
#define CHECK_RIGHT         0x04
#define TILT_HEAD           0x08
#define DO_FLIP             0x10
#define FOLLOW_LEFT         0x20
#define ENTERING_ROOM       0x40
#define EXITING_ROOM        0x80

/*
 * lotsa constants for all things mapalicious
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

/*
 * node definition
 */
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

/*
 * Reactive Behaviors
 */

/** Bang-bang iterative follow, should be called at least a minimum of 10-15Hz */
int followRight(unsigned char dist);
int followLeft(unsigned char dist);

/** = one run of Arbitration loop, 0 if cruise, >0 otherwise */
int arbitrate();

/*
 * Mapping and Navigation
 */

void PrintHeading(signed char heading, signed char node);
void mapInit();
int plan();

/* 
 * Fire Fighting Routines
 */
#define PHOTO_THRESH        128
#define PHOTO_NEAR          15

/** pan for the fire, returns >0 if we find it */
int panForFire();
/** the original fight fire. */
void fightFire(int dist);
/** checks for fire, this will not return if there is a fire */
void isFire();

#define NO_WALL      55

#endif
