#include "MCUFRIEND_kbv.h"
MCUFRIEND_kbv tft;

#include "bitmap_RGB.h"

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x8410
#define ORANGE  0xE880

#define Motor1Speed 45
#define Motor1Direction 22
#define Motor2Speed 46
#define Motor2Direction 23

void setup()
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();

    Serial.print(F("ID = 0x"));
    Serial.println(ID, HEX);

    pinMode(Motor1Speed, OUTPUT);
    pinMode(Motor1Direction, OUTPUT);
    pinMode(Motor2Speed, OUTPUT);
    pinMode(Motor2Direction, OUTPUT);
    
    tft.begin(ID);
    tft.setRotation(1);
        tft.fillScreen(BLACK);
    int x = 0; int x2 = 416;
    int y = 5; int y2 = 5;
    tft.setAddrWindow(x, y, x + 64 - 1, y + 64 - 1);
    tft.pushColors((const uint8_t*)ctt_64x64, 64 * 64, 1, false);
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1);
    tft.setAddrWindow(x2, y2, x2 + 64 - 1, y2 + 64 - 1);
    tft.pushColors((const uint8_t*)ctt_64x64, 64 * 64, 1, false);
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1);
    tft.setTextColor(BLUE);
    tft.setTextSize(4);
    tft.setCursor(75,5);
    tft.print("FollowMeCooler");
    //    invertmap(betty_1_bmp + 62, 8960);
    //    while (1);
}



void loop(void)
{
    tft.setCursor(0,0);
    digitalWrite(Motor1Direction, LOW);
    digitalWrite(Motor2Direction, HIGH);
    for (int pwmwert=0; pwmwert<=255; pwmwert++){
      analogWrite(Motor1Speed, pwmwert);
      analogWrite(Motor2Speed, pwmwert);
      delay(5);
    }
    for (int pwmwert=255; pwmwert>=0; pwmwert--){
      analogWrite(Motor1Speed, pwmwert);
      analogWrite(Motor2Speed, pwmwert);
      delay(5);
    }
    digitalWrite(Motor1Direction, HIGH);
    digitalWrite(Motor2Direction, LOW);
    for (int pwmwert=0; pwmwert<=255; pwmwert++){
      analogWrite(Motor1Speed, pwmwert);
      analogWrite(Motor2Speed, pwmwert);
      delay(5);
    }
    for (int pwmwert=255; pwmwert>=0; pwmwert--){
      analogWrite(Motor1Speed, pwmwert);
      analogWrite(Motor2Speed, pwmwert);
      delay(5);
    }
    //delay(5000);
}
