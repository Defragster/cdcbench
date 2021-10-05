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


uint32_t count = 0;
uint32_t lpsSum[50];
void serialEvent() {
  logEvent( 1 );
}
#if defined(USB_DUAL_SERIAL)
void serialEventUSB1() {
  while ( SerialUSB1.available() ) SerialUSB1.print( (char)SerialUSB1.read());
  logEvent( 2 );
}
#endif
void logEvent( int uu ) {
  while ( Serial.available() ) {
#if defined(USB_DUAL_SERIAL)
    SerialUSB1.print( (char)Serial.read());
#else
    Serial.read();
#endif
  }
#if defined(USB_DUAL_SERIAL)
  SerialUSB1.print("count=");
  SerialUSB1.print(count);
  SerialUSB1.print("\tSend sec =");
  SerialUSB1.println(millis() / 1000.0);
  SerialUSB1.print(" >> Call by USB==1 or USB1==2 :: ");
  SerialUSB1.println(uu);
  for ( int ii = 0; ii < 49; ii++ ) {
    if ( lpsSum[ii] > 0 ) {
      SerialUSB1.print("Mb/sec ");
      SerialUSB1.print((ii) * 10);
      SerialUSB1.print(" up to ");
      SerialUSB1.print((1 + ii) * 10);
      SerialUSB1.print(" = ");
      SerialUSB1.println(lpsSum[ii]);
    }
  }
  SerialUSB1.print("Bogus Speed calcs = ");
  SerialUSB1.println(lpsSum[49]);
#endif
}
//char szTest[] = "13579abcdefghijklmnopqrstuvwxyz02468ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"; // sz63
char szTest[] = "13579abcdefghijklmnopqrstuvwxyz0\n"; //SZ33
int szSize = sizeof(szTest);
void loop() {
  t = micros();
  for (int i = 1; i < n + 1; i++) {
    Serial.print(szTest);
  }
  t = micros() - t;
  count++;
  Serial.printf("%d Bytes in %d us = %0.2f Megabits per second\n", n * szSize, t, (double)n * szSize * 8 / t);
  int idx = (n * szSize * 8 / t) / 10;
  if ( idx > 49 ) idx = 49;
  lpsSum[ idx ]++;
  //delay(10);
}
