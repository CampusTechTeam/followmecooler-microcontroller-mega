#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <CheapStepper.h>
#include "MCUFRIEND_kbv.h"
MCUFRIEND_kbv tft;

#include "cttlogo.h"
#include "fmclogo.h"

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
//IPAddress ip(192, 168, 2, 3);
IPAddress server(192, 168, 2, 1);

CheapStepper stepper (31, 33, 35, 37);

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

#define Motor2Speed 45
#define Motor2Direction 27
#define Motor1Speed 44
#define Motor1Direction 40
#define Motor1Hall 18
#define Motor2Hall 19

#define SERIAL_BUFFER_SIZE 256

int rpminterval = 50;
#define encoderturn 1401.55


long rpmpreviousMillis = 0;
long rpmcurrentMillis = 0;
int rpmcounter = 0;
int stallcounter = 0;

float Motor1RPM = 0;
float Motor2RPM = 0;
volatile long Motor1Encoder = 0;
volatile long Motor2Encoder = 0;

boolean targetreached = true;
boolean manual = false;
boolean motorstate = false;
int targetstate = 0;
int degree = -1;
int targetdegree = -1;
int Motor1 = 0;
int Motor2 = 0;
int Motor1PWM = 0;
int Motor2PWM = 0;
int Motor1Travel = 35;
int Motor2Travel = 35;
int Motor1vh = -1;
int Motor2vh = -1;
boolean Motor1Dir = false;
boolean Motor2Dir = false;
boolean updatestate = true;
//False = Vorwärts

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("followmecooler/status/megastate", "1");
      // ... and resubscribe
      client.subscribe("followmecooler/mega/manual/Motor1");
      client.subscribe("followmecooler/mega/manual/Motor2");
      client.subscribe("followmecooler/mega/manual/");
      client.subscribe("followmecooler/mega/manual/Motor");
      client.subscribe("followmecooler/cooler/compass");
      client.subscribe("followmecooler/jetson/targetdegree");
      client.subscribe("followmecooler/jetson/meandist");
      client.subscribe("followmecooler/jetson/motorstate");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  String payloadString = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadString += (char)payload[i];
  }
  Serial.println("");

  if (String(topic) == "followmecooler/mega/manual/Motor1" && payloadString.toInt() >= -230 && 230 >= payloadString.toInt() && manual == true) {
    if (payloadString.toInt() <= -15 || 15 <= payloadString.toInt() || 0 == payloadString.toInt()) {
      Serial.println("Motor1 " + payloadString);
      if (payloadString.toInt() < 0) {
        Motor1Dir = true;
        Motor1 = payloadString.toInt() * -1;
        Motor1PWM = payloadString.toInt() * -1;
        digitalWrite(Motor1Direction, HIGH);
        analogWrite(Motor1Speed, Motor1PWM);
      }
      else {
        Motor1Dir = false;
        Motor1 = payloadString.toInt();
        Motor1PWM = payloadString.toInt();
        digitalWrite(Motor1Direction, LOW);
        analogWrite(Motor1Speed, Motor1PWM);
      }

    }
  }
  if (String(topic) == "followmecooler/mega/manual/Motor2" && payloadString.toInt() >= -230 && 230 >= payloadString.toInt() && manual == true) {
    if (payloadString.toInt() <= -15 || 15 <= payloadString.toInt() || 0 == payloadString.toInt()) {
      Serial.println("Motor2 " + payloadString);

      if (payloadString.toInt() < 0) {
        Motor2Dir = true;
        Motor2 = payloadString.toInt() * -1;
        Motor2PWM = payloadString.toInt() * -1;
        digitalWrite(Motor2Direction, HIGH);
        Serial.println("Backwards");
        analogWrite(Motor2Speed, Motor2PWM * -1);
      }
      else {
        Motor2Dir = false;
        Motor2 = payloadString.toInt();
        Motor2PWM = payloadString.toInt();
        digitalWrite(Motor2Direction, LOW);
        analogWrite(Motor2Speed, Motor2PWM);
      }

    }
  }
  if (String(topic) == "followmecooler/mega/manual/Motor" && payloadString.toInt() >= -230 && 230 >= payloadString.toInt() && manual == true) {
    if (payloadString.toInt() <= -15 || 15 <= payloadString.toInt() || 0 == payloadString.toInt()) {
      Serial.println("Motor2 " + payloadString);

      if (payloadString.toInt() < 0) {
        Motor2Dir = true;
        Motor2 = payloadString.toInt() * -1;
        Motor2PWM = payloadString.toInt() * -1;
        digitalWrite(Motor2Direction, HIGH);
        analogWrite(Motor2Speed, Motor2PWM);
      }
      else {
        Motor2Dir = false;
        Motor2 = payloadString.toInt();
        Motor2PWM = payloadString.toInt();
        digitalWrite(Motor2Direction, LOW);
        analogWrite(Motor2Speed, Motor2PWM);
      }
      Serial.println("Motor1 " + payloadString);

      if (payloadString.toInt() < 0) {
        Motor1Dir = true;
        Motor1 = payloadString.toInt()*-1;
        Motor1PWM = payloadString.toInt()*-1;
        digitalWrite(Motor1Direction, HIGH);
        analogWrite(Motor1Speed, Motor1PWM);
      }
      else {
        Motor1Dir = false;
        Motor1 = payloadString.toInt();
        Motor1PWM = payloadString.toInt();
        digitalWrite(Motor1Direction, LOW);
        analogWrite(Motor1Speed, Motor1PWM);
      }

    }
  }
  if (String(topic) == "followmecooler/mega/manual/") {
    if (payloadString.toInt() == 1) {
      manual = true;
    }
    else {
      manual = false;
    }
  }
  if (String(topic) == "followmecooler/jetson/targetdegree" && manual == false && degree != -1) {
    targetdegree = payloadString.toInt();
    if (targetdegree + 3 < degree && motorstate == true) {
      Serial.print("going left ");
      Serial.print(targetdegree);
      Serial.print("/");
      Serial.println(degree);
      float MotorOffset = degree - targetdegree;
      if (MotorOffset * 1.25 > 50) {
        MotorOffset = 50 / 1.25;
      }
      Motor1 = Motor1Travel - MotorOffset * 0.62;
      Motor2 = Motor2Travel + MotorOffset * 1.25;

    }
    if (targetdegree - 3 > degree && motorstate == true) {
      Serial.print("going right ");
      Serial.print(targetdegree);
      Serial.print("/");
      Serial.println(degree);
      float MotorOffset = targetdegree - degree;
      if (MotorOffset * 1.25 > 50) {
        MotorOffset = 50 / 1.25;
      }
      Motor1 = Motor1Travel + MotorOffset * 0.62;
      Motor2 = Motor2Travel - MotorOffset * 1.25;
    }
    if (targetdegree + 3 >= degree && targetdegree - 3 <= degree && motorstate == true) {
      Serial.print("centered ");
      Serial.print(targetdegree);
      Serial.print("/");
      Serial.println(degree);
      if (Motor1 != Motor1Travel) {
        analogWrite(Motor1Speed, Motor1Travel);
        Motor1PWM = Motor1Travel;
      }
      if (Motor2 != Motor2Travel) {
        analogWrite(Motor2Speed, Motor2Travel);
        Motor2PWM = Motor2Travel;
      }
      Motor1 = Motor1Travel;
      Motor2 = Motor2Travel;
    }
  }

  if (String(topic) == "followmecooler/jetson/meandist") {
    float meandist = payloadString.toFloat();
    Motor1Travel = meandist * 20;
    Motor2Travel = meandist * 20;
    if (Motor1Travel > 75) {
      Motor1Travel = 75;
    }
    if (Motor2Travel > 75) {
      Motor2Travel = 75;
    }
  }


  if (String(topic) == "followmecooler/cooler/compass") {
    degree = payloadString.toInt();
  }

  if (String(topic) == "followmecooler/jetson/motorstate") {
    if (payloadString.toInt() == 0) {
      motorstate = false;
      Motor1 = 0;
      Motor2 = 0;
      Motor1PWM = 0;
      Motor2PWM = 0;
      analogWrite(Motor1Speed, 0);
      analogWrite(Motor2Speed, 0);
    }
    if (payloadString.toInt() == 1) {
      motorstate = true;
      Motor1 = Motor1Travel;
      Motor2 = Motor2Travel;
      Motor1PWM = Motor1Travel;
      Motor2PWM = Motor2Travel;
      analogWrite(Motor1Speed, Motor1Travel);
      analogWrite(Motor2Speed, Motor2Travel);
    }
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(1);
  uint16_t ID = tft.readID();

  Serial.print(F("ID = 0x"));
  Serial.println(ID, HEX);
  stepper.setRpm(16);

  pinMode(Motor1Speed, OUTPUT);
  pinMode(Motor1Direction, OUTPUT);
  pinMode(Motor2Speed, OUTPUT);
  pinMode(Motor2Direction, OUTPUT);
  pinMode(Motor1Hall, INPUT);
  pinMode(Motor2Hall, INPUT);
  pinMode(37, INPUT);
  pinMode(35, OUTPUT);
  pinMode(31, INPUT);
  pinMode(33, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(Motor1Hall), updateEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(Motor2Hall), updateEncoder2, RISING);

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
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.init(53);
  Ethernet.begin(mac);
  delay(1500);
}



void loop()
{
  if (!client.connected() && stallcounter == 0) {
    reconnect();
  }
  client.loop();

  rpmcurrentMillis = millis();
  if (rpmcurrentMillis - rpmpreviousMillis > rpminterval) {
    rpmpreviousMillis = rpmcurrentMillis;
    Motor1RPM = (float)((Motor1Encoder * 1980 / encoderturn));
    Motor2RPM = (float)((Motor2Encoder * 1980 / encoderturn));
    String Motor1RPMString = (String)((Motor1Encoder * 1950 / encoderturn));
    String Motor2RPMString = (String)((Motor2Encoder * 1950 / encoderturn));
    char Motor1RPMchar[6];
    char Motor2RPMchar[6];
    Motor1RPMString.toCharArray(Motor1RPMchar, 6);
    Motor2RPMString.toCharArray(Motor2RPMchar, 6);
    if (Motor1RPM > Motor1 + 5 && Motor1 > 0) {
      if (Motor1PWM - 3 > 0) {
        Motor1PWM -= 3;
      }
      analogWrite(Motor1Speed, Motor1PWM);
    }
    if (Motor1RPM < Motor1 - 5 && Motor1 > 0) {
      if (Motor1PWM + 3 < 255) {
        Motor1PWM += 3;
      }
      analogWrite(Motor1Speed, Motor1PWM);
    }
    if (Motor2RPM > Motor2 + 5 && Motor2 > 0) {
      if (Motor2PWM - 3 > 0) {
        Motor2PWM -= 3;
      }
      analogWrite(Motor2Speed, Motor2PWM);
    }
    if (Motor2RPM < Motor2 - 5 && Motor2 > 0) {
      if (Motor2PWM + 3 < 255) {
        Motor2PWM += 3;
      }
      analogWrite(Motor2Speed, Motor2PWM);
    }
    if (Motor2RPM <= 42 && Motor2PWM > 150) {
      stallcounter++;
      Serial.println("stall");
    }
    if (Motor1RPM <= 42 && Motor1PWM > 150) {
      stallcounter++;
      Serial.println("stall");
    }
    if (Motor1RPM >= 50 && Motor1PWM < 100 && Motor2RPM >= 50 && Motor2PWM < 100 && stallcounter > 0) {
      stallcounter--;
    }
    if (stallcounter >= 18) {
      analogWrite(Motor1Speed, 0);
      analogWrite(Motor2Speed, 0);
      Motor1 = 0;
      Motor2 = 0;
      Motor1PWM = 0;
      Motor2PWM = 0;
      stallcounter = 0;
      Serial.println("STALL");
      delay(3000);
    }

    /*Serial.print("Motor1RPM: ");
      Serial.println(Motor1RPM);
      Serial.print("Motor2RPM: ");
      Serial.println(Motor2RPM);*/
    Motor1Encoder = 0;
    Motor2Encoder = 0;




    if (rpmcounter >= 3) {
      rpmcounter = 0;
      char Motor1PWMchar[3];
      char Motor2PWMchar[3];
      char targetdegreechar[5];

      String(targetdegree).toCharArray(targetdegreechar, 5);
      String(Motor1PWM).toCharArray(Motor1PWMchar, 3);
      String(Motor2PWM).toCharArray(Motor2PWMchar, 3);

      client.publish("followmecooler/mega/Motor1RPM", Motor1RPMchar, true);
      client.publish("followmecooler/mega/Motor2RPM", Motor2RPMchar, true);
      client.publish("followmecooler/mega/Motor1PWM", Motor1PWMchar, true);
      client.publish("followmecooler/mega/Motor2PWM", Motor2PWMchar, true);
      client.publish("followmecooler/mega/targetdegree", targetdegreechar, true);
    }
    else {
      rpmcounter++;
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

void updateEncoder1() {
  Motor1Encoder++;
}
void updateEncoder2() {
  Motor2Encoder++;
}
