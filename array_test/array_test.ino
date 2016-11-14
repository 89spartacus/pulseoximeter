#define RED 1
#define IR 0


#define THRESHOLD//Offset um die Aortenklappenspitzen abzuschneiden

#define BUFLEN 3000    //Sollten so ca. 3 Sekunden sein

volatile uint16_t rBufctr = 0;    //Counter für den Rot-Werte-Buffer

volatile uint16_t rBuf[BUFLEN]
volatile uint16_t irCufctr = 0;  //Counter für den IR-WErte-Buffer

volatile uint16_t irBuf[BUFLEN]


setup(){

//Buffer inititalisieren, solltest du machen, kannst aber auch später oder anders abfangen...
for(int i = 0; i<BUFLEN; i++){

rBuf[i] = 0;

irBuf[i] = 0;
}
}



if(ledState==RED){

rBuf[rBufctr] = readTSL()//save current reading in red array (rBuf) @ cntr-position (rBufCntr)

rBufctr = (rBufctr+1) % BUFLEN 
//modulo operator results in remainder from division and rolls over variable - 0+1/buflen[=70]=1;...;68+1/buflen=69;69+1/buflen= 0 ;...
//rolling over the array and start saving readings at the beginning of the array again.
}
if(ledState==IR){

irBuf[irBufctr] = readTSL()//oder wie auch immer

irBufctr = (irBufctr+1)% BUFLEN

}


}


void loop(){

int iMax1;    //index des Maximums eines Herzschlags

int iMax2;    //index des Maximums eines Herzschlags

int iMin;    //index des Minimums eines Herzschlags

iMax1 = getMax(&rBuf,rBufctr,BUFLEN);

iMax2 = getMax(&rBuf,rBufctr,BUFLEN-100);

deltaT = (iMax2-iMax1)*100    //bei 100ms Samplerate oder sowas

iMin = getMin(&rBuf,BUFLEN);

}

int getMax(uint16_t* buffer, uint16_t buf_ptr uint16_t length, int average,int* maxList, int maxListLength){

//sucht den Index vom letzten Peak im Buffer, kann man ja auch zwei Werte speichern und den zweiten dann Kicken, wenn's zu sehr an den Rand geht
uint16_t retVal = 0;
int j=0
for(int i = buf_ptr; i!=buf_ptr-1; i++){

if(buffer[i]>buffer[-1]&&buffer[i]>average){

    maxList[j] = i

}



i %= length;

return retVal
}}

