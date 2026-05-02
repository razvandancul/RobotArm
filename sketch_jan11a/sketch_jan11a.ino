#include <Wire.h>

const int MPU_HAND = 0x68; 
const int MPU_ARM  = 0x69; 

const int PIN_INDEX = A0; 
const int PIN_PINKY = A1; 
const int PIN_THUMB = A2; 

// --- FLEX SETTINGS ---
int INDEX_LOW  = 180; int INDEX_HIGH = 240; //270
int REST_LOW   = 150; int REST_HIGH  = 230; 

// --- GYRO SETTINGS ---
int THRESHOLD_ARM      = 40;   
int THRESHOLD_ROTATION = 45; 

char lastRotation = ' '; 

void setup() {
  Serial.begin(9600); 
  Wire.begin();
  Wire.setWireTimeout(3000, true); 

  Wire.beginTransmission(MPU_HAND); Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);
  Wire.beginTransmission(MPU_ARM);  Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);
}

void loop() {
  int valIndex = analogRead(PIN_INDEX);
  int valPinky = analogRead(PIN_PINKY);
  int valThumb = analogRead(PIN_THUMB);
  
  int yArm  = getVal(MPU_ARM, 0x3D); 
  int zArm  = getVal(MPU_ARM, 0x3F); 
  int xHand = getVal(MPU_HAND, 0x3B); 

  bool sentSomething = false; 

  // ============================================================
  // 1. FINGERS
  // ============================================================
  if (!sentSomething) {
    if (valIndex < INDEX_LOW)       { Serial.write('f'); sentSomething = true; }
    else if (valIndex > INDEX_HIGH) { Serial.write('F'); sentSomething = true; }

    else if (valThumb < REST_LOW)   { Serial.write('P'); sentSomething = true; }
    else if (valThumb > REST_HIGH)  { Serial.write('p'); sentSomething = true; }

    else if (valPinky < REST_LOW)   { Serial.write('G'); sentSomething = true; }
    else if (valPinky > REST_HIGH)  { Serial.write('U'); sentSomething = true; }
  }

  // ============================================================
  // 2. ARM 
  // ============================================================
  if (!sentSomething) {
    if (yArm < -THRESHOLD_ARM)      { Serial.write('S'); sentSomething = true; }
    else if (yArm > THRESHOLD_ARM)  { Serial.write('O'); sentSomething = true; }
    
    else if (zArm < -THRESHOLD_ARM) { Serial.write('c'); sentSomething = true; }
    else if (zArm > THRESHOLD_ARM)  { Serial.write('C'); sentSomething = true; }
  }

  // ============================================================
  // 3. HAND ROTATION
  // ============================================================
  if (!sentSomething) {
    char newState = ' ';
    if (xHand > THRESHOLD_ROTATION) newState = 'H'; 
    else                            newState = 'V'; 

    if (newState != lastRotation) {
      Serial.write(newState);
      lastRotation = newState;
      sentSomething = true;
    }
  }

  delay(400); 
}

int getVal(int addr, int reg) {
  Wire.beginTransmission(addr); Wire.write(reg); Wire.endTransmission(false);
  Wire.requestFrom(addr, 2, true);
  if(Wire.available() < 2) return 0;
  int16_t val = Wire.read() << 8 | Wire.read();
  return map(val, -17000, 17000, -90, 90);
}