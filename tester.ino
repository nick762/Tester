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

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI display(U8G2_R0,OLED_CLK,OLED_MOSI,OLED_CS,OLED_DC,OLED_RESET);

SBS stnd;
MAX17215 wire215;
MAX17320 wire320;
DS2782 _ds;

int vol, cap, temp, cur, c1v, c2v, c3v, c4v, capMah;
byte type = 0;
const float R1 = 100000.0;
const float R2 = 10000.0;
const float uRef = 3.3;
byte bTimer = 0;

bool state = false;

float vRead(){
  float vRef = 0.0;
  float inVol = 0.0;
  vRef = (analogRead(vIn)*uRef)/1024.0;
  inVol = vRef/(R2/(R1+R2));
  float *vState = &inVol;
  return *vState;
}

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
  //Serial.begin(115200);
  display.begin();
  display.enableUTF8Print();
  pinMode(vIn,INPUT);
  pinMode(btn,INPUT_PULLUP);
  digitalWrite(btn,HIGH);
  attachInterrupt(digitalPinToInterrupt(btn), res, RISING);
  vRead();
  type = scan();
  sel();
}

void loop() {
  ///Serial.println("is ok");
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
    Serial.println("press");
    wire215._fReset();
  }
}

void sel(){
  display.clearBuffer();
  display.setFont(u8g2_font_10x20_t_cyrillic);
  switch (type)
  {
  case 1:
    display.firstPage();
    do{
      display.setCursor(48,14);
      display.print("Тип");
      display.setCursor(8,32);
      display.print("контроллера:");
      display.setCursor(26,54);
      display.print("BQ20Z655");
      display.sendBuffer();
    }while (display.nextPage());
    delay(1500);
    break;
  case 2:
  display.firstPage();
    do{
      display.setCursor(48,16);
      display.print("Тип");
      display.setCursor(8,32);
      display.print("контроллера:");
      display.setCursor(36,54);
      display.print("DS2782");
      display.sendBuffer();
    }while (display.nextPage());
    delay(1500);
    break;
  case 3:
  display.firstPage();
    do{
      display.setCursor(48,14);
      display.print("Тип");
      display.setCursor(8,32);
      display.print("контроллера:");
      display.setCursor(26,54);
      display.print("MAX17215");
      display.sendBuffer();
    }while (display.nextPage());
    delay(1500);
    break;
  case 4:
  display.firstPage();
    do{
      display.setCursor(48,14);
      display.print("Тип");
      display.setCursor(8,32);
      display.print("контроллера:");
      display.setCursor(26,54);
      display.print("MAX17320");
      display.sendBuffer();
    }while (display.nextPage());
    delay(1500);
    break;
  case 0: 
    display.firstPage();
    do{
      display.setCursor(16,22);
      display.print("Контроллер");
      display.setCursor(56,44);
      display.print("не");
      display.setCursor(26,62);
      display.print("отвечает");
      display.sendBuffer();
    }while (display.nextPage());
    delay(3000);
    display.firstPage();
    do{
      display.setCursor(16,22);
      display.print("Измеренное");
      display.setCursor(12,40);
      display.print("напряжение:");
      display.setCursor(48,62);
      display.print(vRead());
      display.sendBuffer();
    }while (display.nextPage());
    delay(3500);
    break;
  }
}

void screenOne(){  
  display.clearBuffer(); 
  display.setFont(u8g2_font_6x12_t_cyrillic);
  switch (type)
  {
  case 1:
  display.firstPage();
  do{
    display.setCursor(0,11);
    //display.print((class __FlashStringHelper *)Cap);
    display.print("Емкость:");
    display.setCursor(80,11);
    display.print(stnd.GetCap());
    display.setCursor(114,11);
    display.print("%");
    display.setCursor(0,28);
    display.print("Напряжение:");
    //display.print((class __FlashStringHelper *)a[1]);
    display.setCursor(80,28);
    display.print(stnd.GetVol());
    display.setCursor(114,28);
    display.print("мВ");
    display.setCursor(0,45);
    //display.print((class __FlashStringHelper *)a[2]);
    display.print("Потребление:");
    display.setCursor(80,45);
    display.print(stnd.GetCur());
    display.setCursor(114,45);
    display.print("мА");
    display.setCursor(0,62);
    //display.print((class __FlashStringHelper *)a[3]);
    display.print("Температура:");
    display.setCursor(80,62);
    display.print(stnd.GetTemp());
    display.setCursor(114,62);
    display.print("C");
    display.sendBuffer();
    }while (display.nextPage());
    break;
  case 3:
    display.firstPage();
    do{
      display.setCursor(0,11);
      display.print("Емкость:");
      display.setCursor(80,11);
      display.print(wire215._readCapacity());
      display.setCursor(114,11);
      display.print("%");
      display.setCursor(0,28);
      display.print("Напряжение:");
      display.setCursor(80,28);
      display.print(wire215._readVoltage());
      display.setCursor(114,28);
      display.print("мВ");
      display.setCursor(0,45);
      display.print("Потребление:");
      display.setCursor(80,45);
      display.print(wire215._readCurrent());
      display.setCursor(114,45);
      display.print("мА");
      display.setCursor(0,62);
      display.print("Температура:");
      display.setCursor(80,62);
      display.print(wire215._readTemperature());
      display.setCursor(114,62);
      display.print("C");
      display.sendBuffer();
    }while (display.nextPage());
  break;
  case 2:    
    display.firstPage();
    do{
      display.setCursor(0,11);
      display.print("Емкость:");
      display.setCursor(80,11);
      display.print(_ds._readCapacity());
      display.setCursor(114,11);
      display.print("%");
      display.setCursor(0,28);
      display.print("Напряжение:");
      display.setCursor(80,28);
      display.print(_ds._readVoltage());
      display.setCursor(114,28);
      display.print("мВ");
      display.setCursor(0,45);
      display.print("Потребление:");
      display.setCursor(80,45);
      display.print(_ds._readCurrent());
      display.setCursor(114,45);
      display.print("мА");
      display.setCursor(0,62);
      display.print("Температура:");
      display.setCursor(80,62);
      display.print(_ds._readTemperature());
      display.setCursor(114,62);
      display.print("C");
      display.sendBuffer();
      }while (display.nextPage());
  break;
  case 4:
    display.firstPage();
    do{
      display.setCursor(0,11);
      display.print("Емкость:");
      display.setCursor(80,11);
      display.print(wire320._readCapacity());
      display.setCursor(114,11);
      display.print("%");
      display.setCursor(0,28);
      display.print("Напряжение:");
      display.setCursor(80,28);
      display.print(wire320._readVoltage());
      display.setCursor(114,28);
      display.print("мВ");
      display.setCursor(0,45);
      display.print("Потребление:");
      display.setCursor(80,45);
      display.print(wire320._readCurrent());
      display.setCursor(114,45);
      display.print("мА");
      display.setCursor(0,62);
      display.print("Температура:");
      display.setCursor(80,62);
      display.print(wire320._readTemperature());
      display.setCursor(114,62);
      display.print("C");
      display.sendBuffer();
    }while (display.nextPage());
  break;
  default:
  break;  
  }
}

void screenTwo(){
  display.clearBuffer(); 
  display.setFont(u8g2_font_6x12_t_cyrillic); 
  switch (type)
  {
  case 1:
    display.firstPage();
    do{
      display.setCursor(0,11);
      display.print("Ячейка 1:");
      display.setCursor(80,11);
      display.print(stnd.GetC1());
      display.setCursor(114,11);
      display.print("мВ");
      display.setCursor(0,28);
      display.print("Ячейка 2:");
      display.setCursor(80,28);
      display.print(stnd.GetC2());
      display.setCursor(114,28);
      display.print("мВ");
      display.setCursor(0,45);
      display.print("Ячейка 3:");
      display.setCursor(80,45);
      if(stnd.readCellsNum() == 2){
        display.print("0");
        display.setCursor(114,45);
        display.print("мВ");
        display.setCursor(0,62);
        display.print("Ячейка 4:");
        display.setCursor(80,62);
        display.print("0");
        display.setCursor(114,62);
        display.print("мВ");
        display.sendBuffer();
      }else if(stnd.readCellsNum() == 3){
        display.print(stnd.GetC3());
        display.setCursor(114,45);
        display.print("мВ");
        display.setCursor(0,62);
        display.print("Ячейка 4:");
        display.setCursor(80,62);
        display.print("0");
        display.setCursor(114,62);
        display.print("мВ");
        display.sendBuffer();
      }else if(stnd.readCellsNum() == 4){
        display.print(stnd.GetC3());
        display.setCursor(114,45);
        display.print("мВ");
        display.setCursor(0,62);
        display.print("Ячейка 4:");
        display.setCursor(80,62);
        display.print(stnd.GetC4());
        display.setCursor(114,62);
        display.print("мВ");
        display.sendBuffer();
      }
    }while (display.nextPage());
    break;
  case 3:
    display.firstPage();
    do{
      display.setCursor(0,11);
      display.print("Ячейка 1:");
      display.setCursor(80,11);
      display.print(wire215._readCell1());
      display.setCursor(114,11);
      display.print("мВ");
      display.setCursor(0,28);
      display.print("Ячейка 2:");
      display.setCursor(80,28);
      display.print(wire215._readCell2());
      display.setCursor(114,28);
      display.print("мВ");
      display.setCursor(0,45);
      display.print("Ячейка 3:");
      display.setCursor(80,45);
      display.print(wire215._readCell3());
      display.setCursor(114,45);
      display.print("мВ");
      display.setCursor(0,62);
      display.print("Ячейка 4:");
      display.setCursor(80,62);
      display.print(wire215._readCell4());
      display.setCursor(114,62);
      display.print("мВ");
      display.sendBuffer();
    }while (display.nextPage());
    break;
  case 2:
    display.clearBuffer();
    display.setFont(u8g2_font_10x20_t_cyrillic);
    display.firstPage();
    do{
      display.setCursor(7,20);
      display.print("Информация о");
      display.setCursor(27,34);
      display.print("ячейках");
      display.setCursor(13,48);
      display.print("недоступна");
      display.sendBuffer();
    }while (display.nextPage());
    break;
  case 4:
    display.firstPage();
    do{
      display.setCursor(0,11);
      display.print("Ячейка 1:");
      display.setCursor(80,11);
      display.print(wire320._readCell1());
      display.setCursor(114,11);
      display.print("мВ");
      display.setCursor(0,28);
      display.print("Ячейка 2:");
      display.setCursor(80,28);
      display.print(wire320._readCell2());
      display.setCursor(114,28);
      display.print("мВ");
      display.setCursor(0,45);
      display.print("Ячейка 3:");
      display.setCursor(80,45);
      display.print(wire320._readCell3());
      display.setCursor(114,45);
      display.print("мВ");
      display.setCursor(0,62);
      display.print("Ячейка 4:");
      display.setCursor(80,62);
      display.print(wire320._readCell4());
      display.setCursor(114,62);
      display.print("мВ");
      display.sendBuffer();
    }while (display.nextPage());
    break;  
  default:
  break;
  }
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
    display.print("Измеренное");
    display.setCursor(12,40);
    display.print("напряжение:");
    display.setCursor(48,62);
    display.print(val);
    //display.print(234235);
    display.sendBuffer();
  }while (display.nextPage());
}
