#include <LiquidCrystal.h>

int lm35Pin = A4;
int potPin = A8;

int led = 8;
int buzzer = 5;

int aPin = 22;
int bPin = 23;
int cPin = 24;
int dPin = 25;
int ePin = 26;
int fPin = 27;
int gPin = 28;
int hPin = 29;
int GND1 = 33;
int GND2 = 34;
int GND3 = 35;
int GND4 = 36;

LiquidCrystal lcd(36, 37, 26, 27, 28, 29);

float temp = 0;
int lm35Value = 0;
int potVal = 0;

void setup(){

  pinMode(led,OUTPUT);
  pinMode(buzzer,OUTPUT);

  pinMode(aPin,OUTPUT);
  pinMode(bPin,OUTPUT);
  pinMode(cPin,OUTPUT);
  pinMode(dPin,OUTPUT);
  pinMode(ePin,OUTPUT);
  pinMode(fPin,OUTPUT);
  pinMode(gPin,OUTPUT);
  pinMode(hPin,OUTPUT);
  pinMode(GND1,OUTPUT);
  pinMode(GND2,OUTPUT);
  pinMode(GND3,OUTPUT);
  pinMode(GND4,OUTPUT);

  digitalWrite(GND1, HIGH);
  digitalWrite(GND2, LOW);
  digitalWrite(GND3, LOW);
  digitalWrite(GND4, LOW);

  lcd.begin(16,2);

}

void loop(){

potVal = analogRead(potPin);
lm35Value = analogRead(lm35Pin);

temp = (lm35Value*5.0*100.0)/1023.0;

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Temp:" );
lcd.print(temp);
lcd.print(" C")

if(temparature < 20){

  showA();

  digitalWrite(buzzer,LOW);
  digitalWrite(led,LOW);

}else{

  showC();

  if (potVal > 600) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
    }
  }

  delay(500);

}

void showA() {

  digitalWrite(aPin, HIGH);
  digitalWrite(bPin, HIGH);
  digitalWrite(cPin, HIGH);
  digitalWrite(dPin, LOW);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
  digitalWrite(gPin, HIGH);
  digitalWrite(hPin, LOW);
}

void showC() {

  digitalWrite(aPin, HIGH);
  digitalWrite(bPin, LOW);
  digitalWrite(cPin, LOW);
  digitalWrite(dPin, HIGH);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
  digitalWrite(gPin, LOW);
  digitalWrite(hPin, LOW);
}