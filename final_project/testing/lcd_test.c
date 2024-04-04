#include <LiquidCrystal.h>
#include <time.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
char msgs[5][16] = {
                    "Right  Key OK",
                    "Up     Key OK",
                    "Down   Key OK",
                    "Left   Key OK",
                    "Select Key OK"
                     };

// LCD keys
int adc_key_val[5] ={100, 200, 400, 600,950};
int NUM_KEYS = 5;
int key=-1;
int oldkey=-1;

// Functions for keys
void mins_up();
void hours_up();
void hours_down();
void mins_down();
void toggle_24hr();
void (*func_arr[5])() = {
                          mins_up, // Right key
                          hours_up, // Up key
                          hours_down, // Down key
                          mins_down, // Left key
                          toggle_24hr // Select
                        };

// Misc
bool is24hr = false;
bool is_backlit = true;
int time_offset[2] = {0, 0}; // Hours, mins
int STRING_SIZE = 50;
 
void setup()
{
  // Set up Lcd shield
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  print_time();

  // Set up pin 10 as output
  pinMode(10, OUTPUT);

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

  // %%%%%%%%% TESTING PURPOSE ONLY %%%%%%%%%
  // Initialize serial communication 
  Serial.begin(9600);
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
}

void loop()
{
  // %%%%%%%%% TESTING PURPOSE ONLY %%%%%%%%%
   if (Serial.available() > 0) {
     char incomingByte = Serial.read();
     if (incomingByte == '1') {
       is_backlit = !is_backlit;
     }
   }
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  print_time();
  manage_backlight();
  int adc_key_in = analogRead(0);    // read the value from the sensor 
  key = get_key(adc_key_in);  // convert into key press

  if (key != oldkey) {
    delay(50);  // wait for debounce time
    adc_key_in = analogRead(0);    // read the value from the sensor 
    key = get_key(adc_key_in);    // convert into key press
    if (key != oldkey) {   
      lcd.setCursor(0, 1);
      oldkey = key;
      func_arr[key]();
    }
  }
  delay(100);
}

///////////////////////////////////////

// Convert ADC value to key number
int get_key(unsigned int input)
{
    int k;
    for (k = 0; k < NUM_KEYS; k++)
    {
      if (input < adc_key_val[k])
      {
        return k;
      }
    }   
    if (k >= NUM_KEYS)k = -1;  // No valid key pressed
    return k;
}

void print_time() {
  char time_string[STRING_SIZE];
  get_time(time_string);
  lcd.clear();
  lcd.println(time_string);
}

char* get_time(char* time_string) {
  time_t curr_time;
  struct tm *local_time;

  // Get current time
  curr_time = time(NULL);
  local_time = localtime(&curr_time);

  // Add time offset
  local_time->tm_hour += time_offset[0];
  local_time->tm_min += time_offset[1];

  // Normalize time values if they exceed their respective ranges
  if (local_time->tm_min >= 60) {
    local_time->tm_min %= 60;
  }
  else if (local_time->tm_min < 0) {
    local_time->tm_min += 60;
  }
  if (local_time->tm_hour >= 24) {
    local_time->tm_hour %= 24;
  }
  else if (local_time->tm_hour < 0) {
    local_time->tm_hour += 24;
  }

  //Format time
  char format[9]; // Maximum size is 8 characters plus 1 for the null terminator
  strcpy(format, (is24hr) ? "%R" : "%I:%M %p");

  // Put formatted time string into time_string parameter
  strftime(time_string, STRING_SIZE, format, local_time);
}

void manage_backlight() {
  if (is_backlit) {
    digitalWrite(10, HIGH);
  }
  else {
    digitalWrite(10, LOW);
  }
  return;
}

// Key press functions
void hours_up(){
  if (time_offset[0] < 23) {
    time_offset[0]++;
  } else {
    time_offset[0] = 0;
  }
}

void hours_down(){
  if (time_offset[0] > -23) {
    time_offset[0]--;
  } else {
    time_offset[0] = 0;
  }
}

void mins_up() {
  if (time_offset[1] < 59) {
    time_offset[1]++;
  } else {
    time_offset[1] = 0;
  }
}

void mins_down() {
  if (time_offset[1] > -59) {
    time_offset[1]--;
  } else {
    time_offset[1] = 0;
  }
}

void toggle_24hr() {
  is24hr = !is24hr;
}