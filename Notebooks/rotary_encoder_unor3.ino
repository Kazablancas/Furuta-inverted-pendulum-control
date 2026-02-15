#include <RotaryEncoder.h>

#define ENCODER_PIN_A 2  // Interrupt-capable pin A
#define ENCODER_PIN_B 3  // Interrupt-capable pin B

#define ENC_STEPS_PER_ROTATION 1200  // Adjust based on your encoder specs

RotaryEncoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, RotaryEncoder::LatchMode::TWO03);

void setup() {
  Serial.begin(115200);

  encoder.setPosition(0);

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoderISR, CHANGE);
}

void loop() {
  static int lastPosition = 0;
  int currentPosition = encoder.getPosition();

  if (currentPosition != lastPosition) {
    float angle = get_encoder_angle();
    //Serial.print("Position: ");
    //Serial.print(currentPosition);
    Serial.print("  Angle: ");
    Serial.println(angle, 2);
    lastPosition = currentPosition;
  }

  delay(50);
}

void encoderISR() {
  encoder.tick();
}
/**
float get_encoder_angle() {
  int pos = encoder.getPosition();

  pos = pos % ENC_STEPS_PER_ROTATION;
  if (pos < 0) pos += ENC_STEPS_PER_ROTATION;

  return (float)pos * (360.0 / ENC_STEPS_PER_ROTATION);
}
*/

float get_encoder_angle() {
  int pos = encoder.getPosition();

  pos = pos % ENC_STEPS_PER_ROTATION;
  if (pos < 0) pos += ENC_STEPS_PER_ROTATION;

  float angle = (float)pos * (360.0 / ENC_STEPS_PER_ROTATION);
  angle = fmod(angle + 180.0, 360.0) - 180.0;  // Wrap angle to [-180, 180)

  return angle;
}
