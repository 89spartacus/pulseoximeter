#include <Average.h>
#define BUFLEN 70
Average<int> redBuf(BUFLEN);
Average<int> irBuf(BUFLEN);

void setup() {

}

void loop() {
  redBuf.push(entry);//add "entry" to array
  irBuf.push();
  irBuf.getCount();//current count
  irBuf.mean();//calc mean/average of array
  irBuf.maximum();
  irBuf.minimum();
  irBuf.get(CNT);//get data at CNT position
  //irBuf.sum();
  irBuf.clear();
}
