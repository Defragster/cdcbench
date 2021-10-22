//Use a Terminal that can log to a file.
//Check if the line numbers fit.
//Windows: There is a memory bug in the USB CDC driver, so you'll probably see strange results.

#define FILL_STRING "<<<>>>___[[[]]]---{{{}}}___!\n"

const int n = 100096;
volatile uint32_t t;

char tbuf[256];
char *ptbuf;
int idx;
int szSize;
uint32_t count = 0;
void setup() {
  Serial.begin(9600);
  // while (!Serial); // exe connect doesn't trigger this
  count = 1;
  szSize = sprintf( tbuf, "%8lX0" FILL_STRING, count );  // USE SAME STRING below in loop()
  delay(1000);
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);
  Serial.print( "Test line in Bytes is " );
  Serial.println( szSize );
  Serial.print( "Number of lines in a group is " );
  Serial.println( n );
  Serial.print( "Bytes in a group is " );
  Serial.println( n * szSize );
#if defined(USB_DUAL_SERIAL)
  SerialUSB1.println("\n" __FILE__ " " __DATE__ " " __TIME__);
  SerialUSB1.print( "Test line in Bytes is " );
  SerialUSB1.println( szSize );
  SerialUSB1.print( "Number of lines in a group is " );
  SerialUSB1.println( n );
  SerialUSB1.print( "Bytes in a group is " );
  SerialUSB1.println( n * szSize );
#endif
  count = 0;
  //    Serial.print( szSize );
  for ( idx = 0; tbuf[idx] != 0; idx++ );
  //    Serial.print( idx );
  for ( idx = 0; tbuf[idx] != '1'; idx++ ); // FIRST ONE in tbuf is unroll digit
  //Serial.println( idx );
  ptbuf = &tbuf[idx + 1]; // get pointer into string for unrolled oavverwrite
  delay(1000);
}

uint32_t lpsSum[200];
uint32_t afwSum[200];
void serialEvent() {
  logEvent( 1 );
}
#if defined(USB_DUAL_SERIAL)
void serialEventUSB1() {
  while ( SerialUSB1.available() ) SerialUSB1.print( (char)SerialUSB1.read()); // echo any input as 'comment'
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
  uint64_t count64 = count * 16;
  SerialUSB1.print("count=");
  SerialUSB1.print(count64);
  SerialUSB1.print(" 0x");
  SerialUSB1.print(count64, HEX);
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
  SerialUSB1.println();
  /*
    for ( int ii = 0; ii < 199; ii++ ) {
      if ( afwSum[ii] > 0 ) {
        SerialUSB1.print("Av4Write ");
        if ( afwSum[ii] < 100 ) {
          SerialUSB1.print(ii);
          SerialUSB1.print(" = ");
          SerialUSB1.println(afwSum[ii]);
        }
        else {
          SerialUSB1.print(ii * 64);
          SerialUSB1.print(" = ");
          SerialUSB1.println(afwSum[ii]);
        }
      }
    }
  */
#endif
}

const char boo[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', };
//#define SHOW_TBUF       outBytes += Serial.print( tbuf )
#define SHOW_TBUF       Serial.print( tbuf )

void loop() {
  double ihold;
  int idxSt;
  /*
    uint32_t outBytes = 0;
    idxSt = Serial.availableForWrite();
    if ( idxSt > 64 ) {
    idxSt = idxSt / 64 + 100;
    }
    afwSum[ idxSt ]++;
  */  t = micros();
  for (int i = 1; i < n + 1; i += 256) {
    sprintf( tbuf, "%8lX0" FILL_STRING, count );
    for (int ii = 0; ii < 16; ii++ ) {
      tbuf[idx] = boo[ii]; // unroll of 16 wasn't enough with sprintf(ser#) - for() of 16 made up the diff
      *ptbuf = '0';
      SHOW_TBUF;
      *ptbuf = '1';
      SHOW_TBUF;
      *ptbuf = '2';
      SHOW_TBUF;
      *ptbuf = '3';
      SHOW_TBUF;
      *ptbuf = '4';
      SHOW_TBUF;
      *ptbuf = '5';
      SHOW_TBUF;
      *ptbuf = '6';
      SHOW_TBUF;
      *ptbuf = '7';
      SHOW_TBUF;
      *ptbuf = '8';
      SHOW_TBUF;
      *ptbuf = '9';
      SHOW_TBUF;
      *ptbuf = 'A';
      SHOW_TBUF;
      *ptbuf = 'B';
      SHOW_TBUF;
      *ptbuf = 'C';
      SHOW_TBUF;
      *ptbuf = 'D';
      SHOW_TBUF;
      *ptbuf = 'E';
      SHOW_TBUF;
      *ptbuf = 'F';
      SHOW_TBUF;
      //delayNanoseconds( 24000 ); // 1500 us/print slows to ~140Mbps, times 16 unrolled is 24000
      count++; // unrolled 9th 0xdigit printed without count++
    }
  }
  t = micros() - t;
  ihold = (double)(n * 8) * szSize / t;
  // Serial.printf("%d B in %d us = %0.2f Mbps\n", n * szSize, t, ihold); // SHOW NORMAL
  // Serial.printf("%d B {Out=%d} in %d us = %0.2f Mbps\n", n * szSize, outBytes, t, ihold); // show outBytes
  // Serial.printf("%d, B in, %d, us = ,%0.2f, Mbps\n", n * szSize, t, ihold); // SHOW CSV for excel - comma seperated
  Serial.printf("%d, B in, %d, us = ,%0.2f, Mbps:%8lX0\n", n * szSize, t, ihold, count); // SHOW CSV for excel - comma seperated : show Ser#
#if defined(USB_DUAL_SERIAL)
    SerialUSB1.printf("%d, B in, %d, us = ,%0.2f, Mbps:%8lX0\n", n * szSize, t, ihold, count); // SHOW CSV for excel - comma seperated : show Ser#
    // SerialUSB1.flush();  ?? This seems to slow it to under 30 Mbps
#endif
  idxSt = ihold / 10;
  if ( idxSt > 199 ) idx = 199;
  lpsSum[ idxSt ]++;
  //delay(10);
}
