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
int readTM = 45; //sampling period of TSL (best signal @ 45ms through experimenting)
unsigned long diffTM = 0;
unsigned long currentTM = millis();
unsigned long startTM = currentTM;
unsigned long pulse_cnt = 0;
volatile unsigned long temp_pulse_cnt;

//Operands/operator for Data processing
boolean ledState;//true=RED, false=IR
volatile unsigned long maxTmp,minTmp,lastcnt,cnt,isrCnt;//variables from interrupt
unsigned long Rmax,Rmin,IRmax,IRmin; //MIN,MAX values per Red,IR
float rRed,rIR,HR,R,SpO,Theta=0.55,Heartrate,LastHeartrate,average;//values for calculation
//HR and SpO need FLOAT to INT for LCD display!!!!


void setup(){
  pinMode(redLED, OUTPUT);  pinMode(irLED, OUTPUT);  pinMode(13,OUTPUT);
  ledSwitch(false);//turn on RED
  attachInterrupt(digitalPinToInterrupt(2), add_pulse, RISING); //interrupt on changing Sensor value (PIN2) adding a pulse to counter
  //TIMERONE implementation to give sampling period to interrupt at a certain time period
  setISR(); 
  Timer1.initialize(45000); //interrupt every 90ms seconds
  Timer1.attachInterrupt(min_max_isr);
  LastHeartrate = 60;R=0;Rmax=0;Rmin=0;IRmax=0;IRmin=0;isrCnt=0;
  setupTSL();
  Serial.begin(9600);
  Serial.println("PULSE OXIMETER - Loading...");
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

void ledSwitch(bool ledState){ //color: RED=TRUE, IR=FALSE, OFF=
  int Red_pin = HIGH;
  int IR_pin = LOW;
  if(ledState){   //RED LED - ON
    Red_pin = HIGH;
    IR_pin = LOW;
  }
  else {    //IR LED - ON
    Red_pin = LOW;
    IR_pin = HIGH;
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
  digitalWrite(13,digitalRead(13)==0);
}

void loop(){
 
  if(ledState){//red measurements
    noInterrupts();
    Rmax = maxTmp; Rmin = minTmp;
        maxTmp=0;minTmp=10000;//resetting ISR
    interrupts();
    //maxTmp=0;minTmp=10000;//resetting ISR
    ledState=!ledState;
    ledSwitch(ledState);
    }
  Serial.print("R: ");Serial.print(Rmax);Serial.print(", ");Serial.println(Rmin);//Serial.print(", ");
  //setISR();///dont reset the whole thing?!?!?! only MIN and MAX?!
  delay(45);
  if(!ledState){//ir measurements
    noInterrupts();
    IRmax = maxTmp; IRmin = minTmp;
        maxTmp=0;minTmp=10000;//resetting ISR
    interrupts();
    average=((IRmax + IRmin)/2);

    ledState=!ledState;
    ledSwitch(ledState);
    }
  Serial.print("IR: ");Serial.print(IRmax);Serial.print(", "); Serial.print(IRmin);Serial.print(", "); Serial.println(average);
  delay(45);

    
 
  while((lastcnt>average)&&(cnt<average)){
    noInterrupts();// temporarily disabel interrupts, to ensure no changes while reading 
    //average=(maxTmp+minTmp)/2;
    Rmax = maxTmp; Rmin = minTmp;
    //delay(50);
    //led_state(2);
    HR = (1/(0.09*isrCnt))*60;//heartrate in bpm (*60s) from measured frequency over t=0.09s times the counts (isrCnt) of average crossings of heartbeat
    interrupts();
    //rRed=((Rmax-Rmin)/Rmin);
    //rIR=((IRmax-IRmin)/IRmin);
    //R=(rRed/rIR);
    //SpO=110-25*R;
    R = (Rmax - Rmin);
    SpO = (R-180)*0.01 +97.838;

    /*if(HR>40 && HR<200){
      Heartrate = Theta*HR + (1 - Theta)*LastHeartrate; //Use theta to smooth 
      LastHeartrate = Heartrate;
      }*/
    String HRSPO = HR+String(" HR ")+SpO+String(" SpO2%"); 
    Serial.println(HRSPO);
    delay(100);
    average=0;
    setISR();
  }
 if(minTmp!=10000){
  average=(maxTmp + minTmp)/2;}


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

