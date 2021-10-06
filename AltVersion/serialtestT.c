
// Code taken from
// https://www.pololu.com/docs/0J73/15.6
// and modified for my needs.

// gcc -m64 serialtest.c -O3 -Wall -oserialtest.exe
// strip serialtest.exe

// Frank B, 10/2021

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

void print_error(const char * context)
{
  DWORD error_code = GetLastError();
  char buffer[256];
  DWORD size = FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
    NULL, error_code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
    buffer, sizeof(buffer), NULL);
  if (size == 0) { buffer[0] = 0; }
  fprintf(stderr, "%s: %s\n", context, buffer);
}

// Opens the specified serial port, configures its timeouts, and sets its
// baud rate.  Returns a handle on success, or INVALID_HANDLE_VALUE on failure.
HANDLE open_serial_port(const char * device, uint32_t baud_rate)
{
  HANDLE port = CreateFileA(device, GENERIC_READ | GENERIC_WRITE, 0, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (port == INVALID_HANDLE_VALUE)
  {
    print_error(device);
    return INVALID_HANDLE_VALUE;
  }

  // Flush away any bytes previously read or written.
  BOOL success = FlushFileBuffers(port);
  if (!success)
  {
    print_error("Failed to flush serial port");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }

  // Configure read and write operations to time out after 100 ms.
  COMMTIMEOUTS timeouts = {0};
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutConstant = 100;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 100;
  timeouts.WriteTotalTimeoutMultiplier = 0;
 
  
  success = SetCommTimeouts(port, &timeouts);
  if (!success)
  {
    print_error("Failed to set serial timeouts");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }

  // Set the baud rate and other options.
  DCB state = {0};
  state.DCBlength = sizeof(DCB);
  state.BaudRate = baud_rate;
  state.ByteSize = 8;
  state.Parity = NOPARITY;
  state.StopBits = ONESTOPBIT;
  success = SetCommState(port, &state);
  if (!success)
  {
    print_error("Failed to set serial settings");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }

  success = FlushFileBuffers(port);
  if (!success)
  {
    print_error("Failed to flush serial port #2");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }

  return port;
}

// Writes bytes to the serial port, returning 0 on success and -1 on failure.
int write_port(HANDLE port, uint8_t * buffer, size_t size)
{
  DWORD written;
  BOOL success = WriteFile(port, buffer, size, &written, NULL);
  if (!success)
  {
    print_error("Failed to write to port");
    return -1;
  }
  if (written != size)
  {
    print_error("Failed to write all bytes to port");
    return -1;
  }
  return 0;
}

// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
SSIZE_T read_port(HANDLE port, char * buffer, size_t size)
{
  DWORD received;
  BOOL success = ReadFile(port, buffer, size, &received, NULL);
  if (!success)
  {
    print_error("Failed to read from port");
    exit(1);
    return -1;
  }
  return received;
}


int main(int argc, char **argv)
{
  // COM ports higher than COM9 need the \\.\ prefix, which is written as
  // "\\\\.\\" in C because we need to escape the backslashes.

  char device[16];
  long pnum;
  long scnt=63;

  if ( argc < 2 || strlen(argv[1]) < 4 ||
       argv[1][0] != 'C' || argv[1][1] != 'O' || argv[1][2] != 'M' ||
      (argv[1][3] < '1' || argv[1][3] > '9')) {

      fprintf(stderr, "Usage: %s COMx [SZx] \n", argv[0]);
      return 1;
  }
  pnum = strtol(&argv[1][3], NULL, 10);

  if (pnum > 9) snprintf(device, sizeof device, "\\\\.\\COM%ld", pnum);
  else snprintf(device, sizeof device, "COM%ld", pnum);

  if ( argc >= 3 && argv[2][0] == 'S' && argv[2][1] == 'Z' ) {
    scnt = strtol(&argv[2][2], NULL, 10);
    if ( scnt < 1 ) scnt=63;
  }

  printf("Port: %s\n", device);

  uint32_t baud_rate = 9600;

  HANDLE port = open_serial_port(device, baud_rate);
  if (port == INVALID_HANDLE_VALUE) { return 1; }

  printf("After start, you'll probably see some garbage. Things will normalize after a few seconds.\n");
  printf("Waiting...");

  unsigned int lines = 0;
  unsigned int pos = 0;
  unsigned int lcnt = 0;

  SSIZE_T r;
  SSIZE_T rmax=0;

  char line[256];
  char buf[2000000];

  memset(buf, 0, sizeof buf);
  memset(line, 0, sizeof line);

  do {
    r = read_port(port, buf, 1);
    //if (r) printf("%c", buf[0]);
  } while(  r < 1 || buf[0] != '\n' );

  printf("Start.\n");
  uint8_t Hello[] = "Hello World - from serialTest\n";  // this message could tell Teensy how many "SZ" bytes to send
  write_port(port, Hello, sizeof(Hello) );

  unsigned int gCnt = 0;
  unsigned int zCnt = 1;
  unsigned int qck = 1;
  unsigned long bcnt=0;
  long unsigned int stats[3] = {0,0,0};
  do {

    r = read_port(port, buf, sizeof buf);
    if ( r > rmax ) rmax = r;
    pos = 0;
    while (r-- && pos < sizeof buf) {
      char ch = buf[pos++];
      if (lcnt < sizeof line) {
        line[lcnt++] = ch;
        bcnt++; // count bytes parsed
      }
      if (ch == '\n') {
        line[lcnt] = '\0';
        if ( qck==1 && lcnt != scnt ) {
          printf("%s", line);
        }
        else if (lcnt != scnt || '1' != line[0] || 'z' != line[30] ) {
          if ( lines == 100000) {
            stats[0]++;
            printf( "." );
            if ( !( gCnt % 50) )
              printf("\nLines-Delta: %d. Received: %s", lines, line);
            if ( !( gCnt % 200) ) {
              printf( "\tstats: 100K=%lu less=%lu repeated %lu rmax=%I64d\n", stats[0], stats[1], stats[2], rmax );
              rmax=0;
            }
            gCnt++;
          }
          else {
            if ( 0 != zCnt ) {
              printf("\nX_Lines-Delta: %d. Received: %s bytes: %lu", lines, line, bcnt);
              if ( 0!= stats[0] )
                stats[1]++;
            }
            else {
              if ( 0!= stats[0] )
                stats[2]++;
              printf( ".X_%lu", bcnt );
              SSIZE_T rr=0;
              do {
                r = read_port(port, buf, sizeof buf);
                rr+=r;
              } while ( r >1800000 );
              printf( "\tBufdump of %I64u\n", rr );
              lines++; // stop cascade
            }
            zCnt = lines;
          }
          lines = 0;
        }
        else lines++;
        lcnt = 0;
        bcnt = 0;
      }
    }

  } while (1);


  CloseHandle(port);
  return 0;
}
