/* 
 * Terrain adapting Amble
 */ 
 
#include <WProgram.h>
#define TERRAIN_LIFT_HEIGHT  50
#define STABILITY_K          15

int terrainSensors[4] = {200,200,200,200};
int terrainThresholds[4] = {25,25,25,75};
int legInTransit;
int inAir;

/* Is sensor on Ground? */ 
int getTerrainSensor(int leg){
  analogRead(7-leg);
  analogRead(7-leg);
  analogRead(7-leg);
  terrainSensors[leg] = ((terrainSensors[leg] * 5)/8) + ((analogRead(7-leg)*3)/8);
  if(terrainSensors[leg] > terrainThresholds[leg])  // leg on ground
    return 1;
  else
    return 0;
}
 
/* Output Ground Sensor Status */ 
void doGroundUpdate(){
  // update foot sensor outputs
  digitalWrite(7,getTerrainSensor(RIGHT_FRONT)); // A7 = RF
  digitalWrite(6,getTerrainSensor(RIGHT_REAR));  // A6 = RR
  digitalWrite(5,getTerrainSensor(LEFT_FRONT));  // A5 = LF
  digitalWrite(4,getTerrainSensor(LEFT_REAR));   // A4 = LR
}

/* Calculations at the beginning of a cycle. */
void TerrainGaitSetup(){
  bodyPosX = Xspeed/20;
  tranTime = 65;
  if(inAir > 0){
    inAir = 2;
    step = step -1;
    if(step < 0) step = 7;
  }
  if((step%4)==0){  // Body Shift
    // move all feet back
    int leg;
    for(leg=0;leg<LEG_COUNT;leg++){
      gaits[leg].x = gaits[leg].x - (Xspeed*cycleTime)/2;
      gaits[leg].y = gaits[leg].y - (Yspeed*cycleTime)/2;
      gaits[leg].r = gaits[leg].r - (Rspeed*cycleTime)/2;
      terrainSensors[leg] = 0;
    }
  }
}

/*  */
ik_req_t TerrainGaitGen(int leg){
  if( MOVING ){
    // are we moving?
    if(step == gaitLegNo[leg]){
      // leg up
      gaits[leg].z = -TERRAIN_LIFT_HEIGHT;
    }else if(step == gaitLegNo[leg] + 1){
      // leg forward
      gaits[leg].x = (Xspeed*cycleTime)/2;
      gaits[leg].y = (Yspeed*cycleTime)/2;
      gaits[leg].r = (Rspeed*cycleTime)/2;
    }else if(step == gaitLegNo[leg] + 2){
      // is leg on ground? If so, done
      if((getTerrainSensor(leg) > 0) || (gaits[leg].z > 0)){
        inAir--;
      }else{ 
        // leg down
        legInTransit = leg;
        inAir = 2;
        gaits[leg].z = gaits[leg].z + 7;    // ADJUSTMENT VALUE
        //step += -1;
      }
      tranTime = 32;
    } 
  }else{
    // ?
  }   
  Serial.print("GaitReq:");
  Serial.print(gaits[leg].x);
  Serial.print(",");
  Serial.print(gaits[leg].y);
  Serial.print(",");
  Serial.print(gaits[leg].z);
  Serial.print(",");
  Serial.println(gaits[leg].r);
  return gaits[leg];
}

/* Select the Terrain Adapting gait */
void gaitSelectTerrain(){
  tranTime = 98;    // 65 for body shift, 65 for leg up, 65 for forward motion of leg, 32*X for placement of leg (X typically <10)
  cycleTime = 2;    // approximately 2 seconds for full cycle
  bodyPosX = 0;
  currentGait = TERRAIN_RIPPLE;
  gaitGen = &TerrainGaitGen;
  gaitSetup = &TerrainGaitSetup;
  gaitLegNo[RIGHT_FRONT] = 1;  // 0,4,8,12 are body shift
  gaitLegNo[LEFT_REAR] = 1;    // 1,5,9,13 are leg up
  gaitLegNo[LEFT_FRONT] = 5;   // 2,6,10,14 are leg forward
  gaitLegNo[RIGHT_REAR] = 5;   // 3,7,11,15 is place leg to ground
  //pushSteps = 4;
  stepsInCycle = 8;
  inAir = 0;
  legInTransit = -1;
  step = 0;  
}

// How does it work?
// 16 Steps 

