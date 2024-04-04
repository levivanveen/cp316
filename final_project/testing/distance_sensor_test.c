#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int echo_pin = 2;
const int trigger_pin = 3;
const int backlight_pin = 10;

volatile long starttime, endtime, duration;
volatile bool newVal=false;
bool is_backlit = false;

void setup() {
  pinMode(echo_pin, INPUT);
  pinMode(trigger_pin, OUTPUT);
  pinMode(backlight_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW); // Ensure trigger pin is initially LOW

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);

/////////////////////////////////////////
// first clear registers
//
   TCCR2A = 0b00000000;
   TCCR2B = 0b00000000;
////////////////////////////////////////
// compare output mode options
//
// TCCR2A |=  bit(COM2A1);
   TCCR2A |=              bit(COM2A0);
   TCCR2A |=                          bit(COM2B1);
// TCCR2A |=                                      bit(COM2B0);
///////////////////////////////////////
// waveform generation mode options
//
   TCCR2B |= bit(WGM22);
   //TCCR2A |=              bit(WGM21);
   TCCR2A |=                          bit(WGM20);

///////////////////////////////////////
// Clock selection options
//
  TCCR2B |= bit(CS20);

  OCR2A = 150;
  OCR2B = 50;

  // initialize serial communication:
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(echo_pin), ISR0, RISING);   
}

void loop() {

  if (newVal) {
    Serial.print(duration);
    Serial.print("us ");
    Serial.println();
    newVal=false;
    if (duration < 500) {
      is_backlit = true;
    } else {
      is_backlit = false;
    }
    manage_backlight();
    delay(100);
   }
}

void manage_backlight() {
  if (is_backlit) {
    digitalWrite(backlight_pin, HIGH);
  }
  else {
    digitalWrite(backlight_pin, LOW);
  }
  return;
}

void ISR0(){
  if(digitalRead(echo_pin)==HIGH) {
     starttime=micros();
     attachInterrupt(digitalPinToInterrupt(echo_pin), ISR0, FALLING);   
    } else {
     endtime=micros();
     attachInterrupt(digitalPinToInterrupt(echo_pin), ISR0, RISING);   
     duration=endtime-starttime;
     newVal=true;
    }
}
