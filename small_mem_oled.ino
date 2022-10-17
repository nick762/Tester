#include <SBS.h>
#include <ds2782.h>
#include <max17215.h>
#include <max17320.h>
#include <U8g2lib.h>
//#include <U8x8lib.h>

#define OLED_MOSI   10//9//d1 SDA
#define OLED_CLK   9//10//d0 SCL
#define OLED_DC    8//5//11
#define OLED_CS    6//12
#define OLED_RESET 13

#define vIn A2
#define btn 2

U8G2_SSD1306_128X64_NONAME_2_4W_SW_SPI display(U8G2_R0,OLED_CLK,OLED_MOSI,OLED_CS,OLED_DC,OLED_RESET);

SBS stnd;
MAX17215 wire215;
MAX17320 wire320;
DS2782 _ds;

byte type = 0;
byte nCells = 0;
const float PROGMEM R1 = 100000.0;
const float PROGMEM R2 = 10000.0;
const float uRef = 3.3;
byte bTimer = 0;

void(* resetFunc) (void) = 0;

float vRead(){
  float vRef = 0.0;
  float inVol = 0.0;
  vRef = (analogRead(vIn)*uRef)/1024.0;
  inVol = vRef/(R2/(R1+R2));
  float *vState = &inVol;
  return *vState;
}

const char s1[] PROGMEM ="Напряжение";
const char s2[] PROGMEM ="Емкость";
const char s3[] PROGMEM ="Потребление";
const char s4[] PROGMEM ="Температура";

const char c1[] PROGMEM ="Ячейка 1";
const char c2[] PROGMEM ="Ячейка 2";
const char c3[] PROGMEM ="Ячейка 3";
const char c4[] PROGMEM ="Ячейка 4";

const char e1[] PROGMEM ="%";
const char e2[] PROGMEM ="мВ";
const char e3[] PROGMEM ="мА";
const char e4[] PROGMEM ="С";

const char t1[] PROGMEM = "BQ20Z655";
const char t2[] PROGMEM = "DS2782";
const char t3[] PROGMEM = "MAX17215";
const char t4[] PROGMEM = "MAX17230";

byte scan(){
  if(stnd.scan() == 11){
    type = 1;
    return type; //bq20z655
  }else if(stnd.scan() == 52) {
    type = 2;
    return type; //ds2782
  }else if(wire215._scan() != 0) {
    if(wire215._readType() == 0x50){
      type = 3;
      return type; //max17215
    }else if(wire215._readType() == 0x90){
      type = 4;
      return type; //max17320
    }
  }else{
    return 0; 
  }
}

void setup() {
  TWBR=144;
  analogReference(DEFAULT);
  display.begin();
  display.enableUTF8Print();
  pinMode(vIn,INPUT);
  pinMode(btn,INPUT_PULLUP);
  digitalWrite(btn,HIGH);
  attachInterrupt(digitalPinToInterrupt(btn), res, RISING);
  vRead();
  type = scan();
  if(type == 1){
    nCells = stnd.readCellsNum();
  }else if(type == 3){
    nCells = wire215._cellNum();
  }else if(type == 4){
    nCells = wire320._cellNum();
  }
  sel();
}

void loop() {
  screenOne();
  delay(7000);
  screenTwo();
  delay(4000);
  screenThree();
  delay(3000);
}

void res(){
  if(bTimer==0){
    bTimer++;
  }else{
    if(type == 3){
      wire215._fReset(); 
    }else if(type == 4){
      wire320._fReset(); 
    }
  }
}

void sel(){
  //display.clearBuffer();
  display.setFont(u8g2_font_10x20_t_cyrillic);
  display.firstPage();
  do{
    display.setCursor(48,14);
    display.print(F("Тип"));
    display.setCursor(8,32);
    display.print(F("контроллера:"));
    switch (type)
    {
      case 1:
        display.setCursor(26,54);
        display.print((class __FlashStringHelper *)t1);
      break;
      case 2:
        display.setCursor(36,54);
        display.print((class __FlashStringHelper *)t2);
      break;
      case 3:
        display.setCursor(26,54);
        display.print((class __FlashStringHelper *)t3);
      break;
      case 4:
        display.setCursor(26,54);
        display.print((class __FlashStringHelper *)t4);
      break;
      case 0: 
        do{
          display.clearBuffer();
          display.setCursor(16,22);
          display.print(F("Контроллер"));
          display.setCursor(56,44);
          display.print(F("не"));
          display.setCursor(26,62);
          display.print(F("отвечает"));
          display.sendBuffer();
        }while (display.nextPage());
        delay(3000);
        screenThree();
        delay(3500);
        resetFunc();
      break;
    }
    display.sendBuffer();
  }while (display.nextPage());
  delay(1500);
}

void screenOne(){  
  //display.clearBuffer(); 
  display.setFont(u8g2_font_6x12_t_cyrillic);
  display.firstPage();
  do{
    display.setCursor(0,11);
    display.print((class __FlashStringHelper *)s2);
    display.setCursor(114,11);
    display.print((class __FlashStringHelper *)e1);
    display.setCursor(0,28);
    display.print((class __FlashStringHelper *)s1);
    display.setCursor(114,28);
    display.print((class __FlashStringHelper *)e2);
    display.setCursor(0,45);
    display.print((class __FlashStringHelper *)s3);
    display.setCursor(114,45);
    display.print((class __FlashStringHelper *)e3);
    display.setCursor(0,62);
    display.print((class __FlashStringHelper *)s4);
    display.setCursor(114,62);
    display.print((class __FlashStringHelper *)e4);
    switch (type)
    {
      case 1:
        display.setCursor(80,11);
        display.print(stnd.GetCap());
        display.setCursor(80,28);
        display.print(stnd.GetVol());
        display.setCursor(80,45);
        display.print(stnd.GetCur());
        display.setCursor(80,62);
        display.print(stnd.GetTemp());
      break;
      case 2:
        display.setCursor(80,11);
        display.print(_ds._readCapacity());
        display.setCursor(80,28);
        display.print(_ds._readVoltage());
        display.setCursor(80,45);
        display.print(_ds._readCurrent());
        display.setCursor(80,62);
        display.print(_ds._readTemperature());
      break;
      case 3:
        display.setCursor(80,11);
        display.print(wire215._readCapacity());
        display.setCursor(80,28);
        display.print(wire215._readVoltage());
        display.setCursor(80,45);
        display.print(wire215._readCurrent());
        display.setCursor(80,62);
        display.print(wire215._readTemperature());
      break;
      case 4:
        display.setCursor(80,11);
        display.print(wire320._readCapacity());
        display.setCursor(80,28);
        display.print(wire320._readVoltage());
        display.setCursor(80,45);
        display.print(wire320._readCurrent());
        display.setCursor(80,62);
        display.print(wire320._readTemperature());
      break;
      default:
      break;  
    }
    display.sendBuffer();
    }while(display.nextPage());
}

void screenTwo(){
  //display.clearBuffer(); 
  display.setFont(u8g2_font_6x12_t_cyrillic); 
  display.firstPage();
  do{
    display.setCursor(0,11);
    display.print((class __FlashStringHelper *)c1);
    display.setCursor(114,11);
    display.print((class __FlashStringHelper *)e2);
    display.setCursor(0,28);
    display.print((class __FlashStringHelper *)c2);
    display.setCursor(114,28);
    display.print((class __FlashStringHelper *)e2);
    display.setCursor(0,45);
    display.print((class __FlashStringHelper *)c3);
    display.setCursor(114,45);
    display.print((class __FlashStringHelper *)e2);
    display.setCursor(0,62);
    display.print((class __FlashStringHelper *)c4);
    display.setCursor(114,62);
    display.print((class __FlashStringHelper *)e2);
    switch (type)
    {
      case 1:
        display.setCursor(80,11);
        display.print(stnd.GetC1());
        display.setCursor(80,28);
        display.print(stnd.GetC2());
        display.setCursor(80,45);
        if(nCells == 2){
          display.print("0");
          display.setCursor(80,62);
          display.print("0");
        }else if(nCells == 3){
          display.print(stnd.GetC3());
          display.setCursor(80,62);
          display.print("0");
        }else if(nCells == 4){
          display.print(stnd.GetC3());          
          display.setCursor(80,62);
          display.print(stnd.GetC4());
        }
      break;
      case 2:
        do{
          display.clearBuffer();
          display.setFont(u8g2_font_10x20_t_cyrillic);
          display.setCursor(7,20);
          display.print(F("Информация о"));
          display.setCursor(27,34);
          display.print(F("ячейках"));
          display.setCursor(13,48);
          display.print(F("недоступна"));
          display.sendBuffer();
        }while (display.nextPage());
      break;
      case 3:
        display.setCursor(80,11);
        display.print(wire215._readCell1());
        display.setCursor(80,28);
        display.print(wire215._readCell2());
        display.setCursor(80,45);
        if(nCells == 2){
          display.print("0");
          display.setCursor(80,62);
          display.print("0");
        }else if(nCells == 3){
          display.print(wire215._readCell3());
          display.setCursor(80,62);
          display.print("0");
        }else if(nCells == 4){
          display.print(wire215._readCell3());          
          display.setCursor(80,62);
          display.print(wire215._readCell4());
        }
      break;  
      case 4:
        display.setCursor(80,11);
        display.print(wire320._readCell1());
        display.setCursor(80,28);
        display.print(wire320._readCell1());
        display.setCursor(80,45);
        if(nCells == 2){
          display.print("0");
          display.setCursor(80,62);
          display.print("0");
        }else if(nCells == 3){
          display.print(wire320._readCell3());
          display.setCursor(80,62);
          display.print("0");
        }else if(nCells == 4){
          display.print(wire320._readCell3());          
          display.setCursor(80,62);
          display.print(wire320._readCell4());
        }
      break;  
      default:
      break;
    }
    display.sendBuffer();
  }while(display.nextPage());
}

void screenThree(){
  char val[8];
  dtostrf(vRead(), 5, 2, val); 
  delay(200);
  display.clearBuffer();
  display.setFont(u8g2_font_10x20_t_cyrillic);
  display.firstPage();
  do{
    display.setCursor(16,22);
    display.print(F("Измеренное"));
    display.setCursor(12,40);
    display.print(F("напряжение:"));
    display.setCursor(48,62);
    display.print(val);
    display.sendBuffer();
  }while (display.nextPage());
}
