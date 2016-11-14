// Help:  http://bildr.org/2011/08/tsl230r-arduino/ 
//Pinner-setup
int redLED = 8; 
int irLED = 9;
int TSL230_Pin = 2; //TSL230 "digital" output
int TSL230_s0 = 3;
int TSL230_s1 = 4;
int TSL230_s2 = 5;
int TSL230_s3 = 6;
int TSL_samples = 4; //higher = slower but more stable and accurate

void setup(){
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(irLED, OUTPUT);
  digitalWrite(redLED, HIGH); //?!?!?!adjust brightness of LED to measured value through PWM mode (analogWrite() )
  digitalWrite(irLED, LOW);
  setupTSL230();
}
void loop(){
  //make this a function to call and receive red and IR sensor values
//  float red = readTSL230(TSL_samples);
//  float red = constrain(red, 0, 10000); //threshold at lightlevel<10000
//  Serial.print(red);
//  digitalWrite(irLED, HIGH);digitalWrite(redLED, LOW);
//  Serial.print(" ");
//  float ir = readTSL230(TSL_samples);
//  ir = constrain(ir, 0, 12000);
//  Serial.print(ir);
//  Serial.println("");
//  digitalWrite(irLED, LOW);digitalWrite(redLED, HIGH);
 //array to hold sample data to measure max min?! https://www.arduino.cc/en/Reference/Array; https://www.arduino.cc/en/Tutorial/Smoothing; http://playground.arduino.cc/Main/RunningAverage
  digitalWrite(redLED, HIGH);
  float raw_red = readTSL230(TSL_samples);
  digitalWrite(redLED, LOW);

  float amb = readTSL230(TSL_samples);
  digitalWrite(irLED,HIGH);
  float raw_ir = readTSL230(TSL_samples);
  digitalWrite(irLED,LOW);
  float red=(raw_red - amb)*5;
  float ir= raw_ir - amb;
  Serial.print(red);Serial.print(" ");  Serial.print(amb);Serial.print(" "); 
  Serial.println(ir); Serial.print("");

}
//void sensor(){
//  digitalWrite(redLED, HIGH);
//  float raw_red = readTSL230(TSL_sample);
//  digitalWrite(redLED, LOW);
//  float amb = readTSL230(TSL_sample);
//  digitalWrite(irLED,HIGH);
//  float raw_ir = readTSL230(TSL_sample);
//  float red=raw_red-amb;
//  float ir= raw_ir-amb;
//  return red,ir;
//}
void setupTSL230(){
  pinMode(TSL230_s0, OUTPUT); 
  pinMode(TSL230_s1, OUTPUT);
  pinMode(TSL230_s2, OUTPUT);
  pinMode(TSL230_s3, OUTPUT);

  //configure sensitivity - Can set to
  //S1 LOW  | S0 HIGH: low
  //S1 HIGH | S0 LOW:  med *****
  //S1 HIGH | S0 HIGH: high
  digitalWrite(TSL230_s1, HIGH);
  digitalWrite(TSL230_s0, HIGH);
  
  //config Scaling:
  //S3  S2  SCALING (divide-by)
  //L   L   1
  //L   H   2
  //H   L   10**************default
  //H   H   100x
  digitalWrite(TSL230_s2, LOW);
  digitalWrite(TSL230_s3, HIGH);
 
}
float readTSL230(int samples){
//sample light, return reading in frequency
//higher number means brighter
  float start = micros();  
  int readings = 0;

  while(readings < samples){
   pulseIn(TSL230_Pin, HIGH); //starts timing - waits - pin=LOW -->stops timing; Returns the length of the pulse in microseconds (10ys to 3min) ===> alternative function COUNT()
   readings ++;
  }
  float length = micros() - start;//length is the time of the pulse width depending on incident intensity
  float freq = (1000000 / (length / samples)) * 10; //frequency calc by dividing 1*10^6 (factor ys in s) / time[ys] / sample-rate; *10 see Scaling in setupTSL
  return freq;
}
