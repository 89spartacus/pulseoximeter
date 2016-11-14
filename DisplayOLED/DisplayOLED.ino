
#include "U8glib.h"
int valueHR, valueO2;
boolean reDraw=true;
#define logo_width 128
#define logo_height 56
static unsigned char heartlogo[] U8G_PROGMEM = { };


//driver constructor with function u8g. ...
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 


void draw_header(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  u8g.setFontPosTop();
  u8g.drawStr( 0, 0, "HR[bpm]");
  int w=u8g.getStrWidth("SpO2[%]");
  u8g.drawStr(128-w,0,"SpO2[%]");
  u8g.drawHLine(0,15,128);
}
void draw_values(){
  u8g.setFont(u8g_font_fur30);
  u8g.setFontPosBottom();
  u8g.setPrintPos(0,64);
  u8g.print(valueHR);
  u8g.setPrintPos(85,64);
  u8g.print(valueO2);

}

void setup(void) {
    u8g.setColorIndex(1);         // pixel on
    if(reDraw){
      u8g.firstPage();
      while(u8g.nextPage()){
        u8g.drawXBMP( 0, 8, logo_width, logo_height, heartlogo);
    }}
    reDraw=false;
    delay(2000);
}

void loop(void) {
  valueHR=random(40,60);
  valueO2=random(95,100);

  
  
  // picture loop
  u8g.firstPage();  
  do {
    draw_header();
    draw_values();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(1000);
}

