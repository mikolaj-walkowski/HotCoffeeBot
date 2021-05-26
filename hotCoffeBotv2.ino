#include <SoftwareSerial.h>
#include <math.h>
//Piny
#define RX1 8
#define TX1 9
#define TX2 11
#define RX2 10
#define Trig A0
#define Echo A1
#define EN1 3
#define MC11 4
#define MC12 2
#define EN2 5
#define MC21 12
#define MC22 13
//Stałe  globalne
#define moduleDelay 125 // chuj wie w testach wyszło że jest wmiarę równo co 106 
#define threshold -50

unsigned long tStart;
int rssi, rssiMAX;
String ssid = "\"SadSpook\"", pswd = "\"amenoera\"";
SoftwareSerial wifi = SoftwareSerial(RX2, TX2);
enum States {
  AVOID,      //Zajmuje się unikaniem przeszkód
  SEEK,       //Decyduje dokąd jechać
  REST,       //Osiągnął cel
  WAITING     //Jedzie i czeka na odbiór rssi
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
void motorStop() {
  breaking("left");
  breaking("right");
}
void goF() {
  forward("left", 180); //to powinno sprawić że nie będzie boksował w miejscu
  forward("right", 210);// Przy tak niskiej prędkości lubi stać w miejscu
  delay(30); //delay i prędkości do góry do zmiany
  forward("left", 140); // Im niższa prędkość tym lepiej dla nas wifi moażemy próbkować tylko co ok 110 sec.
  forward("right", 170);
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
void rotateByAngle(double angle) {
  int onTime;
  if (angle > 0) {
    onTime = (angle / 65.0) * 1000;
    backward("right", 200);
    delay(onTime);
    breaking("right");
  } else {
    onTime = -1 * (angle / 85.0) * 1000;
    backward("left", 200);
    delay(onTime);
    breaking("left");
  }
}
void wifiSetup() {
  String response;
  wifi.begin(9600);
  delay(100);
  wifi.println("AT+RST");
  while (wifi.available() == 0) {}
  while (wifi.available() > 0) response = wifi.readString();
  Serial.print("ESP response" + response);
  requestRSSI();
  while (recivedRSSI() == 0) {
    requestRSSI();  //czekam aż wifi zacznie wypluwać wyniki
    Serial.print(".");
  }
  Serial.println("ESP setup done");
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
  Serial.begin(9600);
  Serial.println("initializing");
  motorStop();
  wifiSetup();
  requestRSSI();
  rssiMAX = recivedRSSI();
  rssi = rssiMAX;
  robot_state = SEEK;
  goF();
}

void loop() {
  switch (robot_state) {
    case REST:
      Serial.println("REST");
      break;
    case AVOID:
      Serial.println("AVOID");
      rotateByAngle(90); //omijam przeszkodę
      goF();
      delay(2000);
      requestRSSI();//RSSI startowe
      motorStop();
      rotateByAngle(-90);//wracam do oryginalnej orientacji
      while (tStart + moduleDelay > millis()) {}//czekam na odpowiedź modułu
      rssiMAX = recivedRSSI();//przyjmuje jako nowego maxa obecną wartość RSSI
      goF();//Przygotowuje się do wejścia w stan waiting
      requestRSSI();
      robot_state = WAITING;
      break;
    case SEEK:
      Serial.println("SEEK");
      if (rssi > threshold) { //jeżeli cel osiągnięty
        robot_state = REST;
      }
      Serial.println("rssiMax: " + String(rssiMAX) + " rssi: " + String(rssi));
      if (rssi + 2 > rssiMAX) { //jeżeli rssi ~> rssi MAX TD jeżeli ktoś ma lepszy pomysł na ignorowanie zakłóceń to pisać
        rssiMAX = max(rssi, rssiMAX);//Jeżeli wartość rzeczywiście większa to przyjmuje ją jako nowego maxa
        requestRSSI();
        Serial.println("Decision: go straight");
        robot_state = WAITING;//Jade dalej w tym samym kierunku
      } else {
        Serial.println("Decision: turn");
        requestRSSI();//Jeżeli wartość naprawde zmalała to zmieniam kireunek
        motorStop();
        rotateByAngle(90);
        while (tStart + moduleDelay > millis()) {}//czekam na nowego maxa
        rssiMAX = recivedRSSI();
        goF();
        requestRSSI();
        robot_state = WAITING;
      }
    case WAITING:
      Serial.println("WAITING");
      if (tStart + moduleDelay >= millis()) { //Jeżeli otrzymałem nowe RSSI to wchodze w tryb SEEK
        rssi = recivedRSSI();
        if (rssi == 0) {
          Serial.println("Bad rssi");
        }
        robot_state = SEEK;
      }
      Serial.println("Distance :" + String(measureDist()));
      if (distance < 10) { //W przeciwnym wypadku sprawdzam kolizję
        motorStop();
        robot_state = AVOID; //Jeżeli jest kolizja wchodze w stan AVOID

      }
      break;
  }

}
