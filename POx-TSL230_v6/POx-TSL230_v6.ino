//Pulse Oximeter based on TSL230
//Copyright 2016 by Andreas Faulhaber

//Define Sensor (LED + PD)
int redLED = 8; int irLED = 9;
//TSL-OUT output
int TSL_Pin = 3;
//TSL input - sensor configuration (setupTSL)
//int TSL_S0 = 3; int TSL_S1 = 4; int TSL_S2 = 5; int TSL_S3 = 6;
int TSL_divider=2;
int TSL_sensitiv=100;

//Operands for sensor data aquisition (TM=Time)
int readTM = 45; //sampling period of TSL (best signal @ 45ms through experimenting)
unsigned long currentTM,startTM,freq;
volatile unsigned long pulse_cnt;

//Operands for data processing
boolean ledState;//true=RED, false=IR
unsigned long Cnt,average,lastaverage,red,ir,lastred,lastir,Rmax,Rmin,IRmax,IRmin;
float rRed,rIR,HR,R,SpO,Heartrate,LastHeartrate;//values for calculation
//HR and SpO may need [FLOAT to INT] for LCD display?!?!?!?!

//Display
#include "U8glib.h"
int printHR, printO2;boolean reDraw=true;
#define logo_width 128
#define logo_height 56
static unsigned char logo[] U8G_PROGMEM = { /*insert xbm logo data here*/};
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);

void setup(){
  pinMode(redLED, OUTPUT);  pinMode(irLED, OUTPUT);
  currentTM=0;startTM=0;pulse_cnt=0;Cnt=0;average=0;  
  R=0;Rmax=0;Rmin=10000;IRmax=0;IRmin=10000;
  ledSwitch(false);//turn on RED or IR according true or false
  //initialize the TSL digital read on PinX on interrupt
  attachInterrupt(digitalPinToInterrupt(TSL_Pin), add_pulse, RISING);
  //setupTSL();//function to call sensor configuration settings (sensitivity, frequency-divider)
  Serial.begin(9600);
  Serial.println("PULSE OXIMETER - Loading...");
  
  //draw welcoming logo
  u8g.setColorIndex(1);//pixel on
    if(reDraw){u8g.firstPage();
      while(u8g.nextPage()){
        u8g.drawXBMP( 0, 8, logo_width, logo_height, logo);}}
    reDraw=false;
    delay(2000);
}

void add_pulse() {//ISR adding a pulse count to every interrupt from sensor I/O
  pulse_cnt++;}
unsigned long readTSL(){//returns "freq" frequency/intensity from sensor reading  
  currentTM = millis();
  if((currentTM - startTM) >= readTM)// once reaching sampling period - save value & reset the ms counter
    {startTM = currentTM;
    freq = pulse_cnt * TSL_divider;
    pulse_cnt = 0;} 
  return(freq);}



void ledSwitch(bool ledState){ //color: RED=TRUE, IR=FALSE
  int Red_pin = HIGH;
  int IR_pin = LOW;
  if(ledState){//RED LED - ON
    Red_pin = HIGH;
    IR_pin = LOW;}
  else {    //IR LED - ON
    Red_pin = LOW;
    IR_pin = HIGH;}
  digitalWrite(redLED, Red_pin);
  digitalWrite(irLED, IR_pin);}

void loop(){
  //red measurement
  if(ledState){
    delay(45);
    red=readTSL();
    if(red>Rmax){Rmax=red;}
    else if(red<Rmin){Rmin=red;}
    ledState=!ledState;//switch to IR LED
    ledSwitch(ledState);}
  //IR measurement
  if(!ledState){
    delay(45);
    lastir=ir;//keep last ir for average-crossing comparison
    ir=readTSL();
    if(ir>IRmax){IRmax=ir;}
    else if(ir<IRmin){IRmin=ir;}
    Cnt++;//count the times the loop has ran (for HR measurement)
    if(IRmin!=10000){//calculate Average of Min & Max
      average=((IRmax + IRmin)/2);}
    ledState=!ledState;//switch to RED LED
    ledSwitch(ledState);}
  //Average-Crossing and calculations
  if((lastir>average)&&(ir<average)&&(Rmin<10000)&&(50<IRmax-IRmin)){
    HR = (1/(0.09*(float)Cnt) )*60;//heartrate in bpm (*60s) from measured frequency over t=0.09s times the counts (isrCnt) of average crossings of heartbeat
    if(40<HR&&HR<180){
      rRed=(float(Rmax-Rmin)/Rmin);//normalized Ratio of Red absorbance
      rIR=(float(IRmax-IRmin)/IRmin); //normalized Ratio of IR absorbance
      R=(rRed/rIR); //Ratio of ratios
      if(0.397<R&&R<1.5){
        SpO=108-20*R-0.375*R*R;} //SpO2 calculation currently based on literature
      else{SpO=99;}
      printO2=int(SpO);printHR=int(HR);
      String HRSPO = int(HR)+String(" HR ")+int(SpO)+String("% SpO2"); //creating string for PC display
      Serial.println(HRSPO);

      // picture loop
      u8g.firstPage();  
      do {draw_header();draw_values();} while( u8g.nextPage() );}
      
    delay(1);//delay for stabilisation
    Rmax=0;Rmin=10000;IRmax=0;IRmin=10000;average=0;Cnt=0;lastir=0;//resetting variables
  }
  ///hard-reset if finger was temporarily removed loop continues and wont run into HR-check 
}

void draw_header(void) {
  u8g.setFont(u8g_font_unifont);
  u8g.setFontPosTop();
  u8g.drawStr( 0, 0, "HR[bpm]");
  int w=u8g.getStrWidth("SpO2[%]");
  u8g.drawStr(128-w,0,"SpO2[%]");
  u8g.drawHLine(0,15,128);}
void draw_values(){
  u8g.setFont(u8g_font_fur30);
  u8g.setFontPosBottom();
  u8g.setPrintPos(0,64);
  u8g.print(printHR);
  u8g.setPrintPos(85,64);
  u8g.print(printO2);}

/*void setupTSL(){
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
*/
