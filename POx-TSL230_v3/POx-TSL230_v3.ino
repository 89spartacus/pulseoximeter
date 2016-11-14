//Pulse Oximeter based on TSL230
//Copyright by Andreas Faulhaber
#include <TimerOne.h> //interrupts

//Define Sensor (LED + PD)
int redLED = 8; int irLED = 9;
int TSL_Pin = 2; //TSL "digital" output
int TSL_S0 = 3; int TSL_S1 = 4;
int TSL_S2 = 5; int TSL_S3 = 6;
int TSL_divider=2;
int TSL_sensitiv=100;

//Operands for sensor data aquisition (TM=Time)
int readTM = 15; //sampling period of TSL
unsigned long diffTM = 0;
unsigned long currentTM = millis();
unsigned long startTM = currentTM;
unsigned long pulse_cnt = 0;
volatile unsigned long temp_pulse_cnt;

//Operands/operator for Data processing
volatile unsigned long maxTmp,minTmp,average,lastcnt,cnt,isrCnt;
unsigned long Rmax, Rmin;
float HR_freq,HR,R,SpO,Theta=0.6;
unsigned int Heartrate,LastHeartrate;

void setup(){ //Initialization block
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(irLED, OUTPUT);
  digitalWrite(redLED, HIGH);
  digitalWrite(irLED, LOW);
  attachInterrupt(digitalPinToInterrupt(2), add_pulse, RISING); //interrupt on changing Sensor value (PIN2) adding a pulse to counter
  //TIMERONE implementation to give sampling period to interrupt at a certain time period
  setISR(); 
  Timer1.initialize(40000); //interrupt every 0.04 seconds
  Timer1.attachInterrupt(min_max_isr);
  LastHeartrate = 60;R=0;Rmax=0;Rmin=0;isrCnt=0;
  setupTSL();
  Serial.println("Start...");
}

void add_pulse() {//ISR adding a pulse count to every interrupt from sensor I/O
  pulse_cnt++;
  
  startTM   = currentTM;
  currentTM = millis();
  if( currentTM > startTM ) {
    diffTM += currentTM - startTM;
    }
  // if enough time has passed to do a new reading...
  if(diffTM >= readTM) {// once reaching sampling period - save value & reset the ms counter
    temp_pulse_cnt = pulse_cnt;
    pulse_cnt = 0;
    diffTM = 0;
    }
}

unsigned long readTSL(){//returns "freq" frequency/intensity from sensor
  //function or procedure to calculate the frequency from the called interrupt counter
  unsigned long freq = temp_pulse_cnt * TSL_divider; //multiply counts by the sensors divide-by factor to solve for frequency
  return(freq);
}

void led_state (int color){ //color: RED=1, IR=2, OFF=0
  int Red_pin = HIGH;
  int IR_pin = LOW;
  switch(color){
    case 0:
      Red_pin = LOW;
      IR_pin = LOW;
      break;
    case 1:
      Red_pin = HIGH;
      IR_pin = LOW;
      break;
    case 2:
      Red_pin = LOW;
      IR_pin = HIGH;
      break;
    default:
  return;
  }
digitalWrite(redLED, Red_pin);
digitalWrite(irLED, IR_pin);
}

void setISR(){//ISR values reset or initialize
  maxTmp=0;
  minTmp=10000;
  cnt=0;
  lastcnt=0;
  isrCnt=0;
}

void min_max_isr(){//ISR for reading sensor value and checking max_min
  lastcnt=cnt;//keep previous value
  cnt=readTSL();//read new value
  if(cnt>maxTmp) {//check if value is max
    maxTmp=cnt;
    }
  else if(cnt<minTmp){//check if value is min
    minTmp=cnt;
    }
  isrCnt++;//add ISR counter

}

void loop(){
  led_state(2);
  //Serial.println(isrCnt);
    average=(maxTmp+minTmp)/2;
  if((lastcnt>average)&& (cnt<average)) {
    noInterrupts();
    //average=(maxTmp+minTmp)/2;
    HR_freq = 1/(0.04*isrCnt);
    Rmax = maxTmp; Rmin = minTmp;
    interrupts();
    HR = HR_freq * 60;
    R = (Rmax - Rmin);
    SpO = (R-180)*0.01 +97.838;
    String HRSPO = HR+String(" HR ")+SpO+String(" SpO2%"); 
    Serial.println(HRSPO);
    delay(100);
    setISR();
    }
//  while(!((lastcnt>average )&& (cnt<average)) ){}
//    noInterrupts(); // temporarily disabel interrupts, to be sure it will not change while we are reading 
//    Rmax = maxTmp; Rmin = minTmp; 
//    delay(40); 
//    HR_freq = 1/(0.015*isrCnt); //isrCnt is the times of ISR in 1 second, 
//    interrupts(); //enable interrupts
//    HR = HR_freq * 60;
////    if(HR>40 && HR<150){
////      Heartrate = Theta*Heartrate + (1 - Theta)*LastHeartrate; //Use theta to smooth 
////      LastHeartrate = HR;
////      }
//    //led_state(2);
//    R = (Rmax - Rmin);
//    SpO = (R-180)*0.01 +97.838;
//    String HRSPO = HR+String(" HR ")+SpO+String(" SpO2%"); 
//    Serial.println(HRSPO);
//    delay(100);
//    setISR();
    
   



//
//  float raw_red = readTSL(TSL_sample);
//  led_state(0);
//  float amb = readTSL(TSL_sample);
//  led_state(2);
//  float raw_ir = readTSL(TSL_sample);
//  float red=raw_red-amb;
//  float ir= raw_ir-amb;
//  return red,ir;

// get current frequency reading
//  unsigned long value = readTSL();
//  Serial.println(value);
//  delay(100);

//change_sensitivit() //true to increase, false to decrease sensitivity
  //use threshold to determine if change_sensitivity needs adjustment //max_thres; min_thres
//set_scaling () //"2, 10, 100" as divide-by factors
}

void setupTSL(){
  pinMode(TSL_S0, OUTPUT); 
  pinMode(TSL_S1, OUTPUT);
  pinMode(TSL_S2, OUTPUT);
  pinMode(TSL_S3, OUTPUT);
  pinMode(TSL_Pin, INPUT);

  //configure initial sensitivity
  //S1 LOW  | S0 HIGH: low 1x
  //S1 HIGH | S0 LOW:  med 10x
  //S1 HIGH | S0 HIGH: high 100x
  digitalWrite(TSL_S1, HIGH);
  digitalWrite(TSL_S0, HIGH);

  //config initial scaling
  //S3  S2  SCALING (divide-by)
  //L   L   1
  //L   H   2
  //H   L   10*default
  //H   H   100x
  digitalWrite(TSL_S3, LOW);
  digitalWrite(TSL_S2, HIGH);
}
void change_sensitivity(/*unit8_t*/bool dir ) {
  /*Call function w/ HIGH or LOW to increase or decrease sensitivity level
   * need to measure what to divide freq by
   * 1x sensitivity = 10,
   * 10x sens       = 100,
   * 100x sens      = 1000
   * adjust sensitivity in 3 steps of 10x either direction
  */
  int pin_0;
  int pin_1;
  if( dir == true ) {
      // increasing sensitivity
      // -- already as high as we can get
    if( TSL_sensitiv == 1000 )
      return;
    if( TSL_sensitiv == 100 ) {
        // move up to max sensitivity
      pin_0 = HIGH;
      pin_1 = HIGH;
      }
    else {
        // move up to med. sesitivity
      pin_0 = LOW;
      pin_1 = HIGH;
      }
      // increase sensitivity divider
    TSL_sensitiv *= 10;
  }
  else {
      // reducing sensitivity
      // already at lowest setting
    if( TSL_sensitiv == 10 )
      return;
    if( TSL_sensitiv == 100 ) {
        // move to lowest setting
      pin_0 = HIGH;
      pin_1 = LOW;
      }
    else {
        // move to medium sensitivity
      pin_0 = LOW;
      pin_1 = HIGH;
      }
    // reduce sensitivity divider
    TSL_sensitiv = TSL_sensitiv / 10;
  }
  //chang pin states
  digitalWrite(TSL_S0, pin_0);
  digitalWrite(TSL_S1, pin_1);
  return;
}
void set_scaling ( int what ) {
  // set output frequency scaling
  // adjust frequency multiplier and set proper pin values
  // scale = 2 == TSL_divider = 2
  // scale = 10 == TSL_divider = 10
  // scale = 100 == TSL_divider = 100
  int pin_2 = HIGH;
  int pin_3 = HIGH;
  switch( what ) {
    case 2:
      pin_3     = LOW;
      TSL_divider = 2;
      break;
    case 10:
      pin_2     = LOW;
      TSL_divider = 10;
      break;
    case 100:
      TSL_divider = 100;
      break;
    default:
        // don't do anything with levels
        // we don't recognize
      return;
    }
    // set the pins
  digitalWrite(TSL_S2, pin_2);
  digitalWrite(TSL_S3, pin_3);
  return;
}

