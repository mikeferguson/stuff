/* Issy Navigation Module */
#include "nav.h"
#include <WProgram.h>

#define CNRG
//#define TRINITY

char BREG;                  // Behavior Status Register
int odometer;               // the metric odometer
int flipdometer;

/*
 * Reactive Planning
 */
  
/** Bang-bang iterative follow, should be called at least a minimum of 10-15Hz */
int followRight(unsigned char dist){
  SetPosition(HEAD_SERVO,360);  // position head
  int x = getIR();
  if(x < dist){ 
    if(x < ((dist * 3) / 5))
      travelRotZ = 0.11;    // motor way to the left
    else
      travelRotZ = 0.07;    // motor to the left...    
  }else{
    if(x > ((dist*5)/3))
      travelRotZ = -0.10;
    else 
      travelRotZ = -0.05;       // motor to the right...
  }
  return 0;
}
int followLeft(unsigned char dist){
  SetPosition(HEAD_SERVO,664);  // position head
  int x = getIR();
  if(x < dist){ 
    if(x < ((dist * 3) / 5))
      travelRotZ = -0.11;   // motor way to the right
    else
      travelRotZ = -0.07;   // motor to the right...
  }else{
    if(x > ((dist * 5) / 3))
      travelRotZ = 0.10;  // 0.10
    else
      travelRotZ = 0.05;        // motor to the left...        
  }
  return 0;
}

/** = one run of Arbitration loop, 0 if cruise, >0 otherwise */
int arbitrate(){
  int steps;

  /*if(BREG&DO_FLIP){
    if(flipdometer <=0 ){
      BREG &= ~DO_FLIP;
      BREG |= FOLLOW_LEFT;
    }
  }*/
  
  if(odometer <= 0){
  // priority 4 = entering room
    if(BREG&ENTERING_ROOM){
      // this call will not return if a fire is found
      isFire();
      // no fire, call planner to exit room
      return 4;
    }    
  // priority 3 = wall in front
    if(BREG&CHECK_FORWARD){
      if(BREG&TILT_HEAD)
        SetPosition(HEAD_SERVO,612);
      else
        SetPosition(HEAD_SERVO,512);    
      delay(250);
      if(getIR() < 23){
        Serial.println("Forward Wall");
        // move to wall, avoiding side wall
        //moveX(GET_HEAD_IR - 16);                
        // call topological planner
        return 3;            
      }      
    }  
  // priority 2 = turn at opening, if within neighborhood
    if(BREG&CHECK_RIGHT){            
      SetPosition(HEAD_SERVO,204); // was 284  // was 204, 11/15/09  
      delay(250);    
      if(getIR() > 46){            
        Serial.println("No Right Wall");             
        // call metric planner - to plot course of action
             
        // call topological planner
        return 2;
      }
    }
    if(BREG&CHECK_LEFT){
      SetPosition(HEAD_SERVO,819); // was 739, 11/21   // was 819, 11/15/09   
      delay(500); //250); 11/21/09  
      if(getIR() > 46){
        Serial.println("No Left Wall");               
        // call metric planner - to plot course of action
               
        // call topological planner
        return 2;
      }
    }
    if(BREG&FOLLOW_LEFT)
      travelRotZ = -.04;
    else if(BREG&TILT_HEAD)
      travelRotZ = .05;
    else
      travelRotZ = .03;
  }else{                      // This IS NEW -- 11/21/09
    // ... set params 
    if(BREG&FOLLOW_LEFT)
      followLeft(30);   
    else
      followRight(30);
  }
      
  if(odometer < 15){
    moveX(5);
    odometer += -6;
    flipdometer += -6;
  }else{
    moveX(15);
    odometer += -20;
    flipdometer += -20;
  }
  return 0;
}

/*
 * High Level Planning
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
signed char lnode;	  // this is that last node we left

#ifdef CNRG
  #include "cnrg.h"
#endif
#ifdef TRINITY
  #include "trinity.h"
#endif

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
    signed char nheading=0;        // next heading
    signed char tnode;            // temp node
    signed char theading;        // temp heading
    signed char priority;        // used in search pattern
    
    // this should be our new current node
    signed char curnode = navMap[lnode].dirs[lheading];
    
    // update flags to show visited
    if(navMap[curnode].flags > 0){
        navMap[curnode].flags= -navMap[curnode].flags;
    }    

    // if in room, we need to exit
    if(navMap[curnode].flags == RM_VIS){                
        // no fire, already facing exit       
        // setup parameters for room exit
        odometer = 28; //ROOM_DIST - 10;
        BREG = CHECK_FORWARD|EXITING_ROOM;
#ifdef CNRG
        if(curnode == RM1)
          BREG |= FOLLOW_LEFT;
#endif
        // no planning required
        lnode = curnode;
        lheading= shiftHeadingR(lheading);
        lheading= shiftHeadingR(lheading);
        // skip the rest of the stuff below, return to arbitrate
        //  we will follow wall out of room, until we hit a forward wall
        return curnode;
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

    // logic to turn from lheading to nheading! 
    theading= nheading-lheading;
    switch(theading){
        case 1:
        case -3:
            turnX(-90);        // LEFT
            break;
        case 2:
        case-2:
            turnX(180);
            break;
        case -1:
        case 3:
            turnX(90);
            break;
        default:    
            break;
    }
    // update globals
    lheading= nheading;
    lnode= curnode;
    BREG = 0;
    if(navMap[nnode].flags == IS_ROOM){
        // entering room
#ifdef CNRG
        if(nnode != RM1)
#endif 
        BREG = FOLLOW_LEFT;
        BREG |= ENTERING_ROOM;
        //if((nnode == RM2)){
        //  odometer = 25;
        //}else{
          odometer = 40;
        //}
        /*if((nnode == RM3) || (nnode == RM4))
            odometer = 40;  // was 60
        else
            odometer = 35;  // was 30*/
    }else if(navMap[nnode].dirs[nheading] == mNULL){
        // if we can, forward wall detection is least problematic!
        BREG = CHECK_FORWARD;
        odometer = navMap[lnode].paths[lheading] - NBHD_SIZE;
    }else{
        // oh well, this might work!
        if(navMap[nnode].dirs[shiftHeadingR(nheading)] != mNULL)
            BREG |= CHECK_RIGHT;
        if(navMap[nnode].dirs[shiftHeadingL(nheading)] != mNULL){
            BREG |= CHECK_LEFT | DO_FLIP;
            flipdometer = 30;
        }
        odometer = navMap[lnode].paths[lheading] - NBHD_SIZE/2;
    }
    if((nnode == n4) && (curnode == n5))
      BREG |= TILT_HEAD;
    if((curnode == n4) && (nnode == n5))
      BREG |= FOLLOW_LEFT;
    // return control (to arbitrate)    
    return (int) curnode;
}

/* 
 * Fire Fighting Routines
 */

/** pan for the fire, returns >0 if we find it */
int panForFire(){
  int i,v;
  bodyRotY = 0.03;
  if((BREG&FOLLOW_LEFT) == 0){  // RM1 @ CNRG
    Serial.println("Full Left Pan");
    // pan head a little to the right, for the region from door to wall
    for(i=-250;i<=0;i+=5){
      SetPosition(HEAD_SERVO, 512+i);
      delay(25);
      if(getPhoto() < PHOTO_THRESH){
        // turn towards fire
        turnX((-(i*3)/10)/2);
        bodyRotY = 0.0;
        return 1;
      }
    }
    // pan body looking for fire
    for(i=0;i<14;i++){
      turnX(-13);
      if(getPhoto() < PHOTO_THRESH){
        bodyRotY = 0.0;
        return 1;
      }  
    }
  }else{  // regular room
    turnX(-26);
    // pan head to the left a bit, to see region near door
    for(i=+250;i>=0;i+=-5){
      SetPosition(HEAD_SERVO, 512+i);
      delay(25);
      if(getPhoto() < PHOTO_THRESH){
        // turn towards fire
        turnX((-(i*3)/10)+10);
        bodyRotY = 0.0;
        return 1;
      }
    }
    // pan body looking for fire
    for(i=0;i<6;i++){   //14x 13
      turnX(26);
      for(v=+100;v>-100;v+=-5){
        SetPosition(HEAD_SERVO, 512+v);
        delay(25);
        if(getPhoto() < PHOTO_THRESH){
          // turn towards fire
          turnX((-(v*3)/10) +10);
          bodyRotY = 0.0;
          return 1;
        }
      }
    }
    turnX(59);  // was 52 11/22/09 1:31PM
  }
  bodyRotY = 0.0;      
  return 0; 
}

/** Locate fire and kill it. */
void fightFire(int dist){
  int i,x;  
  Serial.println("Fighting Fire");
  // pointed at the fire, move at it, but avoid wall
  while(getSonar() > dist){
    travelRotZ = 0;  // TODO: home in on fire?
    moveX(5);
  }
  // close enough now, put it out
  digitalWrite(PIN_FAN, HIGH);
  // pan fan back and forth
  for(x=0;x<3;x++){    
    for(i=704;i>320;i--){
      SetPosition(HEAD_SERVO,i);
      delay(5);
    }
    for(i=320;i<704;i++){
      SetPosition(HEAD_SERVO,i);
      delay(5);
    }    
  }
  digitalWrite(PIN_FAN, LOW);
  // check to make sure we are ok	
  moveX(-10);
  if(panForFire() > 0) fightFire(dist);   
  if(panForFire() > 0) fightFire(dist);     
  Serial.println("Done");
  while(1);
}

/** checks for fire, this will not return if there is a fire */
void isFire(){
  // check for fire
  if(panForFire() > 0){
    fightFire(15);
  }/*else{
    turnX(180);
  }*/
  // no fire - return to arbitrate
  Serial.println("No Fire");	
}

