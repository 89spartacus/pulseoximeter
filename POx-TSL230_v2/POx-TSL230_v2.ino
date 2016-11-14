//Copyright by Andreas Faulhaber
int readTM = 35;
int redLED = 8; int irLED = 9;
int TSL_Pin = 2; //TSL "digital" output
int TSL_S0 = 3; int TSL_S1 = 4;
int TSL_S2 = 5; int TSL_S3 = 6;
int freq_multi=10;
int sensitivity=100;
unsigned long currentTM=millis();
unsigned long startTM= currentTM;
unsigned long diffTM=0;

unsigned long pulse_cnt = 0;
unsigned long value=0;

void setup(){
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(irLED, OUTPUT);
  digitalWrite(redLED, LOW); //?!?!?!adjust brightness of LED to measured value through PWM mode (analogWrite() )
  digitalWrite(irLED, HIGH);
  setupTSL();
  attachInterrupt(digitalPinToInterrupt(2), add_pulse, CHANGE); //interrupt on changing Sensor value (PIN2) adding a pulse to counter
  //TIMERONE implementation to give sampling period to interrupt at a certain time period

}
void add_pulse() {//ISR adding a pulse count to every interrupt
  pulse_cnt++;
  return;
  
  }

unsigned long readTSL(){
  //function or procedure to calculate the frequency from the called interrupt counter
  unsigned long freq = pulse_cnt * freq_multi; //multiply counts by the sensors divide-by factor to solve for frequency
  pulse_cnt = 0; //reset interrupt counter
  return(freq);
}

void loop(){
//  float red = readTSL

//void sensor(){
//  digitalWrite(redLED, HIGH);
//  float raw_red = readTSL(TSL_sample);
//  digitalWrite(redLED, LOW);
//  float amb = readTSL(TSL_sample);
//  digitalWrite(irLED,HIGH);
//  float raw_ir = readTSL(TSL_sample);
//  float red=raw_red-amb;
//  float ir= raw_ir-amb;
//  return red,ir;

startTM   = currentTM;
currentTM = millis();
if( currentTM > startTM ) {
  diffTM += currentTM - startTM;
  }
else if( currentTM < startTM ) {// handle overflow and rollover (Arduino 011)
  diffTM += ( currentTM + ( 34359737 - startTM )); //that number is undefined?!
  } 
   // if enough time has passed to do a new reading...
if( diffTM >= readTM ) {
// re-set the ms counter
  diffTM = 0;
 
// get our current frequency reading
  unsigned long value = readTSL();
  Serial.println(value);
  
  }

/*
 * switch LED per timer?!interrupt?!
 * per LED: max min function - count time between max-min (timer2 ?)
   * lastcount=count;
   * count=freq or readTSL or similar;
   * if(count>max){max=count;}
   * else if (count<min){min=count;}
   * c++;
   * if((lastcount>average) && (count<average)){
   * average=((max+min)/2); //average calculation
   * a=max;b=min; 
   * calculate AC = max-min; 
   * calculate HR = 1/(0.01*ISR_freq or d)*60
*/
//change_sensitivit() //true to increase, false to decrease sensitivity
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
  digitalWrite(TSL_S3, HIGH);
  digitalWrite(TSL_S2, LOW);
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
    if( sensitivity == 1000 )
      return;
    if( sensitivity == 100 ) {
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
    sensitivity *= 10;
  }
  else {
      // reducing sensitivity
      // already at lowest setting
    if( sensitivity == 10 )
      return;
    if( sensitivity == 100 ) {
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
    sensitivity = sensitivity / 10;
  }
  //chang pin states
  digitalWrite(TSL_S0, pin_0);
  digitalWrite(TSL_S1, pin_1);
  return;
}
void set_scaling ( int what ) {//scale 2,10,100
  // set output frequency scaling
  // adjust frequency multiplier and set proper pin values
  // scale = 2 == freq_multi = 2
  // scale = 10 == freq_multi = 10
  // scale = 100 == freq_multi = 100
  int pin_2 = HIGH;
  int pin_3 = HIGH;
  switch( what ) {
    case 2:
      pin_3     = LOW;
      freq_multi = 2;
      break;
    case 10:
      pin_2     = LOW;
      freq_multi = 10;
      break;
    case 100:
      freq_multi = 100;
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

