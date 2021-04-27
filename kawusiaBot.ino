#include <SoftwareSerial.h>
#define RX1 8
#define TX1 9
#define RX2 11
#define TX2 10
#define Trig 6
#define Echo 7
#define EN1 3
#define MC11 4
#define MC12 2
#define EN2 5
#define MC21 12
#define MC22 13

String ssid, pswd;

SoftwareSerial wifi2 = SoftwareSerial(RX1,TX1);
SoftwareSerial wifi1 = SoftwareSerial(RX2,TX2);


String wyslij1(String komenda, int czas_czekania){
  char odp[4]="1111";
  String aa="";
  char a;
  wifi2.println(komenda);
  delay(czas_czekania);
  while(wifi2.available()>0){
     a=wifi2.read();
     aa+=a;
  }
  Serial.print(aa);
  //wifi1.find(ssid+"\",");
  //wifi1.readBytesUntil(",",odp,4);
  return odp;
}

boolean wyslij2(String komenda, char *odpowiedz, int czas_czekania){
  wifi2.println(komenda);
  delay(czas_czekania);
  while(wifi2.available()>0){
    if(wifi2.find(odpowiedz)){
      return 1;
      }
  }
  return 0;
}

int SignalStrenght1(){
  wifi2.begin(115200);
  while(!wifi2);
  Serial.begin(115200);
  Serial.println(wyslij1("AT+CWJAP?",7000));
  return 0;
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

int measureDist(){
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  return pulseIn(Echo, HIGH) * 0.034 / 2; 
}

void setup() {
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(MC11, OUTPUT);
  pinMode(MC12, OUTPUT);
  pinMode(MC21, OUTPUT);
  pinMode(MC22, OUTPUT);
  pinMode(Trig, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(Echo, INPUT); // Sets the echoPin as an INPUT
  pinMode(RX1,INPUT);
  pinMode(TX1, OUTPUT);
  pinMode(RX2,INPUT);
  pinMode(TX2, OUTPUT);
  wifi1.begin(115200);
  wifi1.println("AT+RST");
  delay(100);
  wifi1.println("AT+CWMODE=1");
  delay(100);
  wifi1.println("AT+CWLAP=\""+ ssid + "\",\""+ pswd +"\"");
  delay(5000);
   while(wifi1.available()>0){
     wifi1.read();
  }
  wifi2.begin(115200);
  wifi2.println("AT+RST");
  delay(100);
  wifi2.println("AT+CWMODE=1");
  delay(100);
  wifi2.println("AT+CWLAP=\""+ ssid + "\",\""+ pswd +"\"");
  delay(5000);
   while(wifi2.available()>0){
     wifi2.read();
  }
  breaking("left");
  breaking("right");

  SignalStrenght1();
}

void loop() {
  // put your main code here, to run repeatedly:

}
