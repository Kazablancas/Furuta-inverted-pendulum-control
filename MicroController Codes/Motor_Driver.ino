#include <SPI.h>
#include <l6474.h>

L6474 myL6474;
static const uint8_t SHIELD = 0;

// ==== CONFIGURE THESE ====
#define MOTOR_FULL_STEPS_PER_REV 200L   // 200 for 1.8° stepper, 400 for 0.9°
#define MICROSTEP_DIV            16L    // if using 1/16 microstepping
#define OUTPUT_GEAR_RATIO_NUM    1L     // motor revs
#define OUTPUT_GEAR_RATIO_DEN    1L     // per output rev (set e.g. 3/1 for 3:1)
// ==========================

static const float STEP_DEG = 5.0f;

long stepsForDegrees(float deg) {
  // microsteps per output revolution:
  // motor_full_steps * microstep * gear_ratio
  long microstepsPerRev = (MOTOR_FULL_STEPS_PER_REV * MICROSTEP_DIV);
  microstepsPerRev = (microstepsPerRev * OUTPUT_GEAR_RATIO_NUM) / OUTPUT_GEAR_RATIO_DEN;

  // steps = microstepsPerRev * deg / 360
  long steps = (long)lround((double)microstepsPerRev * (double)deg / 360.0);
  if (steps < 1) steps = 1;
  return steps;
}

void MyFlagInterruptHandler(void) {
  (void)myL6474.CmdGetStatus(SHIELD);
}

void setup() {
  Serial.begin(115200);

  myL6474.Begin(1);
  myL6474.AttachFlagInterrupt(MyFlagInterruptHandler);

  // Pick your step mode to match MICROSTEP_DIV above
  myL6474.SelectStepMode(SHIELD, L6474_STEP_SEL_1_16);

  // Reasonable motion params for testing
  myL6474.SetMaxSpeed(SHIELD, 1200);
  myL6474.SetAcceleration(SHIELD, 300);
  myL6474.SetDeceleration(SHIELD, 300);

  Serial.println(F("Ready: enter +/- degrees (e.g., +45, -30, 0)"));
}

void loop() {
  // Read a complete line from serial
  static String inputString = "";
  
  while (Serial.available()) {
    char c = (char)Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (inputString.length() > 0) {
        // Parse the signed number
        float targetDeg = inputString.toFloat();
        inputString = "";  // Reset
        
        // Calculate steps and direction
        long steps = stepsForDegrees(fabs(targetDeg));
        uint8_t direction = (targetDeg >= 0) ? FORWARD : BACKWARD;
        
        // Move
        myL6474.Move(SHIELD, direction, (uint32_t)steps);
        myL6474.WaitWhileActive(SHIELD);
        
        Serial.print(F("Moved "));
        Serial.print(targetDeg);
        Serial.print(F(" deg ("));
        Serial.print(steps);
        Serial.println(F(" microsteps)"));
      }
    } else {
      inputString += c;  // Accumulate characters
    }
  }
}
