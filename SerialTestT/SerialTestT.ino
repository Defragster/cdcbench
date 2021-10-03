//Use a Terminal that can log to a file.
//Check if the line numbers fit.
//Windows: There is a memory bug in the USB CDC driver, so you'll probably see strange results.

const int n = 100000;
volatile uint32_t t;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  delay(1000);
}

void loop() {
  t = micros();
  for (int i = 1; i < n + 1; i++) {
    Serial.print("13579abcdefghijklmnopqrstuvwxyz_02468ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
  }
  t = micros() - t;
  Serial.printf("%d Bytes in %d us = %0.2f Megabits per second\n", n * 64, t, (double)n * 64 * 8 / t);
  //delay(10);
}
