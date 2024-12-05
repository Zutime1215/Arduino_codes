// Pin definitions
#define SPEED_CH 2  // White wire - CH3
#define CH1 3       // Black wire 
#define CH2 11      // Orange wire

// Motor pins
#define LEFT_MOTOR_FWD 6   // R1PWM
#define LEFT_MOTOR_REV 5   // L1PWM
#define RIGHT_MOTOR_FWD 10 // R2PWM
#define RIGHT_MOTOR_REV 9  // L2PWM

// Constants
const int TURN_SPEED = 150;
const int PULSE_TIMEOUT = 30000;
const unsigned long SERIAL_BAUD = 115200;

// RC control thresholds
const int RC_HIGH = 1600;
const int RC_LOW = 1350;
const int RC_CENTER_MIN = 1450;
const int RC_CENTER_MAX = 1490;
const int RC_TURN_HIGH = 1750;
const int RC_TURN_LOW = 1250;

// PWM mapping
const int PWM_MIN = 0;
const int PWM_MAX = 255;
const int PULSE_MIN = 1000;
const int PULSE_MAX = 2000;

// Volatile variables for interrupt handling
volatile unsigned long startTimeFWD = 0;
volatile unsigned long currentTimeFWD = 0;
volatile unsigned long pulsesFWD = 0;

struct Motors {
  void stop() {
    analogWrite(LEFT_MOTOR_FWD, 0);
    analogWrite(LEFT_MOTOR_REV, 0);
    analogWrite(RIGHT_MOTOR_FWD, 0);
    analogWrite(RIGHT_MOTOR_REV, 0);
  }
  
  void forward(int speed) {
    analogWrite(LEFT_MOTOR_FWD, speed);
    analogWrite(LEFT_MOTOR_REV, 0);
    analogWrite(RIGHT_MOTOR_FWD, speed);
    analogWrite(RIGHT_MOTOR_REV, 0);
  }
  
  void backward(int speed) {
    analogWrite(LEFT_MOTOR_FWD, 0);
    analogWrite(LEFT_MOTOR_REV, speed);
    analogWrite(RIGHT_MOTOR_FWD, 0);
    analogWrite(RIGHT_MOTOR_REV, speed);
  }
  
  void turnLeft() {
    analogWrite(LEFT_MOTOR_FWD, 0);
    analogWrite(LEFT_MOTOR_REV, TURN_SPEED);
    analogWrite(RIGHT_MOTOR_FWD, TURN_SPEED);
    analogWrite(RIGHT_MOTOR_REV, 0);
  }
  
  void turnRight() {
    analogWrite(LEFT_MOTOR_FWD, TURN_SPEED);
    analogWrite(LEFT_MOTOR_REV, 0);
    analogWrite(RIGHT_MOTOR_FWD, 0);
    analogWrite(RIGHT_MOTOR_REV, TURN_SPEED);
  }
};

Motors motors;

void setup() {
  Serial.begin(SERIAL_BAUD);
  
  // Configure pins
  pinMode(SPEED_CH, INPUT_PULLUP);
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  
  pinMode(LEFT_MOTOR_FWD, OUTPUT);
  pinMode(LEFT_MOTOR_REV, OUTPUT);
  pinMode(RIGHT_MOTOR_FWD, OUTPUT);
  pinMode(RIGHT_MOTOR_REV, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(SPEED_CH), pulseTimerFWD, CHANGE);
}

void loop() {
  static unsigned long lastUpdateTime = 0;
  const unsigned long UPDATE_INTERVAL = 50; // 20Hz update rate
  
  // Throttle loop execution
  if (millis() - lastUpdateTime < UPDATE_INTERVAL) {
    return;
  }
  lastUpdateTime = millis();
  
  // Get speed and channel values
  int speed = getSpeed();
  int ch1Value = readChannel(CH1);
  int ch2Value = readChannel(CH2);
  
  // Control logic
  if (isInDeadzone(ch1Value, ch2Value)) {
    motors.stop();
  }
  else if (ch2Value > RC_HIGH) {
    motors.backward(speed);
  }
  else if (ch2Value < RC_LOW) {
    motors.forward(speed);
  }
  else if (ch1Value > RC_TURN_HIGH) {
    motors.turnLeft();
  }
  else if (ch1Value < RC_TURN_LOW) {
    motors.turnRight();
  }
  
  // Debug output
  #ifdef DEBUG
    Serial.print(speed);
    Serial.print(" ");
    Serial.print(ch1Value);
    Serial.print(" ");
    Serial.println(ch2Value);
  #endif
}

int getSpeed() {
  // Protect against overflow
  unsigned long pulseWidth = (pulsesFWD < PULSE_MAX) ? pulsesFWD : PULSE_MAX;
  return map(pulseWidth, PULSE_MIN, PULSE_MAX, PWM_MIN, PWM_MAX);
}

int readChannel(int channelInput) {
  return pulseIn(channelInput, HIGH, PULSE_TIMEOUT);
}

bool isInDeadzone(int ch1, int ch2) {
  return (ch1 > RC_CENTER_MIN && ch1 < RC_CENTER_MAX) &&
         (ch2 > RC_CENTER_MIN && ch2 < RC_CENTER_MAX);
}

void pulseTimerFWD() {
  currentTimeFWD = micros();
  if (currentTimeFWD > startTimeFWD) {
    pulsesFWD = currentTimeFWD - startTimeFWD;
    startTimeFWD = currentTimeFWD;
  }
}
