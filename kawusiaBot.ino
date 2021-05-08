#include <SoftwareSerial.h>
#include <math.h>
#define RX1 8
#define TX1 9
#define TX2 11
#define RX2 10
//#define Trig 7
//#define Echo 6
#define Trig A0
#define Echo A1
#define EN1 3
#define MC11 4
#define MC12 2
#define EN2 5
#define MC21 12
#define MC22 13
long duration;
int distance;
String ssid= "cus1", pswd = "cus";
SoftwareSerial wifis[] = {SoftwareSerial(RX1,TX1),SoftwareSerial(RX2,TX2)} ;
//TODO wifi
/**
 * Funkcja ma zwracać moc odbieraną przez 1-szą antenę.
 * 
 *  Problemy:
 * - Znaki z odpowiedzi czasem wychodzą pozmieniane np. a zaamiast 6 
 * 
 * */
String wyslij(String command, int wait_time,int n){
  String buff;
  wifis[n].begin(9600);
  delay(100);
   while(wifis[n].available()>0){
     wifis[n].read();
  }
  long startTime = millis();
  wifis[n].println(command);
  while(millis()-startTime < wait_time){
    if(wifis[n].available()>0)
      buff+=wifis[n].readStringUntil('\n')+'\n';
  }
  wifis[n].end();
  return  buff;
}
bool compare(const String &big, const int &s,const String &small){
  for(int i = 0; i< small.length() ; ++i ){
    if(big[i+s]!=small[i]) return false; 
  }
  return true;
}
int fis(String in, String target, int i=0){
    for(; i< in.length()-target.length();++i){
        if(compare(in,i,target)){
          return i;
      }
  }
  return -1;
}

int rssi(String in){
  int start=0;
  for(int i=0; i<3 ; ++i) start = fis(in,",",start+1);
  String out = in.substring(start+1);
  return out.toInt();
}

int SignalStrenghtNoC(int n){
  return  rssi(wyslij("AT+CWJAP?",2000,n));
}
double wifiDist(int n){
  int Signal = SignalStrenghtNoC(n);
  double out = 0.02 *pow(10, (-15.0 - Signal)/20);
  Serial.println("RSSI: "+String(Signal)+" Calculated dist: "+ String(out));
  return out;
}

void directionNoC(double* t){
  double d02 = pow(wifiDist(0),2);
  Serial.println(d02);
  t[0] = ( d02 - pow(wifiDist(1),2))/24;
  t[1] = sqrt(d02-pow(t[0]+6,2));
} 



/**
 * Funkcja porusza lewy albo prawym silniekiem do przodu z daną prędkością (1 - 255)
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
void wifiSetupNoC(){
    for(int n=0 ; n<2 ; ++n){
    //  Serial.println("Moduł wifi "+ String(n+1));
     wifis[n].begin(9600);
     delay(100);
    //  Serial.println(wifis[n].readString());
     wifis[n].println("AT+RST");
     delay(100);
    //  Serial.println(wifis[n].readString());
     wifis[n].println("AT+CWMODE=1");
     delay(100);
    //  Serial.println(wifis[n].readString());
     wifis[n].println("AT+CWJAP="+ssid+","+pswd);
     delay(5000);
     while(wifis[n].available()>0) wifis[n].read();
     wifis[n].end();
     delay(100);
  }
}

//bool danger=false;
double vec[2];
float del = 1000;
bool run = true;
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
  wifiSetupNoC();
  Serial.begin(9600);

  // SignalStrenght(0);
  // SignalStrenght(1);
//  forward("left",224);
//  delay(1000);
//  backward("left",244);
//  delay(1000);
//  breaking("left");
//  forward("right",224);
//  delay(1000);
//  backward("right",244);
//  delay(1000);
//  breaking("right");
}

void loop() {
if(run){
  directionNoC(&vec[0]);
  Serial.println("X: "+ String(vec[0])+" Y: "+String(vec[1]));
  if(vec[0]<-0.5){
    //forward("left",255);
    delay(del);
    Serial.println("Turning left");
    //breaking("left");
    //del/=2;
  }else if(vec[0]>0.5){
    //forward("right",255);
    delay(del);
    Serial.println("Turning right");
    //breaking("right");
    //del/=2;
  }
  if(del < 50){
    run=false;
  }
}

// if(!danger && measureDist()< 15){
//    breaking("left");
//    breaking("right");
//    danger=true;
//  }
//  if(danger && measureDist()>15){
//    forward("left",224);
//    forward("right",224);
//    danger = false;
//  }
  
  
}
