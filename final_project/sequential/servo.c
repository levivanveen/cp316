void setup_servo() {
  myservo.attach(servo_pin);
}

void alarm_check() {
  if (alarm_set && time_arr[0] == alarm_arr[0] && time_arr[1] == alarm_arr[1]) {
    // Alarm going off!
    alarm_ringing = true;
    alarm_set = false;
    digitalWrite(backlight_pin, HIGH);
    is_backlit = true;
  }
}

void ring_bell(){
  myservo.write(0);
  delay(200);
  myservo.write(90);
  delay(200);
}

void update_snooze() {
  int seconds, mins, hours;
  int curr_sec = time_arr[2];
  int curr_min = time_arr[1];
  int curr_hour = time_arr[0];
  int snooze_seconds = 10;

  seconds = (snooze_seconds + curr_sec) % ONE_MINUTE;
  mins = (((snooze_seconds + curr_sec) / ONE_MINUTE) + curr_min) % HOURS_IN_MINUTES;
  hours = (((snooze_seconds + curr_sec) / ONE_MINUTE) + curr_min) / HOURS_IN_MINUTES;
  hours = (hours + curr_hour) % 24;

  snooze_time[0] = hours;
  snooze_time[1] = mins;
  snooze_time[2] = seconds;
}

void handle_alarm() {
  if (detected_object()) {
    update_snooze();
  }
  if (keep_snoozing()) {
    alarm_snoozed = true;
    return;
  }
  // Alarm isn't snoozed
  alarm_snoozed = false;
  ring_bell();
}

void turn_off_alarm() {
  alarm_ringing = false;
  alarm_snoozed = false;
}

bool keep_snoozing() {
  return (
    snooze_time[0] > time_arr[0] || 
    (snooze_time[0] == time_arr[0] && snooze_time[1] > time_arr[1]) || 
    (snooze_time[0] == time_arr[0] && snooze_time[1] == time_arr[1] && snooze_time[2] > time_arr[2])
  );
}
