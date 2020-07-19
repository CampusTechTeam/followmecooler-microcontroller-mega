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

int Motor1 = 0;
int Motor2 = 0;
boolean Motor1Richtung = false;
boolean Motor2Richtung = false;
//False = Vorwärts
void setup()
{
  Serial.begin(115200);
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
  tft.setCursor(75, 5);
  tft.print("FollowMeCooler");

  tft.setTextSize(3);
  tft.setCursor(0, 100);
  tft.setTextColor(CYAN, BLACK);
  tft.print("Motor 1:");

  tft.setTextSize(3);
  tft.setCursor(0, 150);
  tft.setTextColor(CYAN, BLACK);
  tft.print("Motor 2:");
}



void loop(void)
{
  digitalWrite(Motor1Direction, LOW);
  digitalWrite(Motor2Direction, HIGH);
  Motor1Richtung = false;
  Motor2Richtung = true;
  handleSerial();

  //tft.fillRect(0,65,320,480,BLACK);

  tft.setTextSize(2);
  tft.setTextColor(MAGENTA, BLACK);
  tft.setCursor(0, 130);
  tft.print("- ");
  tft.print(Motor1 / 2.55);
  tft.println("%  ");
  tft.setCursor(0, 180);
  tft.print("- ");
  tft.print(Motor2 / 2.55);
  tft.println("%  ");


}

void handleSerial() {
  while (Serial.available() >= 2) {
    switch (Serial.read()) {
      case 'W':
        Serial.println("Sent: W");
        switch (Serial.read()) {
          case 'M':
           Serial.println("Sent: WM");
           switch (Serial.read()) {
            case '1':
            Serial.println("Sent: WM1");
            Motor1 = (Serial.readString()).toInt();
            analogWrite(Motor1Speed, Motor1);
            break;

            case '2':
            Serial.println("Sent: WM2");
            Motor2 = (Serial.readString()).toInt();
            analogWrite(Motor2Speed, Motor2);
            break;
           }
          break;
        }
        break;

      case 'R':
        Serial.println("Sent: R");

        break;
    }
  }
}
