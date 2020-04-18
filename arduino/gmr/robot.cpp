#include <WProgram.h>
#include "robot.h"

BioloidController bioloid = BioloidController(115200);

/* 
 * Motor/Encoder Stuff
 */ 
int LeftSpeed;
int RightSpeed;
int SLeftSpeed;
int SRightSpeed;

/* we compare to OCR1A/B for R/L motor speeds */
#define lPWM		OCR0B
#define rPWM		OCR0A

#define SetBit(sfr, bit) ((sfr) |= (1 << (bit)))
#define ClearBit(sfr, bit) ((sfr) &= ~(1 << (bit)))
/* set direction (input to H-bridge) and wave output mode */
#define LFwd()		( ClearBit(PORTC, PC3),   SetBit(TCCR0A, COM0B1), ClearBit(TCCR0A, COM0B0) )
#define LRev()		(   SetBit(PORTC, PC3),   SetBit(TCCR0A, COM0B1),   SetBit(TCCR0A, COM0B0) )
#define LStop()		( ClearBit(PORTC, PC3), ClearBit(TCCR0A, COM0B1), ClearBit(TCCR0A, COM0B0) )
#define RFwd()		( ClearBit(PORTC, PC4),   SetBit(TCCR0A, COM0A1), ClearBit(TCCR0A, COM0A0) )
#define RRev()		(   SetBit(PORTC, PC4),   SetBit(TCCR0A, COM0A1),   SetBit(TCCR0A, COM0A0) )
#define RStop()		( ClearBit(PORTC, PC4), ClearBit(TCCR0A, COM0A1), ClearBit(TCCR0A, COM0A0) )

/* sets up microprocessor for PWM control of motors */
void motorInit(void){
  /* set up ports */
  pinMode(20,OUTPUT);	// Right Dir - PC4
  pinMode(4,OUTPUT);	// Left PWM  - PB4
  pinMode(3,OUTPUT);	// Right PWM - PB3
  pinMode(19,OUTPUT);	// Left Dir  - PC3
  
  /* OCR0A/B are the values that the timer is compared to; a match will
     cause the output to change; small values mean the motor runs for a
     short period (slower); larger values are longer times (faster)*/ 
  digitalWrite(19,LOW);
  digitalWrite(20,LOW);
  analogWrite(3,0);
  analogWrite(4,0);
  lPWM = rPWM = 0;	// (value is irrelevant since outputs are disconnected)
}

/** pwm values can range from -255 (full-speed reverse)
    to 255 (full-speed forward), with 0 indicating a stop */
void motorLeft(int pwm){
  if (pwm == 0){
    LStop();
  }else{
    if (pwm >= 0){
      LFwd();
    }else{
      LRev();
      pwm = -pwm;
    }
    if (pwm > 255)
      pwm = 255;
    OCR0B = pwm;  // LEFT PWM = PB4 - OC0B = D4
  }
  LeftSpeed = pwm;
}

/** pwm values can range from -255 (full-speed reverse)
    to 255 (full-speed forward), with 0 indicating a stop */
void motorRight(int pwm){
  if (pwm == 0){
    RStop();
  }else{
    if (pwm >= 0){
      RFwd();
    }else{
      RRev();
      pwm = -pwm;
    }
    if (pwm > 255)
      pwm = 255;
    OCR0A = pwm;  // RIGHT PWM = PB3 = OC0A = D3
  }
  RightSpeed = pwm;
}

/** Stops both motors */
void motorStop(void){
  SLeftSpeed = LeftSpeed;
  SRightSpeed = RightSpeed;
  LStop();
  RStop();
}

/** Restarts both motors */
void motorResume(void){
  motorRight(SRightSpeed);
  motorLeft(SLeftSpeed);
}

/**************************CLOSED-LOOP FUNCTIONS**************************/
volatile int rCount= 0;
volatile int rLastB= 0;
volatile int lCount= 0;
volatile int lLastB= 0;

/** This enables the closed loop control, still need to call sei() */
void closedInit(){
  // all IO are inputs
  // PC[2] - LEFT ENCODER B
  // PC[5] - RIGHT ENCODER B
  // PC[7] - RIGHT ENCODER A (INT)
  // PD[7] - LEFT ENCODER A (INT)
  
  //rLastB= digitalGetData(PIN_RENC_B);
  //lLastB= digitalGetData(PIN_LENC_B);

  PCICR |= (1 << PCIE3) | (1 << PCIE2) ;  // enable PC interrupt 2 & 3
  //PCMSK2 = 0x80; 		// enable interrupt on PC7 (RENC_A)
  PCMSK3 = 0x80; 		// enable interrupt on PD7 (LENC_A)
}

/** NOTE ALL OF THIS IS HACKED UP, COUNTING IS ALWAYS POSTIVE */

/** right side closed loop counter */
ISR(PCINT2_vect){
  rCount++;
}

/** left side closed loop counter */
ISR(PCINT3_vect){
  lCount++;
}

/** causes robot to move x centimeters (positive = forward) */
void moveX(int x){
  int goal = COUNTS_CM(x);
  clearCounters;

  if(x > 0){
    motorLeft(REG_SPEED);
    motorRight(REG_SPEED);
  }else{
    goal = -goal;
    motorLeft(-REG_SPEED);
    motorRight(-REG_SPEED);
  }

  while(lCount < goal); 	
  motorStop();	
}

/** turns angle degrees (positive = right) */
void turnX(int angle){
  int goal= COUNTS_DEGREE(angle);  
  clearCounters;      
  if(angle < 0){
    goal = -goal;
    motorLeft(-130);
    motorRight(130);
  }else{
    motorLeft(130);
    motorRight(-130);
  }
  while(lCount < goal); 
  motorStop();
}

/*
 * SENSORY
 */
SharpIR headIR = SharpIR(GP2D12,PIN_HEAD_IR);
SharpIR frontLeftIR = SharpIR(GP2D12,PIN_FRONT_L);
SharpIR frontRightIR = SharpIR(GP2D12,PIN_FRONT_R);
SharpIR RightIR = SharpIR(GP2D120,PIN_RIGHT_IR);
Tpa81 thermo = Tpa81(0);
Srf08 sonar = Srf08(0);

/*
 * UVTRON interrupt based driver
 */
int uv_val;

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

ISR(PCINT0_vect){
  // assume this is only interrupt on port A
  if(uv_val < 1000)
    uv_val++;
}



