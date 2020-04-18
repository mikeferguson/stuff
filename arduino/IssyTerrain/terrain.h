/* 
 * Terrain adapting Gaits
 */ 
 
#include <WProgram.h>
#define TERRAIN_LIFT_HEIGHT  50
#define STABILITY_K          15

int terrainSensors[4]; // = {200,200,200,200};
int terrainThresholds[4] = {700,700,700,700}; //{25,25,25,75};
int legInTransit;
int inAir;

/* Which leg is the cross leg? */
int cross(int leg){
  if((leg == RIGHT_FRONT) || (leg == LEFT_FRONT))
    return leg+1;
  else
    return leg-1;
}

/* Is sensor on Ground? */ 
int getTerrainSensor(int leg){
  //analogRead(7-leg);
  //analogRead(7-leg);
  //analogRead(7-leg);
  //terrainSensors[leg] = ((terrainSensors[leg] * 5)/8) + ((analogRead(7-leg)*3)/8);
  //if(terrainSensors[leg] > terrainThresholds[leg])  // leg on ground
  //if(analogRead(7-leg) > terrainThresholds[leg])
  if(PINA&(0x80>>leg))
    terrainSensors[leg]++;
  else
    terrainSensors[leg] = 0;
  if(terrainSensors[leg] > 1)
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
  if(inAir > 0){          // Sort of a hack, need to back up
    //step = step -1;
  }else if((step%4)==0){  // Body Shift
    // fix heights, if needed
    
    
   
    // move all feet back
    int leg;
    for(leg=0;leg<LEG_COUNT;leg++){
      gaits[leg].x = gaits[leg].x - (Xspeed*cycleTime)/4;
      gaits[leg].y = gaits[leg].y - (Yspeed*cycleTime)/4;
      gaits[leg].r = gaits[leg].r - (Rspeed*cycleTime)/4;
      terrainSensors[leg] = 0;
    }
    // adjust COG for next foot
    if((step == 0)||(step == 8)){  // front leg
      bodyPosX = 0; //-(Xspeed*cycleTime)/4;
    }else{
      bodyPosX = (Xspeed*cycleTime)/4;
    }
    // adjust COG for next foot
    int S1, S2;   // Support legs
    int sign = 1; // sign for body position Y
    if(step == 0){        // balance for RF
      S1 = LEFT_FRONT;
      S2 = RIGHT_REAR;
      sign = -1;
    }else if(step == 4){  // balance for LR
      S1 = RIGHT_REAR;
      S2 = LEFT_FRONT;
    }else if(step == 8){  // balance for LF
      S1 = RIGHT_FRONT;
      S2 = LEFT_REAR;
    }else{                // balance for RR
      S1 = LEFT_REAR;
      S2 = RIGHT_FRONT;
      sign = -1;
    }
    // X distance COG to S1
    int X1 = X_COXA + abs(endpoints[S1].x + gaits[S1].x);
    // X distance COG to S2
    int X2 = X_COXA + abs(endpoints[S2].x + gaits[S2].x);
    // Y distance COG to S1
    int Y1 = Y_COXA + abs(endpoints[S1].y + gaits[S1].y);
    // Y width S1 to S2
    int Y = Y1 + Y_COXA + abs(endpoints[S2].y + gaits[S1].y);
    // Stability Margin
    int SM = (( (long)X1*(long)Y )/( (long)X1+(long)X2 )) - Y1;
    if(SM < 0)
      bodyPosY = sign * (STABILITY_K - SM);
    else
      bodyPosY = sign * STABILITY_K;
    Serial.print("GaitSetup:");
    Serial.print(X1);
    Serial.print(",");
    Serial.print(X2);
    Serial.print(",");
    Serial.print(Y1);
    Serial.print(",");
    Serial.print(Y);
    Serial.print(",");
    Serial.print(SM);
    Serial.print(",");
    Serial.println(bodyPosY);
  }else if((step%4)==1){  // lift leg
    delay(100);  // wait for body to settle
  }
  tranTime = 200;
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
      if(((getTerrainSensor(leg) > 0) && getTerrainSensor(cross(leg))) || (gaits[leg].z > 0)){
        inAir = 0;
      }else{ 
        // leg down
        legInTransit = leg;
        inAir = 1;
        gaits[leg].z = gaits[leg].z + 7;    // ADJUSTMENT VALUE
        step += -1;
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
  gaitLegNo[LEFT_REAR] = 5;    // 1,5,9,13 are leg up
  gaitLegNo[LEFT_FRONT] = 9;   // 2,6,10,14 are leg forward
  gaitLegNo[RIGHT_REAR] = 13;  // 3,7,11,15 is place leg to ground
  //pushSteps = 4;
  stepsInCycle = 16;
  inAir = 0;
  legInTransit = -1;
  step = 0;  
}

