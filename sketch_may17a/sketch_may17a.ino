/*
 * ============================================================================
 * CE342 Microprocessors Laboratory Project - Spring 2025/2026
 * ----------------------------------------------------------------------------
 * Part I   : Read the LM35 temperature sensor and display it on the LCD.
 *            - Temperature < 20 C  -> show letter "A" on the 7-segment
 *            - Temperature > 20 C  -> show letter "C" on the 7-segment and
 *              sound the buzzer only if the potentiometer reading is > 600
 * Part II  : Read the LDR (light sensor) and the potentiometer, show on LCD.
 *            - LDR > 700 -> light the external LED through the relay
 * Part III : Generate an interrupt using the button on INT1 (pin 3).
 *            - When the button is pressed, LED3 (pin 46) turns on
 *            - LED3 turns off after 10 seconds
 * ============================================================================
 */

#include <LiquidCrystal.h>   // Required library to control the LCD

// Control pins
#define LcdRWPin 35          // LCD Read/Write (R/W) pin
#define BackLight 31         // LCD backlight pin

// Analog sensor pins
int lm35Pin = A4;            // Temperature sensor (LM35)  [Part I]
int potPin = A8;             // Potentiometer              [Part I & II]
int ldr = A11;               // Light sensor (LDR)         [Part II]

// Output pins
int buzzer = 5;              // Buzzer (sound)             [Part I]
int relay = 6;               // Relay -> external LED      [Part II]
int led3Pin = 46;            // LED3, controlled by the INT1 interrupt  [Part III]

// 7-segment display segment pins
int aPin = 22;
int bPin = 23;
int cPin = 24;
int dPin = 25;
int ePin = 26;
int fPin = 27;
int gPin = 28;
int hPin = 29;               // DP (decimal point)

// 7-segment display common-ground (digit select) pins
int GND1 = 33;
int GND2 = 34;
int GND3 = 35;
int GND4 = 36;               // rightmost digit

// LCD object: initialized with (RS, E, D4, D5, D6, D7) pins
LiquidCrystal lcd(36, 37, 26, 27, 28, 29);

// Variables holding the sensor readings
float temp = 0;              // Temperature computed in Centigrade
int lm35Value = 0;           // Raw ADC value of the LM35 (0-1023)
int potVal = 0;              // Raw potentiometer value
int ldrValue = 0;            // Raw LDR value

int displayMode = 0;         // Selects whether LCD row 2 shows LDR or Pot
char currentChar = ' ';      // Character to show on the 7-segment ('A' / 'C')

// Part III: volatile variables shared between the ISR and the main loop
// "volatile" tells the compiler to always read the variable from memory,
// since it can be changed inside the ISR.
volatile bool led3Active = false;          // true while LED3 should be on
volatile unsigned long led3StartTime = 0;  // millis() value when the interrupt fired

void setup(){
  // Configure output pins and start them OFF
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(relay, LOW);

  // Keep the LCD R/W pin LOW so the LCD stays in write mode
  pinMode(LcdRWPin, OUTPUT);
  digitalWrite(LcdRWPin, LOW);

  // Turn on the LCD backlight
  pinMode(BackLight, OUTPUT);
  digitalWrite(BackLight, HIGH);

  // Part III: set up LED3 and the INT1 interrupt
  pinMode(led3Pin, OUTPUT);
  digitalWrite(led3Pin, LOW);              // LED3 starts off
  pinMode(3, INPUT_PULLUP);                // pin 3 = INT1; pull-up -> button press makes it LOW
  // On a button press (FALLING edge) the handleINT1 function runs
  attachInterrupt(digitalPinToInterrupt(3), handleINT1, FALLING);

  // Initialize the LCD as 16 columns x 2 rows and show a startup message
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("CE342 Project");
  delay(1500);
  lcd.clear();
}

// ============================================================================
// LCD update: row 1 shows temperature, row 2 alternates LDR / Pot value
// (Part I temperature display + Part II LDR and Pot display)
// ============================================================================
void updateLCD(){
  lcd.clear();

  // Row 1: Temperature (Part I)
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp, 1);        // print temperature with 1 decimal place
  lcd.print(" C");

  // Row 2: LDR or Pot value (Part II)
  lcd.setCursor(0, 1);
  if(displayMode == 0){
    lcd.print("LDR: ");      // clearly label which value is shown
    lcd.print(ldrValue);
  } else {
    lcd.print("Pot: ");
    lcd.print(potVal);
  }
  displayMode = 1 - displayMode;   // toggle between LDR and Pot each loop
}

// ============================================================================
// Part I: 7-segment display update
// Shows 'A' or 'C' on the rightmost digit depending on temperature
// ============================================================================
void updateSegment(){
  // Set the segment pins as outputs
  pinMode(aPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(cPin, OUTPUT);
  pinMode(dPin, OUTPUT);
  pinMode(ePin, OUTPUT);
  pinMode(fPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(hPin, OUTPUT);

  // Set the digit-select (GND) pins as outputs
  pinMode(GND1, OUTPUT);
  pinMode(GND2, OUTPUT);
  pinMode(GND3, OUTPUT);
  pinMode(GND4, OUTPUT);

  // Enable only the rightmost digit (GND1 HIGH, the others LOW)
  digitalWrite(GND1, HIGH);
  digitalWrite(GND2, LOW);
  digitalWrite(GND3, LOW);
  digitalWrite(GND4, LOW);

  digitalWrite(LcdRWPin, LOW);   // keep LCD R/W LOW

  // Show the character chosen based on temperature
  if(currentChar == 'A'){
    showA();
  } else if(currentChar == 'C'){
    showC();
  } else {
    clearSegments();
  }
}

void loop(){
  // Read the analog values
  // Note: the first analogRead + short delay lets the ADC settle;
  //       the second read is the value actually used.
  analogRead(potPin); delay(5);
  potVal = analogRead(potPin);                 // Potentiometer [Part I & II]
  analogRead(lm35Pin); delay(5);
  lm35Value = analogRead(lm35Pin);             // Temperature   [Part I]
  analogRead(ldr); delay(5);
  ldrValue = analogRead(ldr);                  // Light (LDR)    [Part II]

  // Convert the raw ADC value to Centigrade (LM35: 10mV per degree C)
  temp = (lm35Value * 5.0 * 100.0) / 1023.0;

  // Part I: temperature conditions
  if(temp < 20){
    currentChar = 'A';                         // below 20 C -> "A"
    digitalWrite(buzzer, LOW);                 // buzzer off
  } else {
    currentChar = 'C';                         // above 20 C -> "C"
    if(potVal > 600){                          // additionally, if pot > 600
      digitalWrite(buzzer, HIGH);              // sound the buzzer
    } else {
      digitalWrite(buzzer, LOW);
    }
  }

  // Part II: LDR condition
  if(ldrValue > 700){                          // environment is dark (LDR > 700)
    digitalWrite(relay, HIGH);                 // light the external LED via the relay
  } else {
    digitalWrite(relay, LOW);                  // otherwise LED off
  }

  // Part III: turn LED3 off if 10 seconds have elapsed
  if (led3Active && (millis() - led3StartTime >= 10000)) {
    led3Active = false;
    digitalWrite(led3Pin, LOW);
  }

  updateLCD();       // update the LCD  (Part I & II)

  updateSegment();   // update the 7-segment  (Part I)

  delay(1500);       // wait before the next loop
}

// ============================================================================
// Part III: ISR (Interrupt Service Routine)
// Runs immediately when the button on INT1 (pin 3) is pressed
// ============================================================================
void handleINT1() {
  led3Active = true;            // signal to loop() that LED3 is on
  led3StartTime = millis();     // record the interrupt time for the 10 s timer
  digitalWrite(led3Pin, HIGH);  // turn LED3 on instantly inside the ISR
}

// ============================================================================
// 7-segment helper functions (Part I)
// ============================================================================

// Turn all segments off
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

// Build the letter "A": segments a, b, c, e, f, g are on
void showA(){
  clearSegments();
  digitalWrite(aPin, HIGH);
  digitalWrite(bPin, HIGH);
  digitalWrite(cPin, HIGH);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
  digitalWrite(gPin, HIGH);
}

// Build the letter "C": segments a, d, e, f are on
void showC(){
  clearSegments();
  digitalWrite(aPin, HIGH);
  digitalWrite(dPin, HIGH);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
}
