/* Compiled with Board: ATTinyCore, ATtiny 25/45/85 (No bootloader) */
#define IR_SMALLD_NEC
#define IR_RX_PIN  PB2   /* ATTiny85 pin 7 */
//#define Z3_BLINK             /* Set up IR for LT/RT blinker, tail lights, brakes, Z3 remote ports */
                             /* When false:  Set up IR for remaining FOG lights, head, parking lights, high beam, dash light Z3 remote ports */
#ifdef Z3_BLINK
#define LIGHTS_PIN        PB3  /* ATTiny85 pin 2 */
#define LT_LIGHTS_PIN     PB4  /* ATTiny85 pin 3 */
#define RT_LIGHTS_PIN     PB1  /* ATTiny85 pin 6 */
#define BRAKE_LIGHTS_PIN  PB0  /* ATTiny85 pin 5 */
#else
#define LIGHTS_PIN        PB3  /* ATTiny85 pin 2 */
#define FOG_LIGHTS_PIN    PB4  /* ATTiny85 pin 3 */
#define HB_LIGHTS_PIN     PB1  /* ATTiny85 pin 6 */
#define HL_LIGHTS_PIN     PB0  /* ATTiny85 pin 5 */
#endif

#define LIGHTS_CMD        28
#define FOG_LIGHTS_CMD    13
#define HL_LIGHTS_CMD     24
#define HB_LIGHTS_CMD     22
#define LT_LIGHTS_CMD     8
#define RT_LIGHTS_CMD     90
#define BRAKE_LIGHTS_CMD  82

#include <IRsmallDecoder.h>

IRsmallDecoder irDecoder(IR_RX_PIN); //assuming that the IR sensor is connected to digital pin X  
irSmallD_t irData;
int lightsState=LOW;     /* OK, Command: 28, Parking Front/Tail lights, Dash lights */
int foglightsState=LOW;  /* #, Command: 13, Fog lights */
int hblightsState=LOW;   /* *, Command: 22, High beam lights */
int hllightsState=LOW;   /* Up arrow, Command: 24, Head lights */
int leftturnState=LOW;   /* Left arrow, Command: 8, Left turn lights */
int rightturnState=LOW;  /* Right arrow, Command: 90, Right turn lights */
int brakeState=LOW;      /* Down arrow, Command: 82, Brake turn lights */
int brakeHeld=0;         /* Track holding of brake button */
int blink=LOW;           /* State of 1 sec blinker */

//int reverseState=LOW;  /* * */
//int highbeamState=LOW; /* # */

// NEC Protocol
// Key 1:
// Command: 69
// P=8 A=0x0 C=0x45 Raw=0xBA45FF00
// 
// Key 2:
// Command: 70
// P=8 A=0x0 C=0x46 Raw=0xB946FF000
// 
// Key 3:
// Command: 71
// P=8 A=0x0 C=0x47 Raw=0xB847FF00
// 
// Key 4:
// Command: 68
// P=8 A=0x0 C=0x44 Raw=0xBB44FF00
// 
// Key 5:
// Command: 64
// P=8 A=0x0 C=0x40 Raw=0xBF40FF00
// 
// Key 6:
// Command: 67
// P=8 A=0x0 C=0x43 Raw=0xBC43FF00
// 
// Key 7:
// Command: 7
// P=8 A=0x0 C=0x7 Raw=0xF807FF00
// 
// Key 8:
// Command: 21
// P=8 A=0x0 C=0x15 Raw=0xEA15FF00
// 
// Key 9:
// Command: 9
// P=8 A=0x0 C=0x9 Raw=0xF609FF00
// 
// 
// Key 0:
// Command: 25
// P=8 A=0x0 C=0x19 Raw=0xE619FF00
// 
// Key *:
// Command: 22
// P=8 A=0x0 C=0x16 Raw=0xE916FF000
// 
// Key #:
// Command: 13
// P=8 A=0x0 C=0xD Raw=0xF20DFF000
//
// Key OK:
// Command: 28
// P=8 A=0x0 C=0x1C Raw=0xE31CFF00
// 
// Key UP:
// Command: 24
// P=8 A=0x0 C=0x18 Raw=0xE718FF00
// 
// Key DOWN:
// Command: 82
// P=8 A=0x0 C=0x52 Raw=0xAD52FF00
// 
// Key LEFT:
// Command: 8
// P=8 A=0x0 C=0x8 Raw=0xF708FF00
// 
// Key RIGHT:
// Command: 90
// P=8 A=0x0 C=0x5A Raw=0xA55AFF000
// 

void setup() {
  // setup code here, to run once:
#ifdef Z3_BLINK
   pinMode(LIGHTS_PIN, OUTPUT);      
   pinMode(LT_LIGHTS_PIN, OUTPUT);   
   pinMode(RT_LIGHTS_PIN, OUTPUT);   
   pinMode(BRAKE_LIGHTS_PIN, OUTPUT);
#else
   pinMode(LIGHTS_PIN, OUTPUT);      
   pinMode(FOG_LIGHTS_PIN, OUTPUT);  
   pinMode(HB_LIGHTS_PIN, OUTPUT);
   pinMode(HL_LIGHTS_PIN, OUTPUT);
#endif
} /* emd setup */

int blinkCnt = 0;

void loop() {
      if(irDecoder.dataAvailable(irData)){
        if (irData.keyHeld) {
#ifdef Z3_BLINK
            // Momentary on for brakes
            if (irData.cmd == BRAKE_LIGHTS_CMD) {
              //  brakeHeld = 32765; /* Maximum int value, 32767 */
                brakeHeld = 2; /* Maximum int value, 32767 */
                if (!brakeState) {
                    brakeState = HIGH;
                    digitalWrite(BRAKE_LIGHTS_PIN, brakeState);
                }
            } /* end brake lights cmd */
#endif
        } else {
            if (irData.cmd == LIGHTS_CMD) {
                lightsState = (lightsState == LOW)? HIGH : LOW; //toggle lights
                digitalWrite(LIGHTS_PIN, lightsState);
                
                if (!lightsState) {
                    /* Turn off all lights too */
                    foglightsState=LOW;
                    hblightsState=LOW;
                    hllightsState=LOW;
                    leftturnState=LOW;   
                    rightturnState=LOW;  
                    brakeState=LOW;

//#if defined (Z3_BLINK)
#ifdef Z3_BLINK
                    digitalWrite(LT_LIGHTS_PIN, leftturnState);
                    digitalWrite(RT_LIGHTS_PIN, rightturnState);
#else
                    digitalWrite(FOG_LIGHTS_PIN, foglightsState);
                    digitalWrite(HB_LIGHTS_PIN, hblightsState);
                    digitalWrite(HL_LIGHTS_PIN, hllightsState);
#endif
                }
            } /* end lights cmd */

//#if defined (Z3_BLINK)
#ifdef Z3_BLINK
            if (irData.cmd == LT_LIGHTS_CMD) {
                leftturnState = (leftturnState == LOW)? HIGH : LOW; //toggle lights
    
                /* Disable any RT turn sig */
                rightturnState = LOW;
                digitalWrite(RT_LIGHTS_PIN, rightturnState);
                digitalWrite(LT_LIGHTS_PIN, leftturnState);
            } /* end LT lights cmd */
    
            if (irData.cmd == RT_LIGHTS_CMD) {
                rightturnState = (rightturnState == LOW)? HIGH : LOW; //toggle lights
    
                /* Disable any LT turn sig */
                leftturnState = LOW;
                digitalWrite(LT_LIGHTS_PIN, leftturnState);
                digitalWrite(RT_LIGHTS_PIN, rightturnState);
            } /* end RT lights cmd */
#else
            if (irData.cmd == FOG_LIGHTS_CMD) {
                foglightsState = (foglightsState == LOW)? HIGH : LOW; //toggle lights
                digitalWrite(FOG_LIGHTS_PIN, foglightsState);
            } /* end fog lights cmd */

            if (irData.cmd == HB_LIGHTS_CMD) {
                hblightsState = (hblightsState == LOW)? HIGH : LOW; //toggle lights
                digitalWrite(HB_LIGHTS_PIN, hblightsState);
            } /* end High beam lights cmd */

            if (irData.cmd == HL_LIGHTS_CMD) {
                hllightsState = (hllightsState == LOW)? HIGH : LOW; //toggle lights
                digitalWrite(HL_LIGHTS_PIN, hllightsState);
            } /* end Head lights cmd */
#endif
        } /* end !IR Held */
      } /* end if IR data */

#ifdef Z3_BLINK
      if (rightturnState) digitalWrite(RT_LIGHTS_PIN, blink);
      if (leftturnState)  digitalWrite(LT_LIGHTS_PIN, blink);

      if (brakeState) {      
          if (brakeHeld <= 0) {
              brakeHeld = 0;
              brakeState = LOW;
              digitalWrite(BRAKE_LIGHTS_PIN, brakeState);
          } else {
              brakeHeld--;
          }
      }
#endif

      delay (100);

#ifdef Z3_BLINK
      blinkCnt++;
      /* Trying for about 1/2 sec cycle */
      if (blinkCnt > 4) {
          blinkCnt = 0;
          blink = (blink == LOW)? HIGH : LOW; //toggle blink
      }
#endif
} /* end loop */
