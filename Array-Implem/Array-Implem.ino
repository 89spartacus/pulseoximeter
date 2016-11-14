//#include "U8glib.h"
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);

#include <Average.h>
#define BUFLEN 70//70 datapoints equal to 3,125s sample time with 45ms measuring times
Average<int> redBuf(BUFLEN);
Average<int> irBuf(BUFLEN);

#define redLED 1
#define irLED 0
int CrossBufCnt=0; int dTmBufCnt=0;
int CrossBuf[0]; int dTmBuf[0];
int red,ir,AVG,Rmin,Rmax,IRmin,IRmax,HR,SpO,R,rRed,rIR;
byte ledState;
void ledSwitch(bool ledState){return}
void readTSL(){}
void setup() {
  
    
}

void loop() {
  if(irBuf.getCount()<BUFLEN){
    //Red and IR measurements 
    //- readings into redBuf[redBufCnt] & IrBuf[IrBufCnt]
    if(ledState==redLED){
      delay(45);
      red=readTSL();
      redBuf.push(red);
      //redBufCnt=(redBufCnt + 1) % BUFLEN; //- increase XBufCnt every iteration //red can roll over
      ledSwitch(irLED);//or tto make this clearer: use redLED=1, irLED=0 and switch by using: ledSwitch(irLED) to switch to IR and vice versa
    }
    if(ledState==irLED){
      delay(45);
      ir=readTSL();
      irBuf.push(ir);
      //irBufCnt++;//- increase XBufCnt every iteration / IR has to be reset in else
      ledSwitch(redLED); 
    }
  }
  else{
    //do calculation of Max,Min,Avg on arrays
    Rmax=redBuf.maximum();
    Rmin=redBuf.minimum();
    IRmax=irBuf.maximum();
    IRmin=irBuf.minimum();
    AVG=irBuf.mean();
    //check AVG-Crossing/Peak deetection
    int i;
    for(i=0,i<BUFLEN,i++){
      if(irBuf.get(i)>AVG && irBuf.get(i+1)<AVG){
        CrossBuf[CrossBufCnt]=i;
        CrossBufCnt++;
      }
    }
    int j;
    for(j=0,j<CrossBufCnt,j++){
     dTmBuf[dTmBufCnt] = CrossBuf[j+1] - CrossBuf[j];      
    }
    dTmAVG=mean(dTmBuf,dTmBufCnt);
    //calculate HR via time difference between peak counts (arraycnt)
    HR=((1/ dTmAVG*45)*60000);
    //calc ratios & SpO2
    rRed=(float(Rmax-Rmin)/Rmin);//normalized Ratio of Red absorbance
    rIR=(float(IRmax-IRmin)/IRmin); //normalized Ratio of IR absorbance
    R=(rRed/rIR); //Ratio of ratios
    SpO=108-20*R-0.375*R*R; //SpO2 calculation currently based on literature
    //Print data to LCD
        //still needs to be done!!!!
    //reset the BufferCounters to roll over
    irBuf.clear;CrossBufCnt=0;dTmBufCnt=0;
    
  }
}
