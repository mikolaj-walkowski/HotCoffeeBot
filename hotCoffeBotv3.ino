#include <SoftwareSerial.h>
#include <math.h>
//Piny
#define RX1 8
#define TX1 9
#define TX2 11
#define RX2 10
#define Trig A0
#define Echo A1
#define EN2 3
#define MC12 4
#define MC11 2
#define EN1 5
#define MC22 12
#define MC21 13
#define YELLOW 9
#define GREEN 8
#define RED 7
//Stałe  globalne
#define moduleDelay 125 // chuj wie w testach wyszło że jest wmiarę równo co 106 
#define threshold -32


unsigned long tStart;
float rssi, rssiMAX;
String ssid = "\"SadSpook\"", pswd = "\"amenoera\"";
SoftwareSerial wifi = SoftwareSerial(RX2, TX2);
enum States {
  AVOID,      //Zajmuje się unikaniem przeszkód
  SEEK,       //Decyduje dokąd jechać
  REST,       //Osiągnął cel
  WAITING,     //Jedzie i czeka na odbiór rssi
  CLOSE       //Bliski stan obrotu
};

States robot_state;
bool compare(const String &big, const int &s, const String &small) {
  for (int i = 0; i < small.length() ; ++i ) {
    if (big[i + s] != small[i]) return false;
  }
  return true;
}
int fis(String in, String target, int i = 0) {
  for (; i < in.length() - target.length(); ++i) {
    if (compare(in, i, target)) {
      return i;
    }
  }
  return -1;
}

void requestRSSI() {
  wifi.println("AT+CWJAP?");
  tStart = millis();
  while (wifi.available() > 0) { //BP To moze powodować wiele problemów
    wifi.read();
  }
}
int recivedRSSI() { //TD dodać opcje na wyświetlanie jaki problem jest
  String buff;
  int start = 0;
  while (wifi.available() == 0) {}
  buff = wifi.readString();
  for (int i = 0; i < 3 ; ++i) start = fis(buff, ",", start + 1);
  String out = buff.substring(start + 1);
  return out.toInt();
}
float getRSSI(int n = 3 ) {
  float  sum = 0;
  for (int i = 0 ; i < n ; ++i ) {
    requestRSSI();
    while (tStart + moduleDelay > millis()) {}
    sum += recivedRSSI();
  }
  sum /= (float)n;
  return sum;
}


int tAcctual = 2500;
void timeProportion(int n) {
  if (n < -50)
    tAcctual = 2500;
  else if (n < -40)
    tAcctual = 2000;
  else if (n < -35)
    tAcctual = 1500;
  else if (n < -30)
    tAcctual = 1000;
  else
    tAcctual = 750;
}

void forward(String motor, int rate) {
  if (motor == "left")
  {
    digitalWrite(EN1, LOW);
    digitalWrite(MC11, HIGH);
    digitalWrite(MC12, LOW);
    analogWrite(EN1, rate);
  }
  else if (motor == "right")
  {
    digitalWrite(EN2, LOW);
    digitalWrite(MC21, HIGH);
    digitalWrite(MC22, LOW);
    analogWrite(EN2, rate);
  }
}
void backward(String motor, int rate) {
  if (motor == "left")
  {
    digitalWrite(EN1, LOW);
    digitalWrite(MC11, LOW);
    digitalWrite(MC12, HIGH);
    analogWrite(EN1, rate);
  }
  else if (motor == "right")
  {
    digitalWrite(EN2, LOW);
    digitalWrite(MC21, LOW);
    digitalWrite(MC22, HIGH);
    analogWrite(EN2, rate);
  }
}
void breaking(String motor) {
  if (motor == "left")
  {
    digitalWrite(EN1, LOW);
    digitalWrite(MC11, LOW);
    digitalWrite(MC12, LOW);
    digitalWrite(EN1, HIGH);
  }
  else if (motor == "right")
  {
    digitalWrite(EN2, LOW);
    digitalWrite(MC21, LOW);
    digitalWrite(MC22, LOW);
    digitalWrite(EN2, HIGH);
  }
}
void rotateByAngle(double angle) {
  int onTime;
  if (angle > 0) {
    onTime = (angle / 80.0) * 1000;
    backward("right", 190);
    delay(onTime);
    breaking("right");
  } else {
    onTime = -1 * (angle / 90.0) * 1000;
    backward("left", 180);
    delay(onTime);
    breaking("left");
  }
}



void motorStop() {
  breaking("left");
  breaking("right");
}
void goF() {
  forward("left", 193); //to powinno sprawić że nie będzie boksował w miejscu
  forward("right", 180);// Przy tak niskiej prędkości lubi stać w miejscu
  delay(10); //delay i prędkości do góry do zmiany
  forward("left", 153); // Im niższa prędkość tym lepiej dla nas wifi moażemy próbkować tylko co ok 110 sec.
  forward("right", 140);
}
long duration;
int distance;
int measureDist() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  duration = pulseIn(Echo, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

void avoidRight() {
  long unsigned timeerr = millis();
  backward("right", 200);
  while (distance <= 30) {
    measureDist();
  }
  long unsigned del = 1.2 * (millis() - timeerr);
  //Serial.println("Timer: " + String(del));
  breaking("right");
  delay(1000);
  timeerr = millis();
  backward("right", 200);
  while ( timeerr + del >= millis()) {
  }
  breaking("right");
}
void avoidLeft() {
  long unsigned timeerr = millis();
  backward("left", 200);
  while (distance <= 30) {
    measureDist();
  }
  long unsigned del = 1.2 * (millis() - timeerr);
  //Serial.println("Timer: " + String(del));
  breaking("left");
  delay(1000);
  timeerr = millis();
  backward("left", 200);
  while ( timeerr + del >= millis()) {
  }
  breaking("left");
}


void wifiSetup() {
  String response;
  wifi.begin(9600);
  delay(100);
  wifi.println("AT+RST");
  while (wifi.available() == 0) {}
  while (wifi.available() > 0) response = wifi.readString();
  digitalWrite(YELLOW, HIGH);
  //  Serial.print("ESP response" + response);
  requestRSSI();
  int count = 0;
  while (recivedRSSI() == 0) {
    requestRSSI();  //czekam aż wifi zacznie wypluwać wyniki
    if (count % 2 == 0 ) {
      digitalWrite(RED, HIGH);
    } else
    {
      digitalWrite(RED, LOW);
    }
    ++count;
    //    Serial.print(".");
  }
  digitalWrite(RED, HIGH);
  //  Serial.println("ESP setup done");
}

void setup() {
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(MC11, OUTPUT);
  pinMode(MC12, OUTPUT);
  pinMode(MC21, OUTPUT);
  pinMode(MC22, OUTPUT);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(RX1, INPUT);
  pinMode(TX1, OUTPUT);
  pinMode(RX2, INPUT);
  pinMode(TX2, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  randomSeed(analogRead(3));
  digitalWrite(GREEN, HIGH);
  //  Serial.begin(9600);
  //  Serial.println("initializing");
  motorStop();
  wifiSetup();
  rssiMAX = getRSSI();
  robot_state = SEEK;
  timeProportion(rssi);
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}
long unsigned tWaiting;
//Trzeba przśpie
void loop() {
  switch (robot_state) {
    case REST:
      motorStop();
      digitalWrite(GREEN, HIGH);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(RED, HIGH);
      //      Serial.println("REST");
      break;
    case AVOID:
      //      Serial.println("AVOID");
      digitalWrite(RED, HIGH);
      if (random(2)) {
        avoidRight();
      } else {
        avoidLeft();
      }
      //Serial.println("Adam mial racje");
      rssiMAX = getRSSI();
      timeProportion(rssiMAX);
      robot_state = WAITING;
      digitalWrite(RED, LOW);
      break;
    case SEEK:
      //      Serial.println("SEEK");
      digitalWrite(GREEN, HIGH);
      motorStop();
      rssi = getRSSI();
      //      Serial.println("rssi: " + String(rssi) + "\nrssiMAX: " + String(rssiMAX) + "\n");
      if (rssi >= threshold) { //jeżeli cel osiągnięty
        robot_state = REST;
        break;
      }
      if (rssi > -45) {
        digitalWrite(GREEN, LOW);
        robot_state = CLOSE;
        break;
      }
      if (rssi >= rssiMAX) {
        rssiMAX = max(rssi, rssiMAX);
        robot_state = WAITING;
        timeProportion(rssi);
        //Serial.println("tW: "+ String(tAcctual));
        //        Serial.println("Jade");
        tWaiting = millis();
        goF();
        digitalWrite(GREEN, LOW);
        break;
      } else {
        if (random(2)) {
          rotateByAngle(90);
        } else {
          rotateByAngle(-90);
        }
        rssiMAX = getRSSI();//przyjmuje jako nowego maxa obecną wartość RSSI
        robot_state = SEEK;
        digitalWrite(GREEN, LOW);
        break;
      }
      break;
    case CLOSE:
      //      Serial.println("CLOSE");
      digitalWrite(YELLOW, HIGH);
      rssiMAX = rssi;
      while (rssiMAX <= rssi) {
        rssiMAX = rssi;
        backward("left", 200);
        delay(380);
        breaking("left");
        rssi = getRSSI();
        //        Serial.println("rssi: " + String(rssi) + "\nrssiMAX: " + String(rssiMAX));
      }
      //      Serial.println("Done right");
      backward("right", 180);
      delay(350);
      rssiMAX = rssi;
      breaking("right");
      while (rssiMAX <= rssi) {
        rssiMAX = rssi;
        backward("right", 180);
        delay(350);
        breaking("right");
        rssi = getRSSI();
        //        Serial.println("rssi: " + String(rssi) + "\nrssiMAX: " + String(rssiMAX));
      }
      //      Serial.println("Done left");
      backward("left", 200);
      delay(380);
      breaking("left");
      timeProportion(rssi);
      //Serial.println("tW: "+String(tAcctual));
      tWaiting = millis();
      goF();

      robot_state = WAITING;
      digitalWrite(YELLOW, LOW);
      break;
    case WAITING:
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, HIGH);
      //Serial.println("WAITING");
      if (tWaiting + tAcctual <= millis()) {
        digitalWrite(RED, LOW);
        digitalWrite(YELLOW, LOW);
        robot_state = SEEK;
      }
      measureDist();
      //Serial.println("Distance :" + String(distance));
      if (distance < 10) { //W przeciwnym wypadku sprawdzam kolizję
        motorStop();
        robot_state = AVOID; //Jeżeli jest kolizja wchodze w stan AVOID
        digitalWrite(RED, LOW);
        digitalWrite(YELLOW, LOW);
        break;
      }
      break;
  }
}
