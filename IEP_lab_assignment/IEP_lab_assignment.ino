/****
NAME:WANGZEHUA & YE THIHA AUNG
ADM NUM:2515520 & 2538022
CLASS:DCEP/FT/1A/09
PROJECT:HOME GARDENING
PROJECT:HOME GARDENING
****/

#include <Wire.h>
#include "RichShieldDHT.h"
#include "RichShieldTM1637.h"
#include "RichShieldLED.h"
#include "RichShieldPassiveBuzzer.h"

// ==== Pin Definitions ====
#define DHTPIN A0      // sensor pin
#define CLK 10         // TM1637 CLK
#define DIO 11         // TM1637 DIO

#define LED_RED 4
#define LED_GREEN 5
#define LED_BLUE 6
#define LED_YELLOW 7

#define BUZZER_PIN 3

#define BUTTON_MODE 8
#define BUTTON_BUZZ 9

#define PassiveBuzzerPin 3
PassiveBuzzer buz(PassiveBuzzerPin);

//------------------------------------
//This is a macro definition of bass, midrange, and treble frequencies
#define NOTE_L1  262
#define NOTE_L2  294
#define NOTE_L3  330
#define NOTE_L4  349
#define NOTE_L5  392
#define NOTE_L6  440
#define NOTE_L7  494
#define NOTE_M1  523
#define NOTE_M2  587
#define NOTE_M3  659
#define NOTE_M4  698
#define NOTE_M5  784
#define NOTE_M6  880
#define NOTE_M7  988
#define NOTE_H1  1046
#define NOTE_H2  1175
#define NOTE_H3  1318
#define NOTE_H4  1397
#define NOTE_H5  1568
#define NOTE_H6  1760
#define NOTE_H7  1976

// ==== Objects ====
DHT dht;                      // integrated DHT sensor object
TM1637 disp(CLK, DIO);        // 7-seg display


// ==== Display symbol indexes from example ====
#define INDEX_NEGATIVE_SIGN 10
#define INDEX_C 12
#define INDEX_H 18

// ==== Globals ====
bool showTemp = true;       // true = temp mode; false = humidity mode
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

bool buzzerActive = false;  // Track if buzzer should be making sound

void setup() {
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_BUZZ, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT); // Buzzer output pin
pinMode(LED_RED,OUTPUT);
pinMode(LED_GREEN,OUTPUT);
pinMode(LED_BLUE,OUTPUT);
pinMode(LED_YELLOW,OUTPUT);
  disp.init();
  dht.begin();

}

void loop() {
  // Read sensor values
  float tempVal = dht.readTemperature();
  float humVal = dht.readHumidity();

  // Check for NaN
  if (isnan(tempVal) || isnan(humVal)) {
    displayError();
    return;
  }

  // Button to toggle display mode with debounce
  if (digitalRead(BUTTON_MODE) == LOW) {
    if (millis() - lastButtonPress > debounceDelay) {
      showTemp = !showTemp;
      lastButtonPress = millis();
    }
  }

  // Display either temperature or humidity
  if (showTemp) {
    displayTemperature((int8_t)tempVal);
  } else {
    displayHumidity((int8_t)humVal);
  }

  // LED warning logic

  if (tempVal < 20) digitalWrite(4,HIGH);   // LED1: Low Temp warning
  if (tempVal > 21) digitalWrite(5,HIGH);   // LED2: High Temp warning
  if (humVal < 40) digitalWrite(6,HIGH);    // LED3: Low Humidity warning
  if (humVal > 70) digitalWrite(7,HIGH);    // LED4: High Humidity warning

// ==== Buzzer cooldown settings ====
static unsigned long lastBuzzerTime = 0;
const unsigned long buzzerCooldown = 10000; // 10 seconds

// Auto buzzer alert if out of range
if (tempVal < 20 || tempVal > 30 || humVal < 40 || humVal > 70) {
  if (digitalRead(BUTTON_BUZZ) == LOW) {
    buzOff(); // Manual stop
  } 
  else if (millis() - lastBuzzerTime > buzzerCooldown) {
    buzOn();
    sing();
    lastBuzzerTime = millis(); // Start cooldown
  }
}
else {
  buzOff();
}


  // If buzzer is active, generate tone
  if (buzzerActive) {
    // Generate ~2 kHz tone for loud buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(250);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(250);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// ==== Display Functions ====

void displayTemperature(int8_t temperature) {
  int8_t temp[4];
  if (temperature < 0) {
    temp[0] = INDEX_NEGATIVE_SIGN;
    temperature = abs(temperature);
  } else if (temperature < 100) {
    temp[0] = INDEX_BLANK;
  } else {
    temp[0] = temperature / 100;
  }
  temperature %= 100;
  temp[1] = temperature / 10;
  temp[2] = temperature % 10;
  temp[3] = INDEX_C;  // Display 'C' symbol for Celsius
  disp.display(temp);
}

void displayHumidity(int8_t humidity) {
  int8_t temp[4];
  if (humidity < 100) {
    temp[0] = INDEX_BLANK;
  } else {
    temp[0] = humidity / 100;
  }
  humidity %= 100;
  temp[1] = humidity / 10;
  temp[2] = humidity % 10;
  temp[3] = INDEX_H;  // Display 'H' symbol for humidity
  disp.display(temp);
}

void displayError() {
  int8_t err[4] = {INDEX_BLANK, INDEX_BLANK, INDEX_BLANK, 14}; // 'E'
  disp.display(err);
}

// ==== Buzzer control (manual tone, no Timer 2) ====
void buzOn() {
  buzzerActive = true;
}

void buzOff() {
  buzzerActive = false;
}

void sing() {
  for (int repeat = 0; repeat < 5; repeat++) {  // play siren 5 times
    // Sweep up
    for (int freq = 400; freq <= 1000; freq += 10) {
      if (digitalRead(BUTTON_BUZZ) == LOW) { // Stop if button pressed
        buzOff();
        return;
      }
      buz.playTone(freq, 10);
    }
    // Sweep down
    for (int freq = 1000; freq >= 400; freq -= 10) {
      if (digitalRead(BUTTON_BUZZ) == LOW) { // Stop if button pressed
        buzOff();
        return;
      }
      buz.playTone(freq, 10);
    }
  }
}
