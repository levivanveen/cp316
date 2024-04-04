#include <Servo.h>
Servo myservo;
const int servo_pin = 3;

void setup() {
  myservo.attach(servo_pin);
  myservo.write(90); // sets the servo position to neutral
}
void loop() {
  myservo.write(0);
  myservo.write(90);
}