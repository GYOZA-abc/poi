#include "DigiJoystick.h"

const int LEFTPIN = 0;
const int LEDPIN = 1;
const int RIGHTPIN = 2;

const int DETECT_SHORT = 50;
int count_low = 0;

int val0_counter = 0;
int val1_counter = 0;

int val0 = 0;
int val1 = 0;

bool val1_flag = false;
bool val0_flag = true;

bool output_flag1 = false;
bool output_flag2 = false;

const int THRESHOLD_EACH_STEP_TIME = 1000;//ms
unsigned long timer1 = 0, timer2 = 0;

unsigned char stickBuf[8]={127,127,127,127,127,127,0,0};// x, y, xrot, yrot, zrot, slider, buttonLowByte, buttonHighByte

unsigned long duration1 = 0,duration2 = 0;
int buf1 = 0,buf2 = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT_PULLUP); //Tilt 1
  pinMode(2, INPUT_PULLUP); //Tilt 2
  pinMode(1, OUTPUT); //LED
  digitalWrite(1,LOW);
}

void loop() {
  /*
   * Get Input
   */
  val0 = digitalRead(LEFTPIN); // LEFT -> val0
  val1 = digitalRead(RIGHTPIN); // RIGHT -> val1  

  /*
   * Step Detection
   */
  if(val0 == LOW && val1 == HIGH){
    val0_counter++;
    val1_counter = 0;  
  }else if(val0 == HIGH && val1 == LOW){
    val1_counter++;
    val0_counter = 0;  
  }

  // Ignore Chattering 
  if(val0_counter > DETECT_SHORT && val0_flag){
    //Detect Walk of one side which is left or right
    val0_flag = false;
    val1_flag = true;
    //LED
    digitalWrite(LEDPIN,HIGH);
    
    check_flag_and_setval();
  }
  else if(val1_counter > DETECT_SHORT && val1_flag){
    //Detect Walk of another side which is left or right
    val0_flag = true;
    val1_flag = false;
    //LED off
    digitalWrite(LEDPIN,LOW);

    check_flag_and_setval();

  }

  /*
   * Output
   */
  
  duration1 = abs(millis() - timer1);
  duration2 = abs(millis() - timer2);
  buf1 = 0;
  buf2 = 0;
  
  if((duration1 < THRESHOLD_EACH_STEP_TIME) && output_flag1){
    buf1 =  (int)(127 * sin((float)duration1/THRESHOLD_EACH_STEP_TIME*PI));
  }else{
    output_flag1 = false;
  }

  if((duration2 < THRESHOLD_EACH_STEP_TIME) && output_flag2){
    buf2 =  (int)(127 * sin((float)duration2/THRESHOLD_EACH_STEP_TIME*PI));
  }else{
    output_flag2 = false;
  }

  stickBuf[1] = max(127 - buf1 - buf2,0);
  
  DigiJoystick.setValues(stickBuf);
  DigiJoystick.update(); 
  
}

void check_flag_and_setval(){
      if(!output_flag1){
      output_flag1 = true;
      timer1 = millis();
    }else if(!output_flag2){
      output_flag2 = true;
      timer2 = millis();
    }else{//overwrite 1
      output_flag1 = true;
      timer1 = millis();
    }
}
