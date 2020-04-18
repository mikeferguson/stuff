/* UV TRON TEST
 * Connect UVTron to A0
 * LED on D0
 */
 
/*
 * UVTRON interrupt based driver
 */
volatile int uv_val;

/** This enables the uvtron, still need to call sei() */
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
 
void setup(){
  pinMode(0,OUTPUT);
  uvtronInit(0);
}

void loop(){
  delay(3000);
  if(uvtronGetData(0) > 3){
    digitalWrite(0,HIGH); 
  }else{
    digitalWrite(0,LOW);
  }
  uvtronClearData(0);
}

ISR(PCINT0_vect){
  // assume this is only interrupt on port A
  if(uv_val < 1000)
    uv_val++;
}
