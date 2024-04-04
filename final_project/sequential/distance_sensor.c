int ONE_MINUTE = 60;
int HOURS_IN_MINUTES = 60;
int MIN_DISTANCE = 0;
int MAX_DISTANCE = 3000;
int MIN_TIME_MILLI = 100;
int MAX_TIME_MILLI = 60000;
int MILLIS_IN_SECOND = 1000;

void setup_distance_sensor() {
  pinMode(echo_pin, INPUT);
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW); // Ensure trigger pin is initially LOW
}

// Updates the time until backlight should turn off
void update_backlit_time() {
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH);

  if (duration > MAX_DISTANCE || duration < MIN_DISTANCE) {
    return; // Object detected too far away
  }
  
  int backlight_seconds = abs(map(duration, MIN_DISTANCE, MAX_DISTANCE, MAX_TIME_MILLI, MIN_TIME_MILLI) / MILLIS_IN_SECOND) + 2;
  int seconds, mins, hours;
  int curr_sec = time_arr[2];
  int curr_min = time_arr[1];
  int curr_hour = time_arr[0];

  seconds = (backlight_seconds + curr_sec) % ONE_MINUTE;
  mins = (((backlight_seconds + curr_sec) / ONE_MINUTE) + curr_min) % HOURS_IN_MINUTES;
  hours = (((backlight_seconds + curr_sec) / ONE_MINUTE) + curr_min) / HOURS_IN_MINUTES;
  hours = (hours + curr_hour) % 24;

  backlit_time[0] = hours;
  backlit_time[1] = mins;
  backlit_time[2] = seconds;
}

bool keep_backlight_on() {
  return (
    backlit_time[0] > time_arr[0] || 
    (backlit_time[0] == time_arr[0] && backlit_time[1] > time_arr[1]) || 
    (backlit_time[0] == time_arr[0] && backlit_time[1] == time_arr[1] && backlit_time[2] > time_arr[2])
  );
}

bool detected_object() {
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH);

  return (duration < MAX_DISTANCE && duration > MIN_DISTANCE); 
}
