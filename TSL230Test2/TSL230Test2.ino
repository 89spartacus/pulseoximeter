// Article:  http://bildr.org/2011/08/tsl230r-arduino/ 

int redLED = 8; 
int irLED = 9;
int TSL230_Pin = 2; //TSL230 output
int TSL230_s0 = 3; //TSL230 sensitivity setting 1
int TSL230_s1 = 4; //TSL230 sensitivity setting 2
int TSL230_s2 = 5; //divide-by setting
int TSL230_s3 = 6; //divide-by setting
int TSL230_samples = 4; //higher = slower but more stable and accurate

void setup(){
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(irLED, OUTPUT);
  digitalWrite(redLED, HIGH);
  digitalWrite(irLED, LOW);
  setupTSL230();
}
void loop(){
  
  float red = readTSL230(TSL230_samples);
  Serial.print(red);
  delay(4);
  digitalWrite(irLED, HIGH);digitalWrite(redLED, LOW);
  float ir = readTSL230(TSL230_samples);
  Serial.print(" ");Serial.println(ir);
  digitalWrite(irLED, LOW);digitalWrite(redLED, HIGH);
//threshold at lightlevel<10000

}




void setupTSL230(){
  pinMode(TSL230_s0, OUTPUT); 
  pinMode(TSL230_s1, OUTPUT);
  pinMode(TSL230_s2, OUTPUT);
  pinMode(TSL230_s3, OUTPUT);

  //configure sensitivity - Can set to
  //S1 LOW  | S0 HIGH: low
  //S1 HIGH | S0 LOW:  med
  //S1 HIGH | S0 HIGH: high
  digitalWrite(TSL230_s1, HIGH);
  digitalWrite(TSL230_s0, LOW);
  
  //config Scaling:
  //S3  S2  SCALING (divide-by)
  //L   L   1
  //L   H   2
  //H   L   10*default
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
   pulseIn(TSL230_Pin, HIGH);
   readings ++;
  }

  float length = micros() - start;
  float freq = (1000000 / (length / samples)) * 10;

  return freq;
}

