#include <SPI.h>
#include <ILI9341_t3.h>
#include <font_Nunito.h>
#include <font_NunitoBold.h>
#include <font_NunitoLight.h>

#define TFT_DC      20
#define TFT_CS      15
#define TFT_RST    255  // 255 = unused, connect to 3.3V
#define TFT_MOSI     11
#define TFT_SCLK    14
#define TFT_MISO    12
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);


void setup() {
  pinMode(25,OUTPUT);
  pinMode(31,INPUT);
  digitalWrite(25,255);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  //tft.setTextSize(2);
  //tft.println("Waiting for Arduino Serial Monitor...");

  Serial.begin(9600);
  //while (!Serial) ; // wait for Arduino Serial Monitor
  //tft.fillScreen(ILI9341_BLACK);
  Serial.println("ILI9341 Test!"); 

  int xx = 0;
  tft.setFont(Nunito_40);
  tft.setCursor(xx, 4);
  tft.print("Nunito");
  tft.setCursor(xx, 74);
  tft.setFont(Nunito_40_Bold);
  tft.print("Nunito Bold");
  tft.setCursor(xx, 144);
  tft.setFont(Nunito_40_Light);
  tft.print("Nunito Light");
  tft.setCursor(xx, 190);
  tft.setFont(Nunito_10_Light);
  tft.print("Nunito Light");
   tft.setCursor(xx, 210);
  tft.setFontAdafruit();
  tft.setTextSize(2);
  tft.print("Nunito Light");
}

void loop(void) {
  int a = (analogRead(31)-100)/4;
  if (a<0)
    a = 0;
  a+=25;
  analogWrite(25,a);
  Serial.println(a);
  delay(100);
  //analogWrite(25,200);
}

