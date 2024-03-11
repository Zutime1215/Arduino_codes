#define speedCH 2
volatile long StartTimeFWD = 0;
volatile long CurrentTimeFWD = 0;
volatile long PulsesFWD = 0;
int PulseWidthFWD = 0;

#define CH1 3
#define CH2 11
int ch1Value;
int ch2Value;

// /FOR LEFT MOTOR/
int R1PWM = 6;
int L1PWM = 5;

// /FOR RIGHT MOTOR/
int R2PWM = 10;
int L2PWM = 9;

void setup(){
  Serial.begin(115200);
  pinMode(speedCH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(speedCH),PulseTimerFWD,CHANGE);
  
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);

  pinMode(R1PWM, OUTPUT);
  pinMode(L1PWM, OUTPUT);
  pinMode(R2PWM, OUTPUT);
  pinMode(L2PWM, OUTPUT);
}


void loop() {
  if (PulsesFWD < 2000){
    PulseWidthFWD = PulsesFWD;
  }
  int Speed = map(PulseWidthFWD, 1000, 2000, 0, 255);

  ch1Value = readChannel(CH1);
  ch2Value = readChannel(CH2);
  
  if (ch2Value > 1600)  { //forward
    analogWrite(R1PWM, Speed);
    analogWrite(L1PWM, 0);

    analogWrite(R2PWM, Speed);
    analogWrite(L2PWM, 0);
  }

  else if (ch2Value < 1350)  {//backward
    analogWrite(R1PWM, 0);
    analogWrite(L1PWM, Speed);

    analogWrite(R2PWM, 0);
    analogWrite(L2PWM, Speed);
  }

  else if (ch1Value > 1600)  {//LEFT
    analogWrite(R1PWM, 0);
    analogWrite(L1PWM, Speed);

    analogWrite(R2PWM, Speed);
    analogWrite(L2PWM, 0);
  }

  else if (ch1Value < 1350)  {//RIGHT
    analogWrite(R1PWM, Speed);
    analogWrite(L1PWM, 0);

    analogWrite(R2PWM, 0);
    analogWrite(L2PWM, Speed);
  }

  else if ( ((ch1Value > 1470) && (ch1Value < 1490)) && ((ch2Value > 1470) && (ch2Value < 1490)) )  { //STOP
    analogWrite(R1PWM, 0);
    analogWrite(L1PWM, 0);

    analogWrite(R2PWM, 0);
    analogWrite(L2PWM, 0);
  }

  // Serial.print("Speed: ");
  Serial.print(Speed);
  Serial.print(" ");
  Serial.print(ch1Value);
  Serial.print(" ");
  Serial.println(ch2Value);
  
  // delay(50);
}

int readChannel(int channelInput){
  int ch = pulseIn(channelInput, HIGH, 30000);
  return ch;
}

void PulseTimerFWD(){
  CurrentTimeFWD = micros();
  if (CurrentTimeFWD > StartTimeFWD){
    PulsesFWD = CurrentTimeFWD - StartTimeFWD;
    StartTimeFWD = CurrentTimeFWD;
  }
}

