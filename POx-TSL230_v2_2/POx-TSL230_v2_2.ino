//Copyright by Andreas Faulhaber
int redLED = 8; int irLED = 9;
int TSL_Pin = 3; //TSL "digital" output
int TSL_S0 = 3; int TSL_S1 = 4;
int TSL_S2 = 5; int TSL_S3 = 6;
int freq_multi=2;
int sensitivity=100;

int readTM = 45;
unsigned long currentTM=millis();
unsigned long startTM= currentTM;
unsigned long diffTM=0;
volatile unsigned long temp_pulse_cnt=0,pulse_cnt=0;
//unsigned long pulse_cnt=0;

unsigned long value=0,freq;

void setup(){
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(irLED, OUTPUT);
  digitalWrite(redLED, HIGH); //?!?!?!adjust brightness of LED to measured value through PWM mode (analogWrite() )
  digitalWrite(irLED, LOW);
  setupTSL();
  attachInterrupt(digitalPinToInterrupt(3), add_pulse, RISING); //interrupt on changing Sensor value (PIN2) adding a pulse to counter
  //TIMERONE implementation to give sampling period to interrupt at a certain time period

}
void add_pulse() {//ISR adding a pulse count to every interrupt
  pulse_cnt++;
  
  
  }

unsigned long readTSL(){
  //function or procedure to calculate the frequency from the called interrupt counter
  //overwrite start with previous value
      startTM   = currentTM;
  currentTM = millis();
//  if( currentTM > startTM ) {
//    diffTM += currentTM - startTM;
//    }
  // if enough time has passed to do a new reading...
  if(/*diffTM*/ (currentTM - startTM) >= readTM) {// once reaching sampling period - save value & reset the ms counter

    freq = pulse_cnt * freq_multi;
    //temp_pulse_cnt = pulse_cnt;
    pulse_cnt = 0;
    diffTM = 0;
    }
  //unsigned long freq = temp_pulse_cnt * freq_multi; //multiply counts by the sensors divide-by factor to solve for frequency
  return(freq);
}

void loop(){

//
//  digitalWrite(redLED, HIGH);
//  long  raw_red = readTSL();
//  delay(15);
//  //Serial.print("R");
//  Serial.print(raw_red);
//  digitalWrite(redLED, LOW);
//  long  amb = readTSL();
//  delay(45);
// // Serial.print(",");Serial.println(amb);
//  digitalWrite(irLED,HIGH);
//  long raw_ir = readTSL();
//  unsigned long red=(raw_red - amb);
//  unsigned long  ir= (raw_ir - amb);
//  delay(15);
//Serial.print(",");Serial.println(raw_ir);
//digitalWrite(irLED,LOW);
//  delay(45);
 
// get our current frequency reading
  //value = readTSL();
  delay(45);
  Serial.println(readTSL());

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
  digitalWrite(TSL_S3,LOW);
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

