namespace motorControl {

byte leftOffset = 0;
byte rightOffset = 0;
/* This could be a pwm class */
unsigned long pwmTimeout = 0;

#define AIN1 3
#define AIN2 4
#define APWM 5
#define BIN1 12
#define BIN2 13
#define BPWM 11
#define STBY 6
#define STEPTIME 600
#define STRAIGHTSPEED 200
#define TURNSPEED 120
#define TURNTIME 300

int pwms[] = {APWM, BPWM};

void writePwms ( int left, int right) {
  analogWrite (pwms[0], left);
  analogWrite (pwms[1], right);
}

void goForward () {
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  writePwms (STRAIGHTSPEED - leftOffset, STRAIGHTSPEED - rightOffset);
  pwmTimeout = millis() + STEPTIME;
}

void goBack() {
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  writePwms (STRAIGHTSPEED - leftOffset, STRAIGHTSPEED - rightOffset);
  pwmTimeout = millis() + STEPTIME;
}

void goLeft () {
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  writePwms (TURNSPEED, TURNSPEED);
  pwmTimeout = millis() + TURNTIME;
}

void goRight () {
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  writePwms (TURNSPEED, TURNSPEED);
  pwmTimeout = millis() + TURNTIME;
}

void stop() {
  //enable standby
  digitalWrite(STBY, LOW);
}

void init() {
  pinMode (STBY, OUTPUT);
  pinMode (AIN1, OUTPUT);
  pinMode (AIN2, OUTPUT);
  pinMode (APWM, OUTPUT);
  pinMode (BIN1, OUTPUT);
  pinMode (BIN2, OUTPUT);
  pinMode (BPWM, OUTPUT);
}

}


