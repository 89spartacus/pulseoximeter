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
unsigned long currentTM,startTM,freq;
volatile unsigned long pulse_cnt;

//Operands/operator for Data processing
boolean ledState;//true=RED, false=IR
volatile unsigned long maxTmp,minTmp,lastcnt,cnt,isrCnt;//variables from interrupt
unsigned long red,ir,lastred,lastir,Rmax,Rmin,IRmax,IRmin; //MIN,MAX values per Red,IR
float rRed,rIR,HR,R,SpO,Theta=0.55,Heartrate,LastHeartrate,average;//values for calculation
//HR and SpO need FLOAT to INT for LCD display!!!!


void setup(){
  pinMode(redLED, OUTPUT);  pinMode(irLED, OUTPUT);  pinMode(13,OUTPUT);
  currentTM=0;startTM=0;pulse_cnt = 0;
  ledSwitch(true);//turn on RED
  attachInterrupt(digitalPinToInterrupt(TSL_Pin), add_pulse, RISING); //Timer3(16bit; MEGA2560) interrupt on changing Sensor value (PIN2/3/5 MEGA; Pin11/13 UNO) adding a pulse to counter
  //TIMERONE implementation to give sampling period to interrupt at a certain time period
  //setISR(); 
  //Timer1.initialize(45000); //interrupt every 90ms seconds
  //Timer1.attachInterrupt(min_max_isr);
  LastHeartrate=60;R=0;Rmax=0;Rmin=10000;IRmax=0;IRmin=10000;isrCnt=0;
  setupTSL();
  Serial.begin(9600);
  Serial.println("PULSE OXIMETER - Loading...");
}

void add_pulse() {//ISR adding a pulse count to every interrupt from sensor I/O
  pulse_cnt++;}

unsigned long readTSL(){//returns "freq" frequency/intensity from sensor  //function or procedure to calculate the frequency from the called interrupt counter  
  currentTM = millis();
  if((currentTM - startTM) >= readTM)// once reaching sampling period - save value & reset the ms counter
    {
      startTM = currentTM;
      freq = pulse_cnt * TSL_divider;
      pulse_cnt = 0;
    } 
  return(freq);
}

void ledSwitch(bool ledState){ //color: RED=TRUE, IR=FALSE, OFF=/////used to have to measure ambient light
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

//void setISR(){//ISR values reset or initialize
//  maxTmp=0;
//  minTmp=10000;
//  cnt=0;
//  lastcnt=0;
//  isrCnt=0;
//}

//void min_max_isr(){//ISR for reading sensor value and checking max_min
//  lastcnt=cnt;//keep previous value
//  cnt=readTSL();//read new value
//  if(cnt>maxTmp) {//check if value is max
//    maxTmp=cnt;
//    }
//  else if(cnt<minTmp){//check if value is min
//    minTmp=cnt;
//    }
//  isrCnt++;//add ISR counter
//  digitalWrite(13,digitalRead(13)==0);
//}

void loop(){
 
  if(ledState){//red measurements
    delay(45);
    //lastred=red;
    red=readTSL();
    if(red>Rmax){Rmax=red;}
    else if(red<Rmin){Rmin=red;}
    ledState=!ledState;
    ledSwitch(ledState);
    //Serial.print("R: ");Serial.print(Rmax);Serial.print(", ");Serial.println(Rmin);
    //Serial.print(red);Serial.print(", ");
  }
  if(!ledState){
    delay(45);
    lastir=ir-5;    
    ir=readTSL();
    if(ir>IRmax){IRmax=ir;}
    else if(ir<IRmin){IRmin=ir;}
    isrCnt++;////////////////////////////Com2myself: rename variable after successful functioning needed
    average=((IRmax + IRmin)/2);
    ledState=!ledState;
    ledSwitch(ledState);
    //Serial.print("IR: ");Serial.print(IRmax);Serial.print(", "); Serial.print(IRmin);Serial.print(", "); Serial.print(isrCnt);
    //Serial.print(average);Serial.print(", ");Serial.print(lastir);Serial.print(", ");
    //Serial.println(ir);
  }

 
  if((lastir>average)&&(ir<average)&&(Rmin<10000)){
    HR = (1/(0.09*(float)isrCnt) )*60;//heartrate in bpm (*60s) from measured frequency over t=0.09s times the counts (isrCnt) of average crossings of heartbeat
    rRed=(float(Rmax-Rmin)/Rmin);
    rIR=(float(IRmax-IRmin)/IRmin);
    R=(rRed/rIR);
    SpO=110-25*R;
    String HRSPO = HR+String(" HR ")+SpO+String(" SpO2%"); 
    //Serial.println(rRed);Serial.println(R);
    Serial.println(HRSPO);
    delay(45);
    Rmax=0;Rmin=10000;IRmax=0;IRmin=10000;average=0;isrCnt=0;lastir=0;//resetting variables
  }
  ///hardreset if finger was temporarily removed loop continues and wont run into HR-check 
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

