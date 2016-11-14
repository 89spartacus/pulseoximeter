//Pulse Oximeter based on TSL230
//Copyright by Andreas Faulhaber

//Libraries:
#include <Average.h>//Array-Average functions
#define BUFLEN 70   //70 datapoints equal to 3,125s sample time with 45ms measuring times
Average<uint16_t> redBuf(BUFLEN);  //redBuf=array, size=BUFLEN, format=int
Average<uint16_t> irBuf(BUFLEN);
Average<uint16_t> dTmBuf(70);
uint16_t dTm, AVG, CrossBufCnt=0, CrossBuf[0];
unsigned long dTmAVG;

//Define Sensor (LED + PD)
int redLED = 8; int irLED = 9;
int TSL_Pin = 2; //TSL "digital" reading
int TSL_S0 = 3; int TSL_S1 = 4; int TSL_S2 = 5; int TSL_S3 = 6;//TSL input - sensor configuration - see function setupTSL
int TSL_divider=2;
int TSL_sensitiv=100;

//Operands for sensor data aquisition (TM=Time)
int readTM = 45; //sampling period of TSL (best signal @ 45ms through experimenting)
unsigned long currentTM,startTM,freq;
volatile unsigned long pulse_cnt;

//Operands for data processing
bool ledState;//true=RED, false=IR
bool redState=1, irState=0;
unsigned long red,ir,Rmax,Rmin,IRmax,IRmin; //MIN,MAX values per Red,IR
float rRed,rIR,R,HR,SpO,Heartrate,LastHeartrate;//values for calculation

//HR and SpO may need [FLOAT to INT] for LCD display?!?!?!?!

//LCD
//#include "U8glib.h"
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);


void setup(){
  pinMode(redLED, OUTPUT);  pinMode(irLED, OUTPUT);
  currentTM=0;startTM=0;pulse_cnt=0;
  R=0;Rmax=0;Rmin=0;IRmax=0;IRmin=0;HR=60;

  ledSwitch(redState);//turn on RED or IR according true or false
  setupTSL();//function to call several configuration settings (sensitivity, frequency-divider)
  
  //initialize the TSL digital read on PinX on interrupt
  attachInterrupt(digitalPinToInterrupt(TSL_Pin), add_pulse, RISING); //Timer3(16bit; MEGA2560) interrupt on changing Sensor value (PIN2/3/5 MEGA; Pin11/13 UNO) adding a pulse to counter

  Serial.begin(9600);
  Serial.println("PULSE OXIMETER - Loading...");delay(200);Serial.println("60 HR 99% SpO2");
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

unsigned long readTSL(){//returns "freq" frequency/intensity from sensor reading //function or procedure to calculate the frequency from the called interrupt counter  
  currentTM = millis();
  if((currentTM - startTM) >= readTM)// once reaching sampling period - save value & reset the ms counter
    {
      startTM = currentTM;
      freq = pulse_cnt * TSL_divider;
      pulse_cnt = 0;
    } 
  return(freq);
}

void add_pulse() {//ISR adding a pulse count to every interrupt from sensor I/O
  pulse_cnt++;
}

void ledSwitch(bool ledState){ //color-state: RED=TRUE=redState, IR=FALSE=irState, ////used to have to measure ambient light
  bool Red_pin = HIGH;
  bool IR_pin = LOW;
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

void loop(){
   for(int c=0;c<BUFLEN;c++){
      ledState=redState;//red measurements
      ledSwitch(ledState);
      pulse_cnt=0; //resetting counter to make sure no ambient or other light is measured beforehand
      delay(readTM);            //wait til Sensor has read
      red=readTSL();        //read Sensor value into "red"-variable
      redBuf.push(red);     //push "red"-Value into "redBuf"-Array
      
      ledState = irState;     //switch to other LED state
      ledSwitch(ledState);  //call the function "ledSwitch" to switch LED
      pulse_cnt=0;
      //IR measurements
      delay(readTM);
      ir=readTSL();
      irBuf.push(ir);
      Serial.println(ir);
   }
   noInterrupts();
   //calculation of Max,Min,Avg on arrays
   Rmax=redBuf.maximum();
   Rmin=redBuf.minimum();
   IRmax=irBuf.maximum();
   IRmin=irBuf.minimum();
   AVG=(IRmax+IRmin)/2;
    
   //check AVG-Crossing/Peak detection algorithm
   for(int i=0; i<BUFLEN; i++){
     if(irBuf.get(i) > AVG && irBuf.get(i+1) < AVG){
       CrossBuf[CrossBufCnt]=i;
       CrossBufCnt++;
     }
   }
   for(int j=0; j<CrossBufCnt; j++){
    dTm = CrossBuf[j+1] - CrossBuf[j];
    dTmBuf.push(dTm);
   }
   dTmAVG=((dTmBuf.maximum()+dTmBuf.minimum())/2);
   //calculate HR via time difference "dTmAVG" between peak counts
   HR=((1/ float(dTmAVG)*45)*60000);
   //calc ratios & SpO2
   rRed=(float(Rmax-Rmin)/Rmin);//normalized Ratio of Red absorbance
   rIR=(float(IRmax-IRmin)/IRmin); //normalized Ratio of IR absorbance
   R=(rRed/rIR); //Ratio of ratios
   SpO=108-20*R-0.375*R*R; //SpO2 calculation currently based on literature
   //Print data to LCD
       //still needs to be done!!!! 
   String HRSPO = int(HR)+String(" HR ")+int(SpO)+String("% SpO2"); //creating string for data display
   Serial.println(HRSPO);Serial.println(IRmin);Serial.println(AVG);Serial.println(IRmax);Serial.println(dTmAVG);Serial.println(irBuf.getCount());

   //reset the BufferCounters to roll over
   irBuf.clear();redBuf.clear();CrossBufCnt=0;dTmBuf.clear();Rmin=0;Rmax=0;IRmin=0;IRmax=0;R=0;
   interrupts();
}
