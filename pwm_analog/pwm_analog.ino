int pulse_per_sec = 60; // Pulse Per Second.
int serial_in;

void setup() {
  pinMode(11, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    serial_in = Serial.read();

    Serial.print("I received: ");
    Serial.println(serial_in);
  }
  // pwm_analog(11, serial_in);
}



void pwm_analog(int pwm_pin, int brightness) {
  int pulse_time = 1000/(pulse_per_sec);
  float pulse_one = pulse_time*(brightness/100.0);
  float pulse_zero = pulse_time - pulse_one;

  digitalWrite(pwm_pin, 1);
  delay(pulse_one);
  digitalWrite(pwm_pin, 0);
  delay(pulse_zero);
}
