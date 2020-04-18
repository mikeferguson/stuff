/* GMR Navigation Module */
#include "nav.h"
#include <WProgram.h>

#define CNRG
//#define TRINITY

char BREG;                  // Behavior Status Register
int odometer;               // the metric odometer

/*
 * Reactive Planning
 */
    
/** Bang-bang iterative follow, should be called at least a minimum of 10-15Hz */
int followRight(unsigned char dist){
  SetPosition(HEAD_SERVO,360);  // position head
  if(getIR() < 17){
    // motor WAY to the left
    motorLeft(REG_SPEED-60);
    motorRight(REG_SPEED+30);
  }else if(getIR() < 23){
    // motor way to the left
    motorLeft(REG_SPEED-30);
    motorRight(REG_SPEED+30);
  }else if(getIR() < 30){
    // motor to the left...	
    motorLeft(REG_SPEED-15);
    motorRight(REG_SPEED+15);
  }else if(getIR() < NO_WALL){
    // motor to the right...
    motorLeft(REG_SPEED+15);
    motorRight(REG_SPEED-15);   
  }else{
    // motor straight
    motorLeft(REG_SPEED+3);
    motorRight(REG_SPEED);
  }
  FOLLOWING_RIGHT;
  return 0;
}
int followLeft(unsigned char dist){
  SetPosition(HEAD_SERVO,664);  // position head
  if(getIR() < 17){
    // motor WAY to the right
    motorLeft(REG_SPEED+60);
    motorRight(REG_SPEED-30);
  }else if(getIR() < 23){
    // motor way to the right
    motorLeft(REG_SPEED+30);
    motorRight(REG_SPEED-30);
  }else if(getIR() < 30){
    // motor to the right...	
    motorLeft(REG_SPEED+15);
    motorRight(REG_SPEED-15);
  }else if(getIR() < NO_WALL){
    // motor to the left...
    motorLeft(REG_SPEED-15);
    motorRight(REG_SPEED+15);   
  }else{
    // motor straight
    motorLeft(REG_SPEED+3);
    motorRight(REG_SPEED);
  }
  FOLLOWING_LEFT;
  return 0;
}

/** = one run of Arbitration loop, 0 if cruise, >0 otherwise */
int arbitrate(){
  int steps;

  if(odometer <= 0){
  // priority 4 = entering room
    if(BREG&ENTERING_ROOM){
      motorStop();
      // done, now extinguish candle!
      return 4;
    }    
  // priority 3 = wall in front
    if(BREG&CHECK_FORWARD){
      SetPosition(HEAD_SERVO,512);    
      delay(250);
      if(getIR() < 23){   // TODO: STOP DIST = 30?        
        motorStop();   
        delay(250);
        Serial.println("Forward Wall");
        // move to wall, avoiding side wall
        moveX(getIR() - 16);                
        // call topological planner
        return 3;            
      }      
    }  
  // priority 2 = turn at opening, if within neighborhood
    if(BREG&CHECK_RIGHT && RightIR.getSmoothData() > NO_R_WALL){          
      motorStop();
      Serial.println("No Right Wall");             
      delay(100);
      return 2;
    }
    if(BREG&CHECK_LEFT){
      SetPosition(HEAD_SERVO, 819);
      delay(250);
      if(getIR() > NO_L_WALL){
        motorStop();
        Serial.println("No Left Wall");             
        delay(100);
        return 2;
      }
    }
  }
    
  // ... set params
  if(BREG&FOLLOW_LEFT)
    followLeft(30);   
  else
    followRight(30);
  delay(20);
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

  // if room, means we saw a fire now fight it
  if(navMap[curnode].flags == RM_VIS){                
    fightFire(28);
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
    // if it is the last room, go in regardless of fire detection
    //if(nnode != RM3){         
      uvtronClearData(PIN_UV);
      // size of room is encoded into dist (100+size of room)
      delay(900); //600*(navMap[nnode].dist - 100));
      if(uvtronGetData(PIN_UV) == 0){
        // no fire detected, don't need to enter room
        navMap[nnode].flags = RM_VIS;
        // lnode, lheading unaltered, so just replan!
        return plan();
      }
    //}
    Serial.println("Fire!!!!");
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
    if(nnode == RM3)
      odometer = 50;
    else
      odometer = 35;
  }else if(navMap[nnode].dirs[nheading] == mNULL){
    // if we can, forward wall detection is least problematic!
    BREG = CHECK_FORWARD;
    odometer = navMap[lnode].paths[lheading] - NBHD_SIZE;
  }else{
    // oh well, this might work!
    if(navMap[nnode].dirs[shiftHeadingR(nheading)] != mNULL)
      BREG |= CHECK_RIGHT;
    if(navMap[nnode].dirs[shiftHeadingL(nheading)] != mNULL)
      BREG |= CHECK_LEFT;
    odometer = navMap[lnode].paths[lheading] - NBHD_SIZE/2;
  }
  // return control (to arbitrate)    
  return (int) curnode;
}

/* 
 * Fire Fighting Routines
 */

/** pan for the fire, returns >0 if we find it */
int panForFire(){
  /*int i = 716;
  SetPosition(HEAD_SERVO,i);
  delay(350);
  // look for a fire
  while(getPhoto() > PHOTO_THRESH){
    i = i - 10;
    SetPosition(HEAD_SERVO,i);
    delay(30);
    if(i < 204){    
      return 0;
    } 
  }   
  // turn towards, i = angle
  i = 150-(i*300L)/1024;
  turnX(i+7);*/
  return 1;
}   

/** Locate fire and kill it. 
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
}*/




/** pan, looking for fire */
int firePan(){
  int ambient, i;
  int average = 21;
  unsigned char cur_reading[8];
  // take readings as we slowly pan
  while(1){
    ambient = thermo.getData(cur_reading); 
    average = (average + ambient)/2;
    // compare and do if neccesary            
    for(i=0;i<8;i++){
      if(cur_reading[i] > 35){//(average + 6)){ // was 8
        motorStop();
        return 1;
      }            
    }
  }
}

/** Locate fire and kill it. */
void fightFire(int dist){
  unsigned char sample;   
  int i;
  Serial.println("Fighting Fire");

  // reset head
  SetPosition(HEAD_SERVO,512);
  // turn a little to the left 
  turnX(-75);
  // slowly pan
  clearCounters;  // added 4/5
  motorLeft(78);//65 changed 6/13 for RG);
  motorRight(-78);
  // and find the fire
  if(firePan() == 0)
    return;

  PING_SONAR;    	
  delay(50);
  // now don't run into wall...
  sample = GET_SONAR;
  while(sample > dist){
    if(frontLeftIR.getSmoothData() < 13){ // was 12
      // arc right
      motorLeft(160);
      motorRight(140);            
    }else{
      if(frontRightIR.getSmoothData() < 13){
        // arc left
        motorLeft(130);
        motorRight(160);
      }else{
        // want to move forward, curving a little (cause we overshoot)
        motorLeft(140);		// was 160
        motorRight(150);	// was 140
      }
    }
    PING_SONAR;
    delay(50);
    sample = GET_SONAR;
  }

  // We are there
  motorStop();
  // close enough now, put it out
  FAN_ON;
  // pan fan back and forth
  //ax12SetSpeed(1,300);
  SetPosition(1,704);
  delay(300);   
  sample = 0;
  while(uvtronGetData(PIN_UV)>0){
    sample++;
    uvtronClearData(PIN_UV);
    // retarget time?
    if(sample > 3){
      // kill fan, start again..
      FAN_OFF;
      SetPosition(HEAD_SERVO,512);
      moveX(-10);
      if(dist > 15){
        fightFire(dist - 1);
      }else{
	fightFire(dist);
      }
    }
    for(i=704;i>320;i--){
      SetPosition(1,i);
      delay(5);
    }
    for(i=320;i<704;i++){
      SetPosition(1,i);
      delay(5);
    }
  }    
  FAN_OFF;
  Serial.println("DONE....");

  // return home
  delay(3500);	
  //goHome();
}

