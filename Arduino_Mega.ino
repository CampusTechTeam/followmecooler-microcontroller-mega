#include "MCUFRIEND_kbv.h"
MCUFRIEND_kbv tft;

#include "cttlogo.h"
#include "fmclogo.h"

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
#define Motor1Direction 23
#define Motor2Speed 46
#define Motor2Direction 22

int Motor1 = 0;
int Motor1vh = -1;
int Motor2 = 0;
int Motor2vh = -1;
boolean Motor1Richtungvh = true;
boolean Motor2Richtungvh = true;
boolean Motor1Richtung = false;
boolean Motor2Richtung = false;
boolean updatestate = true;
//False = Vorwärts
void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(1);
  uint16_t ID = tft.readID();

  Serial.print(F("ID = 0x"));
  Serial.println(ID, HEX);

  pinMode(Motor1Speed, OUTPUT);
  pinMode(Motor1Direction, OUTPUT);
  pinMode(Motor2Speed, OUTPUT);
  pinMode(Motor2Direction, OUTPUT);
  pinMode(37, INPUT);
  pinMode(35, OUTPUT);
  pinMode(31, INPUT);
  pinMode(33, OUTPUT);

  tft.begin(ID);
  tft.fillScreen(BLACK);
  int x = 0; int x2 = 256; int x3 = 128;
  int y = 5; int y2 = 5; int y3 = 0;
  tft.setAddrWindow(x, y, x + 64 - 1, y + 64 - 1);
  tft.pushColors((const uint8_t*)cttlogo_64x64, 64 * 64, 1, false);
  tft.setAddrWindow(x2, y2, x2 + 64 - 1, y2 + 64 - 1);
  tft.pushColors((const uint8_t*)cttlogo_64x64, 64 * 64, 1, false);
  tft.setAddrWindow(x3, y3, x3 + 64 - 1, y3 + 85 - 1);
  tft.pushColors((const uint8_t*)fmclogo_64x85, 64 * 85, 1, false);
  tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1);
  tft.setTextColor(BLUE);
  tft.setTextSize(3);
  tft.setCursor(28, 100);
  tft.print("FollowMeCooler");
  //18.8px/Letter

  tft.setTextSize(3);
  tft.setCursor(0, 130);
  tft.setTextColor(CYAN, BLACK);
  tft.print("Motor 1:");

  tft.setTextSize(3);
  tft.setCursor(0, 215);
  tft.setTextColor(CYAN, BLACK);
  tft.print("Motor 2:");
}



void loop()
{
  handleSerial();
  //tft.fillRect(0,65,320,480,BLACK);
  tft.setTextSize(2);
  tft.setTextColor(MAGENTA, BLACK);
  if (Motor1 != Motor1vh) {
    Motor1vh = Motor1;
    tft.setCursor(0, 154);
    tft.print("- ");
    tft.print(Motor1 / 2.55);
    tft.println("%  ");
  }
  if (Motor1Richtung != Motor1Richtungvh) {
    Motor1Richtungvh = Motor1Richtung;
    tft.setCursor(0, 174);
    tft.print("- ");
    if (Motor1Richtung == false) {
      tft.print("Forward ");
    }
    else {
      tft.print("Backward");
    }
  }
  if (Motor2 != Motor2vh) {
    Motor2vh = Motor2;
    tft.setCursor(0, 240);
    tft.print("- ");
    tft.print(Motor2 / 2.55);
    tft.println("%  ");
  }
  if (Motor2Richtung != Motor2Richtungvh) {
    Motor2Richtungvh = Motor2Richtung;
    tft.setCursor(0, 260);
    tft.print("- ");
    if (Motor2Richtung == false) {
      tft.print("Forward ");
    }
    else {
      tft.print("Backward");
    }
  }
  if (updatestate == true) {
    updatestate = false;
    tft.fillRect(310, 470, 320, 480, YELLOW);
  }
  else {
    updatestate = true;
    tft.fillRect(310, 470, 320, 480, BLACK);
  }

  int Ultrasonic1 = getAVGDistance(35, 37);
  int Ultrasonic2 = getAVGDistance(33, 31);
  Serial.print("WU1");
  Serial.println(Ultrasonic1);
  Serial.print("WU2");
  Serial.println(Ultrasonic2);


}

void handleSerial() {
  while (Serial.available() > 0) {
    switch (Serial.read()) {
      case 'W':
        switch (Serial.read()) {
          case 'M':
            switch (Serial.read()) {
              case '1':
                Motor1 = (Serial.readString()).toInt();
                analogWrite(Motor1Speed, Motor1);
                //Serial.print("Wrote ");
                //Serial.print(Motor1);
                //Serial.println(" to Motor 1");
                break;

              case '2':
                Motor2 = (Serial.readString()).toInt();
                analogWrite(Motor2Speed, Motor2);
                //Serial.print("Wrote ");
                //Serial.print(Motor2);
                //Serial.println(" to Motor 2");
                break;
            }
            break;
        }
        break;
      case 'C':
        switch (Serial.read()) {
          case 'M':
            switch (Serial.read()) {
              case '1':
                switch (Serial.read()) {
                  case 'F':
                    Motor1Richtung = false;
                    digitalWrite(Motor1Direction, LOW);
                    //Serial.println("Changed Motor 1 to Forward");
                    break;

                  case 'B':
                    Motor1Richtung = true;
                    digitalWrite(Motor1Direction, HIGH);
                    //Serial.println("Changed Motor 1 to Backward");
                    break;
                }
                break;

              case '2':
                switch (Serial.read()) {
                  case 'F':
                    Motor2Richtung = false;
                    digitalWrite(Motor2Direction, LOW);
                    //Serial.println("Changed Motor 2 to Forward");
                    break;

                  case 'B':
                    Motor2Richtung = true;
                    digitalWrite(Motor2Direction, HIGH);
                    //Serial.println("Changed Motor 2 to Backward");
                    break;
                }
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

int getDistance(int trigger, int echo) {
  long distance = 0;
  long timect = 0;

  digitalWrite(trigger, LOW);
  delayMicroseconds(3);
  //noInterrupts();
  digitalWrite(trigger, HIGH); //Trigger Impuls 10 us
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  timect = pulseIn(echo, HIGH, 50000); // Echo-Zeit messen
  //interrupts();

  timect = (timect / 2); // Zeit halbieren
  distance = timect / 29.1; // Zeit in Zentimeter umrechnen
  return (distance);
}

int getAVGDistance(int trigger, int echo) {

  int old = 0;
  int avg;
  int distance;
  int counter;

  delay(10);
  old = getDistance(trigger, echo);
  delay(10);
  for (counter = 0; counter < 10; counter++)
  {
    distance = getDistance(trigger, echo);
    avg = (0.8 * old) + (0.2 * distance);
    old = distance;
    delay(10);
  }
  return (avg);
}
