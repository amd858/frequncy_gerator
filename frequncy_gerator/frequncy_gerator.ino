void setup() {
  // Configure Digital Pins 9 and 10 as outputs
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Clear Timer 1 Control Registers
  TCCR1A = 0;
  TCCR1B = 0;

  // Set Mode 14: Fast PWM with ICR1 as TOP
  // WGM11 = 1, WGM12 = 1, WGM13 = 1
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // Configure Channel A (Pin 9): Non-Inverting Fast PWM
  // Clears on match, sets at BOTTOM
  TCCR1A |= (1 << COM1A1);

  // Configure Channel B (Pin 10): Inverting Fast PWM
  // Sets on match, clears at BOTTOM (Creates the perfect differential pair)
  TCCR1A |= (1 << COM1B1) | (1 << COM1B0);

  // Set the TOP value to define the 28 kHz frequency precisely
  // Formula for Fast PWM: TOP = (F_cpu / (N * F_target)) - 1
  // TOP = (16,000,000 / (1 * 28,000)) - 1 = 570.42 -> Round to 570
  ICR1 = 570;

  // Initialize both channels to 0% output
  OCR1A = 0;    // Non-inverting: 0 means completely LOW
  OCR1B = 570;  // Inverting: 570 (TOP) means completely LOW

  // CRITICAL FIX: Start the timer by setting the prescaler to N = 1 (CS10 = 1)
  TCCR1B |= (1 << CS10);
}

void loop() {
  // 1. SOFT ATTACK: Smoothly ramp to a 50% duty cycle (570 / 2 = 285)
  for (int i = 0; i <= 285; i++) {
    // OCR1A = i;        // Pin 9 scales up from 0 to 50%
    OCR1B = 570 - i;  // Pin 10 scales down from 100% to 50% (Inverted)
    delayMicroseconds(200); 
  }
  for (int i = 285; i >= 0; i--) {
    OCR1A = i;
    // OCR1B = 570 - i;
    delayMicroseconds(200);
  }
  delay(125); // Main active window at 28 kHz differential

  for (int i = 0; i <= 285; i++) {
    OCR1A = i;        // Pin 9 scales up from 0 to 50%
    // OCR1B = 570 - i;  // Pin 10 scales down from 100% to 50% (Inverted)
    delayMicroseconds(200); 
  }
  // 2. SOFT DECAY: Smoothly ramp back down to 0% duty cycle
  for (int i = 285; i >= 0; i--) {
    // OCR1A = i;
    OCR1B = 570 - i;
    delayMicroseconds(200);
  }
  delay(125); // Main quiet window
}
