void setup_lcd() {
  // Set up Lcd shield
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  print_time();

  // Set backlight pin as output
  pinMode(backlight_pin, OUTPUT);
  digitalWrite(backlight_pin, LOW);
  is_backlit = false;
}

void handle_keys() {
  int adc_key_in = analogRead(0);
  key = get_key(adc_key_in);

  if (key != oldkey) {
    delay(50);  // wait for debounce time
    adc_key_in = analogRead(0);    // read the value from the sensor 
    key = get_key(adc_key_in);    // convert into key press
    if (key != oldkey) {
      oldkey = key;
      if (alarm_ringing) {
        turn_off_alarm();
      }
      else {
        func_arr[key]();
      }
    }
  }
}

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

void manage_backlight() {
  // Always turn backlight on when override toggled
  if (backlight_override) {
    digitalWrite(backlight_pin, HIGH);
    is_backlit = true;
  }
  else if ((time_arr[0] > day_end[0] || time_arr[0] < day_begin[0]) ||
    (time_arr[0] == day_end[0] && time_arr[1] >= day_end[1]) ||
    (time_arr[0] == day_begin[0] && time_arr[1] < day_begin[1])) {
    // At night time
    update_backlit_time();

    if (keep_backlight_on()) {
      digitalWrite(backlight_pin, HIGH);
      is_backlit = true;
    }
    else {
      digitalWrite(backlight_pin, LOW);
      is_backlit = false;
      // Reset backlit time 
      backlit_time[0] = -1;
      backlit_time[1] = -1;
      backlit_time[2] = -1;
    }
  } 
  else { // day time - turn off 
    if (is_backlit) {
      digitalWrite(backlight_pin, LOW);
      is_backlit = false;
    }
  }
}

// Key press functions
void time_up(){
  int max_time = (time_mode == 0) ? 23 : 59;
  if (curr_arr[time_mode] < max_time) {
    curr_arr[time_mode]++;
  } else {
    curr_arr[time_mode] = 0;
  }
}

void time_down(){
  int max_time = (time_mode == 0) ? 23 : 59;
  if (curr_arr[time_mode] > 0) {
    curr_arr[time_mode]--;
  } else {
    curr_arr[time_mode] = max_time;
  }
}

void mode_key() {
  switch(current_mode) {
    case 0:
      is24hr = !is24hr;
      break;
    case 1:
      alarm_set = !alarm_set;
      break;
    // Toggle backlight override for day begin mode
    case 2:
      backlight_override = !backlight_override;
      break;
    case 3:
      daylight_savings = !daylight_savings;
      daylight_savings_updated = false;
      break;
  }
}

// Swap from minutes to hours
void time_control() {
  time_mode = (time_mode == HOUR) ? MINUTE : HOUR;
}

void swap_modes() {
  current_mode = (current_mode + 1) % 4; 
  switch(current_mode) {
    case 0:
      curr_arr = time_arr;
      break;
    case 1:
      curr_arr = alarm_arr;
      break;
    case 2:
      curr_arr = day_end;
      break;
    case 3:
      curr_arr = day_begin;
      break;
    lcd.clear();
    time_string[0] = '\0'; // When printing time, time_string won't be equal to temp_time
    print_time();
  }
}
