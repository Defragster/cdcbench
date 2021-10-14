//Use a Terminal that can log to a file.
//Check if the line numbers fit.
//Windows: There is a memory bug in the USB CDC driver, so you'll probably see strange results.

#define FILL_STRING "<<<>>>___[[[]]]---{{{}}}___!\n"

const int n = 100096;
volatile uint32_t t;

char tbuf[256];
int idx;
int szSize;
uint32_t count;
void setup() {
  Serial.begin(9600);
  // while (!Serial);
  count = 1;
  szSize = sprintf( tbuf, "%8lX0" FILL_STRING, count );  // USE SAME STRING below in loop()
  count = 0;
  //    Serial.print( szSize );
  for ( idx = 0; tbuf[idx] != 0; idx++ );
  //    Serial.print( idx );
  for ( idx = 0; tbuf[idx] != '1'; idx++ ); // FIRST ONE in tbuf is unroll digit
    Serial.print( idx );
  delay(1000);
}

uint32_t lpsSum[200];
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
  for ( int ii = 0; ii < 199; ii++ ) {
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
  SerialUSB1.println(lpsSum[199]);
#endif
}

char boo[]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', };
void loop() {
  double ihold;
  t = micros();
  char *ptbuf = &tbuf[idx+1];
  for (int i = 1; i < n + 1; i += 256) {
    sprintf( tbuf, "%8lX0" FILL_STRING, count );
    for (int ii = 0; ii < 16; ii++ ) {
      tbuf[idx] = boo[ii];
      *ptbuf = '0';
      Serial.print( tbuf );
      *ptbuf = '1';
      Serial.print( tbuf );
      *ptbuf = '2';
      Serial.print( tbuf );
      *ptbuf = '3';
      Serial.print( tbuf );
      *ptbuf = '4';
      Serial.print( tbuf );
      *ptbuf = '5';
      Serial.print( tbuf );
      *ptbuf = '6';
      Serial.print( tbuf );
      *ptbuf = '7';
      Serial.print( tbuf );
      *ptbuf = '8';
      Serial.print( tbuf );
      *ptbuf = '9';
      Serial.print( tbuf );
      *ptbuf = 'A';
      Serial.print( tbuf );
      *ptbuf = 'B';
      Serial.print( tbuf );
      *ptbuf = 'C';
      Serial.print( tbuf );
      *ptbuf = 'D';
      Serial.print( tbuf );
      *ptbuf = 'E';
      Serial.print( tbuf );
      *ptbuf = 'F';
      Serial.print( tbuf );
      //delayNanoseconds( 24000 );
      count++;
    }
  }
  t = micros() - t;
  ihold = (double)(n*8) * szSize / t;
  Serial.printf("%d Bytes in %d us = %0.2f Mbps\n", n * szSize, t, ihold);
  int idx = ihold/10;
  if ( idx > 199 ) idx = 199;
  lpsSum[ idx ]++;
  //delay(10);
}
