#include <LiquidCrystal.h>
#include <time.h>
#include <string.h>
#include <Servo.h>

// Pins 
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int backlight_pin = 10;

const int servo_pin = 3;

const int echo_pin = 2;
const int trigger_pin = 13;

// Misc vars for lcd
bool is24hr = false;
int current_mode = 0;
bool is_backlit = false;
bool backlight_override = false;
int STRING_SIZE = 50;
int MINUTE = 1;
int HOUR = 0;
int DEFAULT_MODE = 0;

// Global time vars
bool daylight_savings = false;
bool daylight_savings_updated = true;
bool time_recently_updated = true;
unsigned long last_update_time = 0;
bool alarm_set = false;
bool alarm_ringing = false;
bool alarm_snoozed = false;
int time_mode;

int time_arr[3] = {12, 0, 0}; // Hours, mins, seconds
int alarm_arr[3] = {12, 0, 0};
int day_end[3] = {19, 0}; // Hours, mins
int day_begin[3] = {7, 0}; 
int backlit_time[3] = {-1, -1, -1};
int snooze_time[3] = {-1, -1, -1};

int* curr_arr; // Used when updating current time or alarm time
char time_string[50];
////////////////////////////////////////////////////
// LCD keys
int adc_key_val[5] ={100, 200, 400, 600,950};
int NUM_KEYS = 5;
int key=-1;
int oldkey=-1;

// Functions prototypes
void time_control();
void time_up();
void time_down();
void mode_key();
void swap_modes();

// Function pointer array
void (*func_arr[5])() = {
                          time_control, // Right key
                          time_up, // Up key
                          time_down, // Down key
                          mode_key, // Left key
                          swap_modes // Select 
                        };

////////////////////////////////////////////////////
// Distance Sensor stuff
volatile long starttime, endtime;
volatile bool newVal=false;
////////////////////////////////////////////////////
// Servo
Servo myservo;


void setup() {
  setup_lcd();
  setup_distance_sensor();
  setup_servo();

  // General Setup
  // first clear registers
   TCCR2A = 0b00000000;
   TCCR2B = 0b00000000;

  // compare output mode options
  // TCCR2A |=  bit(COM2A1);
   TCCR2A |=              bit(COM2A0);
   TCCR2A |=                          bit(COM2B1);
  // TCCR2A |=                                      bit(COM2B0);

  // waveform generation mode options
   TCCR2B |= bit(WGM22);
   //TCCR2A |=              bit(WGM21);
   TCCR2A |=                          bit(WGM20);

  // Clock selection options
  TCCR2B |= bit(CS20);

  OCR2A = 150;
  OCR2B = 50;

  curr_arr = time_arr;
  time_mode = HOUR;
}

void loop() {
  update_time();
  print_time();

  if (!alarm_ringing) {
    manage_backlight();
    alarm_check();
  }
  else {
    handle_alarm();
  }

  handle_keys();
}

void update_time() {
  if (!daylight_savings_updated) {
    daylight_savings_updated = true;
    int temp_time_mode = time_mode;

    curr_arr = time_arr;
    time_mode = HOUR;
    (daylight_savings) ? time_up() : time_down();
    curr_arr = day_begin;
    time_mode = temp_time_mode;
  }

  unsigned long curr_time = millis();
  unsigned long elapsed_time = curr_time - last_update_time;
  time_arr[2] = (elapsed_time / 1000) % 60;

  while (elapsed_time >= 60000) {
    elapsed_time -= 60000; // Subtract one minute worth of milliseconds from elapsed_time
    last_update_time += 60000; // Update last_update_time to reflect the minute increment

    time_arr[1]++; // Increment minutes
    if (time_arr[1] >= 60) { // Correctly handle overflow and increment hours
      time_arr[1] = 0; // Reset minutes to 0
      time_arr[0]++; // Increment hours
      time_arr[0] %= 24; // Ensure hours stay within a 24-hour format
    }
  }
}

void print_time() {
  char temp_time[STRING_SIZE];
  get_time(temp_time);

  if (strcmp(temp_time, time_string) != 0) {
    lcd.clear();
    lcd.print(temp_time);
    if (current_mode == 2) {
      lcd.print(" Night");
    }
    else if (current_mode == 3) {
      lcd.print(" Morning");
    }
    strcpy(time_string, temp_time);
  }

  // Display text on second line if necessary
  lcd.setCursor(0, 1);
  switch(current_mode) {
    case 0: // default/clock mode
      if (alarm_set) {
        lcd.print("Alarm Set");
      }
      else if (alarm_snoozed) {
        lcd.print("Alarm Snoozed");
      }
      else if (alarm_ringing) {
        lcd.print("Alarm Ringing");
      }
      break;
    case 1:
      lcd.print("Set Alarm - ");
      lcd.print(alarm_set ? "On " : "Off");
      break;
    case 2:
      lcd.print("Toggle Light");
      break;
    case 3:
      lcd.print("Daylight savings");
      break;
  }
  lcd.setCursor(0, 0);
}

void get_time(char* time_string) {
  struct tm time_struct;

  // Initialize time_struct with values from curr_arr
  time_struct.tm_hour = curr_arr[0];
  time_struct.tm_min = curr_arr[1];
  time_struct.tm_sec = curr_arr[2];

  //Format time
  char format[10];
  if (current_mode == DEFAULT_MODE) {
    strcpy(format, (is24hr) ? "%H:%M:%S" : "%I:%M:%S %p");
  }
  else {
    strcpy(format, (is24hr) ? "%R" : "%I:%M %p");
  }
  // Put formatted time string into time_string parameter
  strftime(time_string, STRING_SIZE, format, &time_struct);
}
