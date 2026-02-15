#include <Arduino.h>
#include <RotaryEncoder.h>

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// Measured with your calibration: 600 counts per mechanical revolution
#define COUNTS_PER_REV 600L

RotaryEncoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, RotaryEncoder::LatchMode::FOUR3);

void encoderISR() { encoder.tick(); }

static inline long readEncoderAtomic() {
  long p;
  noInterrupts();
  p = encoder.getPosition();
  interrupts();
  return p;
}

float get_pendulum_angle_deg() {
  long pos = readEncoderAtomic();

  // Convert counts -> degrees (unwrapped)
  float angle = (float)pos * (360.0f / (float)COUNTS_PER_REV);

  // Wrap to [-180, 180)
  while (angle >= 180.0f) angle -= 360.0f;
  while (angle < -180.0f) angle += 360.0f;

  // If you prefer [0, 360), comment the wrap above and use this instead:
  // while (angle >= 360.0f) angle -= 360.0f;
  // while (angle < 0.0f) angle += 360.0f;

  return angle;
}

void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  encoder.setPosition(0); // put pendulum DOWN when powering up (or when you want zero)

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoderISR, CHANGE);
}

void loop() {
  float angle = get_pendulum_angle_deg();
  Serial.print("Angle: ");
  Serial.println(angle, 2);
  delay(50);
}
