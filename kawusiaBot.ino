#include <SoftwareSerial.h>
#define RX1 8
#define TX1 9
#define RX2 11
#define TX2 10
#define Trig 7
#define Echo 6
#define EN1 3
#define MC11 4
#define MC12 2
#define EN2 5
#define MC21 12
#define MC22 13
long duration;
int distance;
//String ssid, pswd;
//SoftwareSerial wifis[] = {SoftwareSerial(RX1,TX1),SoftwareSerial(RX2,TX2)} ;
//TODO wifi
/**
 * Funkcja ma zwracać moc odbieraną przez 1-szą antenę.
 * 
 *  Problemy:
 * - Znaki z odpowiedzi czasem wychodzą pozmieniane np. a zaamiast 6 
 * 
 * */
//String wyslij1(String command, int wait_time,int n){
//  //char buff[1024] ={0};
//  String buff;
//  int readCount =0;
//  wifis[n].begin(9600);
//  delay(100);
//   while(wifis[n].available()>0){
//     wifis[n].read();
//  }
//  Serial.println("Setup done.");
//  long startTime = millis();
//  wifis[n].println(command);
//  while(millis()-startTime < wait_time){
//    if(wifis[n].available()>0)
//      buff+=wifis[n].readStringUntil('\n')+'\n';
//  }
//  Serial.println(buff);
//  Serial.println("Done.");
//  wifis[n].end();
//  return "Nic";
//}

//int SignalStrenght1(){
//  wyslij1("AT+CWLAP",5000,0);
//  return 0;
//}

/**
 * Funkcja porusza lewy albo prawym silniekiem do przodu z daną prędkością (1 - 244?)
 * */
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

/**
 * Hamuje dany silnik L/P\
 * */
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
/** 
 * Zwraca odczyt z sonaru w cm 
 * */
int measureDist(){
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  duration = pulseIn(Echo, HIGH);
  distance = duration * 0.034 / 2; 
  return distance; 
}

bool danger=false;
void setup() {
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(MC11, OUTPUT);
  pinMode(MC12, OUTPUT);
  pinMode(MC21, OUTPUT);
  pinMode(MC22, OUTPUT);
  pinMode(Trig, OUTPUT); 
  pinMode(Echo, INPUT); 
  pinMode(RX1,INPUT);
  pinMode(TX1, OUTPUT);
  pinMode(RX2,INPUT);
  pinMode(TX2, OUTPUT);
  //Wszystkie rzeczy związane z wifi docelow tu nie będą 
  breaking("left");
  breaking("right");
  Serial.begin(9600);
  // for(int n=0 ; n<2 ; ++n){
  //   Serial.println("Moduł wifi "+ String(n+1));
  //   wifis[n].begin(152000);
  //   delay(100);
  //   wifis[n].println("AT+RST");
  //   delay(100);
  //   wifis[n].println("AT+IPR=9600");
  //   delay(1000);
  //   wifis[n].end();
  //   wifis[n].begin(9600);
  //   wifis[n].println("AT+CWMODE=1");
  //   delay(100);
  //   Serial.println(wifis[n].readString());
  //   wifis[n].end();
    // delay(1000);
  //}
  //SignalStrenght1();
  forward("left",224);
  forward("right",224);
}

void loop() {
 if(!danger && measureDist()< 15){
    breaking("left");
    breaking("right");
    danger=true;
  }
  if(danger && measureDist()>15){
    forward("left",224);
    forward("right",224);
    danger = false;
  }
 
}
