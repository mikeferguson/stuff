/* behavior code for SMALdog */
#include <ax12.h>
#include <BioloidController.h>
#include <WProgram.h>
#include <math.h>
#include "behavior.h"
#include "nuke.h"

//#include "cnrg.h"
#include "trinity.h"

char BREG;                  // Behavior Status Register
int odometer;               // the metric odometer

/********************************************************************
 * Reactive Behaviors
 */
  
/** Bang-bang iterative follow, should be called at least a minimum of 10-15Hz */
int followRight(unsigned char dist){
  HEAD_FORWARD;
  int x = getRightSideIR();
  if(x < dist){ 
    digitalWrite(PIN_LED,HIGH);
    if(x < ((dist * 3) / 5))  // travelRotZ is positive in left direction
      travelRotZ = 0.04; //0.08;    // motor way to the left (was 0.08)
    else
      travelRotZ = 0.02;    // motor to the left...   (was 0.05) 
  }else{
    digitalWrite(PIN_LED,LOW);
    //bodyRotX = 0.1;
    if(x > ((dist*5)/3))
      travelRotZ = -0.08;
    else //if( x < NO_WALL) 
      travelRotZ = -0.05;   // motor to the right...
    //else
    //  travelRotZ = 0;
  }
  return 0;
}
int followLeft(unsigned char dist){
  HEAD_FORWARD;
  int x = getLeftSideIR();
  if(x < dist){ 
    digitalWrite(PIN_LED,LOW);
    //bodyRotX = 0.1;
    if(x < ((dist * 3) / 5))
      travelRotZ = -0.08;   // motor way to the right
    else
      travelRotZ = -0.05;   // motor to the right...
  }else{
    digitalWrite(PIN_LED,HIGH);
    if(x > ((dist * 5) / 3))
      travelRotZ = 0.04;  
    else //if( x < NO_WALL)
      travelRotZ = 0.02;    // motor to the left...   
    //else
    //  travelRotZ = 0;     
  }
  return 0;
}

/** = one run of Arbitration loop, 0 if cruise, >0 otherwise */
int arbitrate(){
  int steps;
  travelRotZ = 0;
  bodyRotX = 0;
  
  if(odometer <= 0){
  // priority 4 = entering room
    if(BREG&ENTERING_ROOM){
      // done, now extinguish candle!
      return 4;
    }    
  // priority 3 = wall in front
    if(BREG&CHECK_FORWARD){
      if(getSonar() < 20){
        HEAD_45_DEG_R;
        delay(250);
        if(getLeftIR() < 22){  // confirm it
          Serial.println("Forward Wall");
          // move to wall, avoiding side wall
          moveX(getLeftIR()- 16);                
          // call topological planner
          return 3;   
        }         
      }      
    }  
  // priority 2 = turn at opening, if within neighborhood
    if(BREG&CHECK_RIGHT){   
      //if(getRightSideIR() > 30){     
        HEAD_45_DEG_R; 
        delay(250);    
        if(getRightIR() > 46){            
          Serial.println("No Right Wall");             
          // call metric planner - to plot course of action
               
          // call topological planner
          return 2;
        }
      //}
    }
    if(BREG&CHECK_LEFT){
      //if(getLeftSideIR() > 30){  
        HEAD_45_DEG_L;   
        delay(250);
        if(getLeftIR() > 46){
          Serial.println("No Left Wall");               
          // call metric planner - to plot course of action
                 
          // call topological planner
          return 2;
        }
      //}
    } 
  }
  // ... set params and go 
  if(BREG&FOLLOW_LEFT){
    if((odometer > 15) || (BREG&CHECK_LEFT == 0))
      followLeft(30);   
  }else{
    if((odometer > 15) || (BREG&CHECK_RIGHT == 0))
      followRight(30);  
  }
  
  travelX = 60; //80;
  gaitSelect(AMBLE); //RIPPLE_DISCO);
  for(steps=0;steps<stepsInCycle;steps++){
      doIK();
      bioloid.interpolateSetup(tranTime);
      while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(6);
      }
  }
  odometer += -8;
  return 0;
}

/********************************************************************
 * Some higher level movements
 */

static int m_cnt = 0;
/** causes robot to move x CM (positive = forward) */
void moveX(int x){
  int steps; 
  gaitSelect(RIPPLE_DISCO); // or RIPPLE_SHIFT or RIPPLE_DISCO
  steps = stepsInCycle;
  x = x*10;   // cm->mm
  if(x < 0){
    if(-x < TOP_SPEED)
      travelX = x;
    else{
      travelX = -TOP_SPEED;
      steps = (int)((float)steps * ((float)-x/TOP_SPEED));
    }
  }else{
    if(x < TOP_SPEED)
      travelX = x;
    else{
      travelX = TOP_SPEED;
      steps = (int)((float)steps * ((float)x/TOP_SPEED));
    }
    while(steps > 0){
      doIK();
      bioloid.interpolateSetup(tranTime);
      while(bioloid.interpolating > 0){
        bioloid.interpolateStep();
        delay(3);
      }
      steps--;    
      //Serial.println(m_cnt++);
    }
  }
}

/** turns angle degees (positive = right) */
void turnX(int angle){    
  HEAD_FORWARD;
  gaitSelect(AMBLE);
  travelX = 0;
  if(angle > 0){
    travelRotZ = -0.174; // 10 deg //-0.207; // 12 deg
    angle = (int) (((float)angle/14.0)*(float)stepsInCycle); // 12.0
  }else{
    travelRotZ = 0.174;    
    angle = (int) (((float)-angle/14.0)*(float)stepsInCycle);  // 13.0
  }
  while(angle > 0){
    doIK();
    bioloid.interpolateSetup(tranTime);
    while(bioloid.interpolating > 0){
      bioloid.interpolateStep();
      delay(3);
    }
    angle--;
  }
  travelRotZ = 0;
}

/********************************************************************
 * Mapping and Navigation
 */
void PrintHeading(signed char heading, signed char node){
  switch(heading){
    case 0: Serial.print("N,"); break;
    case 1: Serial.print("W,"); break;
    case 2: Serial.print("S,"); break;
    case 3: Serial.print("E,"); break;
  }
  switch(node){
    case 0: Serial.println("nStart"); break;
    case 1: Serial.println("n1"); break;
    case 2: Serial.println("n2"); break;
    case 3: Serial.println("n3"); break;
    case 4: Serial.println("RM1"); break;
    case 5: Serial.println("n4"); break;
    case 6: Serial.println("n5"); break;
    case 7: Serial.println("RM2"); break;
    case 8: Serial.println("n6"); break;
    case 9: Serial.println("RM3"); break;
    case 10: Serial.println("n7"); break;
    case 11: Serial.println("RM4"); break;
  }
}

/** = new heading, after shifting current heading left */
signed char shiftHeadingL(signed char heading){
  if(heading<EAST)
    return heading + 1;
  else
    return NORTH;
}

/** = new heading, after shifting current heading right */
signed char shiftHeadingR(signed char heading){
  if(heading>NORTH)
    return heading - 1;
  else
    return EAST;
}

// map and state variables
mapNode_t navMap[tNodes];

signed char lheading;	  // our heading when we left node (N,W,S,E)
signed char lnode;	      // this is that last node we left

void mapInit(){
  int x;
  // general setup
  for(x= 0; x<tNodes; x++){
    navMap[x].dirs[NORTH] = mNULL;
    navMap[x].dirs[WEST] = mNULL;
    navMap[x].dirs[SOUTH] = mNULL;
    navMap[x].dirs[EAST] = mNULL;
    navMap[x].paths[NORTH] = 0;
    navMap[x].paths[WEST] = 0;
    navMap[x].paths[SOUTH] = 0;
    navMap[x].paths[EAST] = 0;
    navMap[x].flags = IS_HALL;
    navMap[x].dist = 0;
  }
  // specific setup
  mapSpecInit();
}

/*
 * The real action. Robot is parked when we hit this. Triggered by a loss of 
 *  a wall or appearance of a forward wall. Takes lastnode, last heading, 
 *  current heading and computes new node location. Updates map info as 
 *  needed (i.e. add a visited flag). Then plans course. Moves robot to new
 *  heading, sets BREG, and returns control (... to low level behaviors). 
 *  Returns new node heading.
 */
int plan(){
    signed char nnode=0;        // next node
    signed char nheading=0;     // next heading
    signed char tnode;          // temp node
    signed char theading;       // temp heading
    signed char priority;       // used in search pattern
    
    // this should be our new current node
    signed char curnode = navMap[lnode].dirs[lheading];
    
    // update flags to show visited
    if(navMap[curnode].flags > 0){
        navMap[curnode].flags= -navMap[curnode].flags;
    }    

    // in room, means we saw a fire, now fight it!
    if(navMap[curnode].flags == RM_VIS){
        fightFire(1); //28);
        // if we get here, we failed (TODO: replan)
    }

    // we give priority to going right, then continuing in the same direction.
    //  but loop requires we shiftRightx2 then left to get node directly ahead
    theading= shiftHeadingR(shiftHeadingR(lheading));
    priority = -3;
    do{
        // find our next node and heading
        theading= shiftHeadingL(theading);
        tnode= navMap[curnode].dirs[theading];
        // check if it is a node
        if((tnode != mNULL)){
            // is it of higher priority?
            if((navMap[tnode].flags > priority)){
                // yep, update
                nnode= tnode;
                nheading= theading;
                priority = navMap[nnode].flags;
            }
        }
        // run 4 times (one full circle)
    }while((theading!=shiftHeadingR(shiftHeadingR(lheading))));    
    PrintHeading(nheading,nnode);   // print results of planning

    // need to check for fire?
    if(navMap[nnode].flags == IS_ROOM){
        // if it is the last room, go in regardless of fire detection   TODO: change our map?
        if(nnode != RM1){         
            uvtronClearData(PIN_UV);
            // size of room is encoded into dist (100+size of room)
            delay(2000); // was 500
            if(uvtronGetData(PIN_UV) == 0){
                // no fire detected, don't need to enter room
                navMap[nnode].flags = RM_VIS;
                // lnode, lheading unaltered, so just replan!
                return plan();
            }
            Serial.println("Fire!!!!");
        }else
          Serial.println("Hope for Fire!!!!");
    }

    // logic to turn from lheading to nheading! 
    theading= nheading-lheading;
    switch(theading){
        case 1:
        case -3:
            turnX(-90);        // LEFT
            break;
        case 2:
        case-2:
            turnX(160);
            break;
        case -1:
        case 3:
            turnX(78); //90);
            break;
        default:    
            break;
    }
    // update globals
    lheading= nheading;
    lnode= curnode;
    BREG = CHECK_LEFT;
    if(navMap[nnode].flags == IS_ROOM){
        // entering room
        BREG = FOLLOW_LEFT|ENTERING_ROOM;
        odometer = ROOM_DIST;
    }else if(navMap[nnode].dirs[nheading] == mNULL){
        // if we can, forward wall detection is least problematic!
        BREG = CHECK_FORWARD;
        odometer = navMap[lnode].paths[lheading] - NBHD_SIZE;
    }else{
        // oh well, this might work!
        if(navMap[nnode].dirs[shiftHeadingR(nheading)] != mNULL){
          BREG = CHECK_RIGHT;
        }else{  
          BREG = CHECK_LEFT;
        }
        odometer = navMap[lnode].paths[lheading] - NBHD_SIZE/2;
    }
    if((curnode == n4) && (nnode == n5))
      BREG |= FOLLOW_LEFT;
    if((curnode == n7) && (nnode == n6))
      BREG |= FOLLOW_LEFT;

    // return control (to arbitrate)    
    return (int) curnode;
}

/********************************************************************
 * Sensory 
 */

SharpIR frontLeftIR = SharpIR(GP2D12,PIN_FRONT_L);
SharpIR frontRightIR = SharpIR(GP2D12,PIN_FRONT_R);
SharpIR RightIR = SharpIR(GP2D12,PIN_RIGHT_IR);
SharpIR LeftIR = SharpIR(GP2D12,PIN_LEFT_IR);

int getSonar(){
    int reading = analogRead(PIN_SONAR)/2;
    reading = reading + analogRead(PIN_SONAR)/2;
    reading = reading + analogRead(PIN_SONAR)/2;
    reading = reading + analogRead(PIN_SONAR)/2;
    reading = reading/4;
    return (reading*10)/4;
}

/********************************************************************
 * Fire Fighting Routines
 */

int getPhoto(){
    int reading = analogRead(PIN_PHOTO);
    reading = reading + analogRead(PIN_PHOTO);
    reading = reading + analogRead(PIN_PHOTO);
    reading = reading + analogRead(PIN_PHOTO);
    reading = reading/4;
    return reading;
}

int uv_val;
// This enables the uvtron, still need to call sei()
void uvtronInit(char channel){
  // channel must be on PORTA
  uv_val = 0;
  PCICR |= (1 << PCIE0);      // enable PC interrupt on port A
  PCMSK0 = (1 << channel);    // enable interrupt on channel
}
int uvtronGetData(char channel){
  return uv_val;
}
void uvtronClearData(char channel){
  uv_val = 0;
}
ISR(PCINT0_vect){
  // assume this is only interrupt on port A
  if(uv_val < 1000)
    uv_val++;
}

// TODO: recode/verify below this line

/* locate the fire, at exit, we are pointed towards it */
int findFire(){
  // TODO: scan, find max peak, turn towards, repeat?
  turnX(-70);   // turn left a bit
  while(getPhoto() > PHOTO_THRESH){
    turnX(5);
    delay(125);
  }
  return 0; 
}

/** Locate fire and kill it. */
void fightFire(int search){
  int i,x;  
  if(search > 0){
    // point at the fire
    Serial.println("Finding Fire...");
    findFire();
  }
  // pointed at the fire, move at it, but avoid wall
  Serial.println("Fighting Fire");
  while(getSonar() > 15){
    travelRotZ = 0;  // TODO: home in on fire?
    moveX(7);
  }
  // close enough now, put it out
  digitalWrite(PIN_FAN, HIGH);
  // pan fan back and forth
  for(x=0;x<3;x++){    
    for(i=H_45_DEG_L;i>H_45_DEG_R;i--){
      SetPosition(HEAD_SERVO,i);
      delay(5);
    }
    for(i=H_45_DEG_R;i<H_45_DEG_L;i++){
      SetPosition(HEAD_SERVO,i);
      delay(5);
    }       
  }
  digitalWrite(PIN_FAN, LOW);
  // check to make sure we are ok	
  moveX(-15);
  fightFire(1);   
  fightFire(1);     
  Serial.println("Done");
  while(1);
}




