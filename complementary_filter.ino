#include <Wire.h>
#include <math.h>

const int MPU_ADDR = 0x68;
const float GYRO_SCALE = 131.0f;
const float ACC_SCALE  = 16384.0f;

float alpha = 0.98f;
float pitch = 0, roll = 0;
unsigned long lastTime = 0;
bool firstSample = true;

int16_t ax, ay, az, gx, gy, gz;

void mpuWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission(true);
}

bool mpuRead() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDR, 14, true) != 14) return false;
  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();
  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();
  return true;
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(300);
  mpuWrite(0x6B, 0x00);
  delay(100);
  mpuWrite(0x1B, 0x00); // +/-250 dps
  mpuWrite(0x1C, 0x00); // +/-2g
  lastTime = millis();
  Serial.println(F("pitch_deg,roll_deg"));
  Serial.println(F("Commands: a0.xx = set alpha (0.9-0.999)"));
}

void handleSerial() {
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 'a' || c == 'A') {
    float v = Serial.parseFloat();
    if (v > 0.5f && v < 1.0f) {
      alpha = v;
      Serial.print(F("alpha=")); Serial.println(alpha, 3);
    }
  }
  while (Serial.available()) Serial.read();
}

void loop() {
  handleSerial();
  if (!mpuRead()) {
    delay(20);
    return;
  }

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;
  if (dt <= 0.0f || dt > 0.2f) dt = 0.01f;

  float gyroX = gx / GYRO_SCALE;
  float gyroY = gy / GYRO_SCALE;

  float accPitch = atan2f(-ax, sqrtf((float)ay * ay + (float)az * az)) * 57.2957795f;
  float accRoll  = atan2f(ay, az) * 57.2957795f;

  if (firstSample) {
    pitch = accPitch;
    roll  = accRoll;
    firstSample = false;
  } else {
    pitch = alpha * (pitch + gyroY * dt) + (1.0f - alpha) * accPitch;
    roll  = alpha * (roll  + gyroX * dt) + (1.0f - alpha) * accRoll;
  }

  Serial.print(pitch, 2);
  Serial.print(',');
  Serial.println(roll, 2);

  delay(8);
}
