#include <avr/pgmspace.h>

/* default star-shaped stand */
PROGMEM prog_uint16_t stand[ ]  =    {0x0C,512,512,482,542,662,362,512,512,542,482,362,662}; 

/* pick up and move front left and right rear legs forwards, horizontals are centered on range */
PROGMEM prog_uint16_t walk0[ ] =     {0x0C,552,532,482,492,612,412,532,552,492,482,412,612,
                                          50,50,50,50,30,45,50,50,50,50,45,30};
PROGMEM prog_uint16_t walkend[ ] =   {0x0C,552,472,482,542,562,462,472,552,542,482,412,612,
                                          30,30,30,30,15,15,30,30,30,30,15,15};

/* pick up and move right front and left rear legs forwards, horizontals are centered on range */
PROGMEM prog_uint16_t walk1[ ] =     {0x0C,492,472,532,542,612,412,472,492,542,532,412,612,
                                          50,50,50,50,45,30,50,50,50,50,30,45};
                                      
                  // RANDOM REMOVED CODE                    
                                      
    // reading at 45 and 30
    /*SetPosition(14,358);
    delay(500);
    int b = getIR();
    SetPosition(14,307);
    delay(75);
    int a = getIR();
    float c = sqrt( sq(a)+sq(b)-(a*b*1.93) );      // cos(15)=0.966    
    Serial.println(b);
    Serial.println(a);
    Serial.println(c);
    float alpha = acos(-(sq(a)-sq(b)-sq(c))/(2*b*c));
    Serial.println(alpha);
    float beta = 2.356 - alpha;
    Serial.println(beta); 
    //SetPosition();
    while(1);*/
    //panForFire();
    //delay(5000);
              
              /** pan for the fire, returns >0 if we find it */
/* THIS IS THE PANNING WITH DROP */
/*int panRegion(){
  int i; 
  // squat
  endpoints[RIGHT_FRONT].z = 75;
  endpoints[RIGHT_REAR].z = 75;
  endpoints[LEFT_FRONT].z = 75;
  endpoints[LEFT_REAR].z = 75;
  moveX(0);
  // pan a 60 deg region
  SetPosition(HEAD_SERVO,412);
  delay(150);
  for(i=-100;i<101;i+=10){
    SetPosition(HEAD_SERVO, 512+i);
    delay(100);
    if(getPhoto() < PHOTO_THRESH){
      endpoints[RIGHT_FRONT].z = STD_HEIGHT;
      endpoints[RIGHT_REAR].z = STD_HEIGHT;
      endpoints[LEFT_FRONT].z = STD_HEIGHT;
      endpoints[LEFT_REAR].z = STD_HEIGHT;
      i = (i*3)/10; //150+(i*300L)/1024;
      //if((BREG & FOLLOW_LEFT) == 0)
        turnX(i); // +7
      //else
      //  turnX(i+7);
      return 1;
    }
  }  
  endpoints[RIGHT_FRONT].z = STD_HEIGHT;
  endpoints[RIGHT_REAR].z = STD_HEIGHT;
  endpoints[LEFT_FRONT].z = STD_HEIGHT;
  endpoints[LEFT_REAR].z = STD_HEIGHT;
  return 0;
}
int panForFire(){
  int i;
  if((BREG&FOLLOW_LEFT) == 0){
    Serial.println("Full Left Pan");
    turnX(30);
    for(i=0;i<4;i++){
      if(panRegion() > 0){
        return 1;
      }
      turnX(-45);
    }
    turnX(-30);
  }else{
    turnX(-30);
    for(i=0;i<4;i++){
      if(panRegion() > 0){
        return 1;
      }
      turnX(45);
    }
    turnX(30);
  }  
  return 0;
}*/

/*int panForFire(){
  int i = 819;
  int av = 800;
  int r = 800;
  int lStart = 0;
  //bodyRotY = -0.07;
  if((BREG&FOLLOW_LEFT)==0){
    turnX(-50);
    Serial.println("Full Left Pan");
  }else{
    turnX(50);
  }
  endpoints[RIGHT_FRONT].z = 75;
  endpoints[RIGHT_REAR].z = 75;
  endpoints[LEFT_FRONT].z = 75;
  endpoints[LEFT_REAR].z = 75;
  moveX(0);
  SetPosition(HEAD_SERVO,i);
  delay(500);
  // look for a fire
  while(1){ //r > PHOTO_THRESH){
    r = getPhoto();
    Serial.println(r);
    // compare to rolling average?
    av = ((7 * av) + (3 * r))/10;
    if(lStart == 0){
      if(r<PHOTO_THRESH) //      (r < (av/3)) && (lStart == 0))
        lStart = i;
    }else if (r > PHOTO_THRESH){
      i = (i + lStart)/2;
      break; 
    }
    // move head
    i = i - 10;
    SetPosition(HEAD_SERVO,i);
    delay(100);
    if(i < 204){
      if(r < PHOTO_THRESH){
        break;
      }
      //bodyRotY = 0;
      endpoints[RIGHT_FRONT].z = STD_HEIGHT;
      endpoints[RIGHT_REAR].z = STD_HEIGHT;
      endpoints[LEFT_FRONT].z = STD_HEIGHT;
      endpoints[LEFT_REAR].z = STD_HEIGHT;   
      if((BREG&FOLLOW_LEFT)==0)
        turnX(-130);
      else
        turnX(130);
      return 0;
    } 
  }   
  // turn towards, i = angle
  i = 150-(i*300L)/1024;
  //bodyRotY = 0; 
  endpoints[RIGHT_FRONT].z = STD_HEIGHT;
  endpoints[RIGHT_REAR].z = STD_HEIGHT;
  endpoints[LEFT_FRONT].z = STD_HEIGHT;
  endpoints[LEFT_REAR].z = STD_HEIGHT;
  turnX(i+7);
  return 1;
}   */                         



/*int followRight(unsigned char dist){
  SetPosition(HEAD_SERVO,360);  // position head
  if(getIR() < dist){ 
    if(getIR() < ((dist * 3) / 5))
      travelRotZ = 0.15;    // motor way to the left
    else
      travelRotZ = 0.05;    // motor to the left...    
  }else
     travelRotZ = -0.05;       // motor to the right...
  return 0;
}*/  


/*int followRight(unsigned char dist){
  SetPosition(HEAD_SERVO,360);  // position head
  int x = getIR();
  //Serial.println(x);
  if(x < (dist-10)){ // = 23 @ 30
    travelRotZ = (dist-x)/30.0;
  }else if(x < (dist-2)){ 
    // motor to the left...
    travelRotZ = (dist-x)/45.0; //0.05;
  }else if(x < (dist+2)){
    travelRotZ = 0;
  }else if(x < (dist+10)){
    travelRotZ = (dist-x)/45.0;
  }else if((x < NO_WALL) && (odometer > 10)){
    // motor to the right...
    travelRotZ = -0.15; 
  }else{
    // motor straight
    travelRotZ = 0;
  }
  return 0;
}*/

/*int followLeft(unsigned char dist){
  SetPosition(HEAD_SERVO,664);  // position head
  int x = getIR();
  if(x < 23){
    travelRotZ = (x-30.0)/30.0;
  }else if(x < 28){ 
    // motor to the left...
    travelRotZ = (x-30.0)/45.0; //0.05;
  }else if(x < 32){
    travelRotZ = 0;
  }else if(x < 40){
    travelRotZ = (x-30.0)/45.0;
  }else if((x < NO_WALL) && (odometer > 10)){
    // motor to the right...
    travelRotZ = 0.15; 
  }else{
    // motor straight
    travelRotZ = 0;
  }
  return 0;
}*/
