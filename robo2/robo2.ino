#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 
#define SERVOMAX  600 

int currentPos[6] = {90, 90, 90, 90, 90, 90}; 

void setup() {
  Serial.begin(9600); 
  pwm.begin();
  pwm.setPWMFreq(60); 
  
  // Initialization
  for(int i=0; i<6; i++) {
     moveServo(i, 90);
     delay(200); 
  }
  Serial.println("ROBOT ONLINE");
}

void loop() {
  if (Serial.available() > 0) {
    char state = Serial.read(); 
    int step = 5; 

    // --- MOTOR 5 (CLAW) - INVERTED ---
    if (state == 'f') clawClosed();   // Now goes to 145 
    if (state == 'F') clawOpen();     // Now goes to 65 

    // --- MOTOR 4 (ROTATION)
    if (state == 'V') positionBeak();    // 50
    if (state == 'H') positionPalm();   // 180

    // --- REST OF THE MOTORS ---
    if (state == 'S') changePos(0, step);   // Base
    if (state == 'O') changePos(0, -step);  
    
    if (state == 'C') changePos(1, step);   // Shoulder
    if (state == 'c') changePos(1, -step);  
    
    if (state == 'P') changePos(2, step);   // Elbow
    if (state == 'p') changePos(2, -step);  
    
    if (state == 'U') changePos(3, step);   // Wrist Up/Down
    if (state == 'G') changePos(3, -step);
    
    delay(10); 
  }
}

// --- CLAW FUNCTIONS (MOTOR 5) ---

void clawClosed() {
  int target = 145; 
  
  if(currentPos[5] != target) {
    currentPos[5] = target;
    moveServo(5, target);
    Serial.println("Motor 5 -> CLAW CLOSED (145)");
  }
}

void clawOpen() {
  int target = 65; 
  
  if(currentPos[5] != target) {
    currentPos[5] = target;
    moveServo(5, target);
    Serial.println("Motor 5 -> CLAW OPEN (65)");
  }
}

// --- ROTATION FUNCTIONS (MOTOR 4) ---

void positionBeak() {
  int target = 50;
  if(currentPos[4] != target) {
    currentPos[4] = target;
    moveServo(4, target);
    Serial.println("Motor 4 -> BEAK (50)");
  }
}

void positionPalm() {
  int target = 180;
  if(currentPos[4] != target) {
    currentPos[4] = target;
    moveServo(4, target);
    Serial.println("Motor 4 -> PALM (180)");
  }
}

// --- REST OF THE MOTORS ---
void changePos(int servoIndex, int increment) {
  int newPos = currentPos[servoIndex] + increment;
  
  int minLimit = 0;
  int maxLimit = 180;

  // Motor 3 Protection
  if (servoIndex == 3) { 
    minLimit = 40; 
    maxLimit = 120; 
  }
  
  if (newPos > maxLimit) newPos = maxLimit;
  if (newPos < minLimit) newPos = minLimit;
  
  if (newPos != currentPos[servoIndex]) {
    currentPos[servoIndex] = newPos;
    moveServo(servoIndex, newPos);
    
    Serial.print("Motor ");
    Serial.print(servoIndex);
    Serial.print(" position: ");
    Serial.println(newPos);
  }
}

void moveServo(int servoNum, int degree) {
  int pulse = map(degree, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(servoNum, 0, pulse);
}