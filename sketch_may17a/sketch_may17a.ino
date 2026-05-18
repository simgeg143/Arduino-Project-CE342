#include <LiquidCrystal.h>

#define LcdRWPin 35
#define BackLight 31

int lm35Pin = A4;
int potPin = A8;
int ldr = A11;

int buzzer = 5;
int relay = 6;

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
int GND4 = 36;  // rightmost digit

LiquidCrystal lcd(36, 37, 26, 27, 28, 29);

float temp = 0;
int lm35Value = 0;
int potVal = 0;
int ldrValue = 0;

int displayMode = 0;
char currentChar = ' '; 

void setup(){
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(relay, LOW);

  pinMode(LcdRWPin, OUTPUT);
  digitalWrite(LcdRWPin, LOW);

  pinMode(BackLight, OUTPUT);
  digitalWrite(BackLight, HIGH);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("CE342 Project");
  delay(1500);
  lcd.clear();
}

void updateLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print(" C");
  
  lcd.setCursor(0, 1);
  if(displayMode == 0){
    lcd.print("LDR: ");
    lcd.print(ldrValue);
  } else {
    lcd.print("Pot: ");
    lcd.print(potVal);
  }
  displayMode = 1 - displayMode;
}

void updateSegment(){
  pinMode(aPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(cPin, OUTPUT);
  pinMode(dPin, OUTPUT);
  pinMode(ePin, OUTPUT);
  pinMode(fPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(hPin, OUTPUT);
  
  pinMode(GND1, OUTPUT);
  pinMode(GND2, OUTPUT);
  pinMode(GND3, OUTPUT);
  pinMode(GND4, OUTPUT);
  
  digitalWrite(GND1, HIGH);
  digitalWrite(GND2, LOW);
  digitalWrite(GND3, LOW);
  digitalWrite(GND4, LOW);   
  
  digitalWrite(LcdRWPin, LOW);
  
  if(currentChar == 'A'){
    showA();
  } else if(currentChar == 'C'){
    showC();
  } else {
    clearSegments();
  }
}

void loop(){
  analogRead(potPin); delay(5);
  potVal = analogRead(potPin);
  analogRead(lm35Pin); delay(5);
  lm35Value = analogRead(lm35Pin);
  analogRead(ldr); delay(5);
  ldrValue = analogRead(ldr);

  temp = (lm35Value * 5.0 * 100.0) / 1023.0;

  if(temp < 20){
    currentChar = 'A';
    digitalWrite(buzzer, LOW);
  } else {
    currentChar = 'C';
    if(potVal > 600){
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
    }
  }

  if(ldrValue > 700){
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }

  updateLCD();
  
 updateSegment();

  delay(1500);
}

void clearSegments(){
  digitalWrite(aPin, LOW);
  digitalWrite(bPin, LOW);
  digitalWrite(cPin, LOW);
  digitalWrite(dPin, LOW);
  digitalWrite(ePin, LOW);
  digitalWrite(fPin, LOW);
  digitalWrite(gPin, LOW);
  digitalWrite(hPin, LOW);
}

void showA(){
  clearSegments();
  digitalWrite(aPin, HIGH);
  digitalWrite(bPin, HIGH);
  digitalWrite(cPin, HIGH);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
  digitalWrite(gPin, HIGH);
}

void showC(){
  clearSegments();
  digitalWrite(aPin, HIGH);
  digitalWrite(dPin, HIGH);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
}