
#include <TimerOne.h> //interrupts
#define analogPin A0
#define Theta 0.6
//#define redLED 11 //High is Red led on, Low is Infrared on.
//#define iredLED 12

volatile int maxTemp,minTemp; //shared variables between interrupts and loop 
volatile int lastcount,count;
int Rmax, Rmin, IRmax, IRmin;
float R, Spo2,HeartR_frq;
int Sp02_int;
int redLED = 11, iredLED = 12;
int HeartRate, HeartR;
float LastHeartRate = 60; //default
int average = 140; //average is average crossing //int average2 = 220;
int interrupts_counter =0; //count the times of interrupts float value=0.5;
float Spo2_float;


void setup() {
pinMode(redLED, OUTPUT); //define LED
pinMode(iredLED, OUTPUT);
pinMode(analogPin, INPUT); //analog signal input
//initially switch on Red LED, after each interrupt will turn on the other
digitalWrite(redLED, HIGH);
Serial.begin(9600);

init_interrupts();
Timer1.initialize(40000); //terrupt every 0.04 seconds
Timer1.attachInterrupt(max_min_num); //interrupt call max_min_num function
Rmax = 0;
IRmax = 0;
Rmin = 0;
IRmin = 0;
LastHeartRate = 60;
}
void max_min_num() {
lastcount = count;
count = analogRead(analogPin); //read signa
if(count> maxTemp){
maxTemp = count;
}
else if(count<minTemp){ minTemp = count;
}
interrupts_counter++; //interrupt counter
}
void init_interrupts() {
maxTemp = 0;
minTemp = 1023; count = 0;
lastcount =0;
interrupts_counter = 0;
}
void loop(){ 
  while(1){ //the whole while is used to avoid LCD reinitialize 
    digitalWrite(redLED,HIGH); 
    delay(2000); //let red led signal to be stable 
//interrupts(); 
  while(!((lastcount>average )&& (count<average)) ){ }
    digitalWrite(redLED,HIGH);
    init_interrupts();
  while(!((lastcount>average )&& (count<average)) ){ }
    noInterrupts(); // temporarily disabel interrupts, to be sure it will not change while we are reading 
    Rmax = maxTemp; 
    Rmin = minTemp; 
    delay(100); 
    HeartR_frq = 1/(0.04*interrupts_counter); //d is the times of ISR in 1 second, 
    interrupts(); //enable interrupts
    HeartRate = HeartR_frq * 60;
  if(HeartRate> 60 && HeartRate< 120){
    HeartR = Theta*HeartRate + (1 - Theta)*LastHeartRate; //Use theta to smooth 
    LastHeartRate = HeartR;
    }
digitalWrite(redLED, LOW);

R = (Rmax - Rmin);
Spo2 = (R-180)*0.01 +97.838;
//int Spo2_int = (int)Spo2; //float Spo2 to int Spo2_int
//String Spo2_float = floatToString(buffer,Spo2,2); 
Serial.print(analogPin);
String HRSPO = HeartR+String(" HR ")+Spo2+String(" SpO2%"); 
Serial.println(HRSPO);

delay(1000);
init_interrupts();
}
}

