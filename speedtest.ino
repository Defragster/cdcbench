
//Use a Terminal that can log to a file, or "serialtest.c"
//Windows: There is a memory bug in the USB CDC driver, so you'll probably see strange results.

const int n = 100000;
volatile uint32_t t;

const char teststr[] = "123456789\n";
const size_t size = sizeof teststr - 1;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  delay(1000);
}

void loop() {
  t = micros();
  for (int i = 1; i < n + 1; i++) {
    /*while ( Serial.availableForWrite() < 10 )*/ 
    Serial.print(teststr);
  }
  t = micros() - t;
  Serial.printf("%d Bytes in %d us = %0.2f Megabits per second\n", n * size, t, n * size * 8 / (double)t);
  delay(100); //not really needed - just to slow it down a bit
}
