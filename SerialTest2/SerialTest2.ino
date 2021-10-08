//Use a Terminal that can log to a file.
//Check if the line numbers fit.
//Windows: There is a memory bug in the USB CDC driver, so you'll probably see strange results.

const int n = 100000;
volatile uint32_t t;

char tbuf[256];
int idx;
int szSize;
uint32_t count1;
void setup() {
  Serial.begin(9600);
  // while (!Serial);
  count1 = 1;
  szSize = sprintf( tbuf, "%8lX0<<<>>>___[[[]]]---{{{}}}___!\n", count1 );  // USE SAME STRING below in loop()
  count1 = 0;
  //    Serial.print( szSize );
  for ( idx = 0; tbuf[idx] != 0; idx++ );
  //    Serial.print( idx );
  for ( idx = 0; tbuf[idx] != '0'; idx++ ); // FIRST ZERO in tbuf is unroll digit
  //  Serial.print( idx );
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

void loop() {
  t = micros();
  for (int i = 1; i < n + 1; i += 16) {
    sprintf( tbuf, "%8lX0<<<>>>___[[[]]]---{{{}}}___!\n", count1 );
    Serial.print( tbuf );
    tbuf[idx] = '1';
    Serial.print( tbuf );
    tbuf[idx] = '2';
    Serial.print( tbuf );
    tbuf[idx] = '3';
    Serial.print( tbuf );
    tbuf[idx] = '4';
    Serial.print( tbuf );
    tbuf[idx] = '5';
    Serial.print( tbuf );
    tbuf[idx] = '6';
    Serial.print( tbuf );
    tbuf[idx] = '7';
    Serial.print( tbuf );
    tbuf[idx] = '8';
    Serial.print( tbuf );
    tbuf[idx] = '9';
    Serial.print( tbuf );
    tbuf[idx] = 'A';
    Serial.print( tbuf );
    tbuf[idx] = 'B';
    Serial.print( tbuf );
    tbuf[idx] = 'C';
    Serial.print( tbuf );
    tbuf[idx] = 'D';
    Serial.print( tbuf );
    tbuf[idx] = 'E';
    Serial.print( tbuf );
    tbuf[idx] = 'F';
    Serial.print( tbuf );
    //delayNanoseconds( 24000 );
    count1++;
  }
  t = micros() - t;
  count++;
  Serial.printf("%d Bytes in %d us = %0.2f Mbps\n", n * szSize, t, (double)n * szSize * 8 / t);
  int idx = (n * szSize * 8 / t) / 10;
  if ( idx > 49 ) idx = 49;
  lpsSum[ idx ]++;
  //delay(10);
}
