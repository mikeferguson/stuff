// Definitions for GMR

#ifndef GMR
#define GMR

#include <ax12.h>
#include <BioloidController.h>
#include <SharpIR.h>
#include <Tpa81.h>
#include <Srf08.h>

extern BioloidController bioloid;

// robot specific port definitions
#define PIN_FAN         31      // A0 - FAN OUTPUT
#define PIN_HEAD_IR     0x01    // A1 
#define PIN_FRONT_L     0x04	// A4 - LEFT FRONT IR 
#define PIN_FRONT_R     0x05    // A5 - RIGHT FRONT IR
#define PIN_RIGHT_IR    0x06	// A6 - RIGHT IR  
#define PIN_UV          0x07    // A7 - UV SENSOR

#define PIN_WALL_LED    0x00    // D0
#define PIN_START	0x02	// D2 - START
#define PIN_PROG_LED    22      // D22
#define HEAD_SERVO      1

// 30:1 Gear Motors, 100CPR Encoders, 3.875" Wheels
#define COUNTS_CM(i)        (262 * (i))
#define COUNTS_DEGREE(a)    (27 * (a)) //(28 * (a))
#define COUNTS_OVERSHOOT    1200

/* 
 * Motor/Encoder Stuff
 */ 
#define FORWARD             1
#define BACKWARD	    -1
#define REG_SPEED	    150
#define STOP		    0			

/* sets up microprocessor for PWM control of motors */
void motorInit(void);
/** pwm values can range from -255 (full-speed reverse)
    to 255 (full-speed forward), with 0 indicating a stop */
void motorLeft(int pwm);
/** pwm values can range from -255 (full-speed reverse)
    to 255 (full-speed forward), with 0 indicating a stop */
void motorRight(int pwm);

/** Stops both motors */
void motorStop(void);
/** Restarts both motors */
void motorResume(void);

/**************************CLOSED-LOOP FUNCTIONS**************************/
extern volatile int rCount;
extern volatile int rLastB;
extern volatile int lCount;
extern volatile int lLastB;

/** This enables the closed loop control, still need to call sei() */
void closedInit();

/** Counter get/set macros */
#define getLcount 		lCount
#define getRcount 		rCount
#define clearCounters 	lCount = rCount = 0

/* 
 * Some higher level movement
 */
 
/** causes robot to move x CM (positive = forward) */
void moveX(int x);
/** turns angle degees (positive = right) */
void turnX(int angle);

/*
 * Sensory 
 */
extern SharpIR headIR;
extern SharpIR frontLeftIR;
extern SharpIR frontRightIR;
extern SharpIR RightIR;
extern Tpa81 thermo;
extern Srf08 sonar;

void uvtronInit(char channel);
int uvtronGetData(char channel);
void uvtronClearData(char channel);

#define getIR headIR.getSmoothData

#define PING_SONAR      sonar.ping()
#define GET_SONAR       sonar.getData()

#define FOLLOWING_LEFT  digitalWrite(PIN_WALL_LED, HIGH)
#define FOLLOWING_RIGHT digitalWrite(PIN_WALL_LED, LOW)
#define FAN_ON          digitalWrite(PIN_FAN,HIGH)
#define FAN_OFF         digitalWrite(PIN_FAN,LOW)

#endif
