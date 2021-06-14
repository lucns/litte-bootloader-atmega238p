#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>

#define LED_START_FLASHES 2
//#define F_CPU 998400L
//#define F_CPU 1000000L
#define F_CPU 		8294400L
#define BAUD_RATE   57600
//#define BAUD_RATE   9600

#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN     PINB
#define LED         PINB5

/* STK500 constants list, from AVRDUDE */
#define STK_OK              0x10
#define STK_FAILED          0x11  // Not used
#define STK_UNKNOWN         0x12  // Not used
#define STK_NODEVICE        0x13  // Not used
#define STK_INSYNC          0x14  // ' '
#define STK_NOSYNC          0x15  // Not used
#define ADC_CHANNEL_ERROR   0x16  // Not used
#define ADC_MEASURE_OK      0x17  // Not used
#define PWM_CHANNEL_ERROR   0x18  // Not used
#define PWM_ADJUST_OK       0x19  // Not used
#define CRC_EOP             0x20  // 'SPACE'
#define STK_GET_SYNC        0x30  // '0'
#define STK_GET_SIGN_ON     0x31  // '1'
#define STK_SET_PARAMETER   0x40  // '@'
#define STK_GET_PARAMETER   0x41  // 'A'
#define STK_SET_DEVICE      0x42  // 'B'
#define STK_SET_DEVICE_EXT  0x45  // 'E'
#define STK_ENTER_PROGMODE  0x50  // 'P'
#define STK_LEAVE_PROGMODE  0x51  // 'Q'
#define STK_CHIP_ERASE      0x52  // 'R'
#define STK_CHECK_AUTOINC   0x53  // 'S'
#define STK_LOAD_ADDRESS    0x55  // 'U'
#define STK_UNIVERSAL       0x56  // 'V'
#define STK_PROG_FLASH      0x60  // '`'
#define STK_PROG_DATA       0x61  // 'a'
#define STK_PROG_FUSE       0x62  // 'b'
#define STK_PROG_LOCK       0x63  // 'c'
#define STK_PROG_PAGE       0x64  // 'd'
#define STK_PROG_FUSE_EXT   0x65  // 'e'
#define STK_READ_FLASH      0x70  // 'p'
#define STK_READ_DATA       0x71  // 'q'
#define STK_READ_FUSE       0x72  // 'r'
#define STK_READ_LOCK       0x73  // 's'
#define STK_READ_PAGE       0x74  // 't'
#define STK_READ_SIGN       0x75  // 'u'
#define STK_READ_OSCCAL     0x76  // 'v'
#define STK_READ_FUSE_EXT   0x77  // 'w'
#define STK_READ_OSCCAL_EXT 0x78  // 'x'

/* Watchdog settings */
#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_32MS   (_BV(WDP0) | _BV(WDE))
#define WATCHDOG_64MS   (_BV(WDP1) | _BV(WDE))
#define WATCHDOG_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WATCHDOG_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_4S     (_BV(WDE3) | _BV(WDE))
#define WATCHDOG_8S     (_BV(WDE3) | _BV(WDE0) | _BV(WDE))

/* Function Prototypes */
/* The main function is in init9, which removes the interrupt vector table */
/* we don't need. It is also 'naked', which means the compiler does not    */
/* generate any entry or exit code itself. */
int main(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));
void putch(char);
uint8_t getch(void);
static inline void getNch(uint8_t); /* "static inline" is a compiler hint to reduce code size */
void verifySpace();
static inline void flash_led(uint8_t);
uint8_t getLen();
static inline void watchdogReset();
void watchdogConfig(uint8_t x);
void appStart() __attribute__ ((naked));

/* C zero initialises all global variables. However, that requires */
/* These definitions are NOT zero initialised, but that doesn't matter */
/* This allows us to drop the zero init code, saving us memory */
#define buff    ((uint8_t*)(0x100))
#define address (*(uint16_t*)(0x200))
#define length  (*(uint8_t*)(0x202))
/* main program starts here */
int main(void) {
  // After the zero init loop, this is the first code to run.
  //
  // This code makes the following assumptions:
  //  No interrupts will execute
  //  SP points to RAMEND
  //  r1 contains zero
  //
  // If not, uncomment the following instructions:
  // cli();
  // SP=RAMEND;  // This is done by hardware reset
  // asm volatile ("clr __zero_reg__");

  uint8_t ch;

  TCCR1B = _BV(CS12) | _BV(CS10); // div 1024
  // UART initialize
  UCSR0A = _BV(U2X0); //Double speed mode USART0
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
  UBRR0L = (uint8_t) ((F_CPU / (BAUD_RATE * 8L)) - 1);
  //UBRR0L = 26;

  // Adaboot no-wait mod
  ch = MCUSR;
  MCUSR = 0;
  if (!(ch & _BV(EXTRF))) appStart();

  watchdogConfig(WATCHDOG_1S);
  LED_DDR |= _BV(LED);
  flash_led(LED_START_FLASHES * 2);
  
  for (;;) {
    /* get character from UART */
    ch = getch();

    if(ch == STK_GET_PARAMETER) {
      // GET PARAMETER returns a generic 0x03 reply - enough to keep Avrdude happy
      getNch(1);
      putch(0x03);
    } else if(ch == STK_SET_DEVICE) {
      // SET DEVICE is ignored
      getNch(20);
    } else if(ch == STK_SET_DEVICE_EXT) {
      // SET DEVICE EXT is ignored
      getNch(5);
    } else if(ch == STK_LOAD_ADDRESS) {
      // LOAD ADDRESS
      address = getch();
      address = (address & 0xff) | (getch() << 8);
      address += address; // Convert from word address to byte address
      verifySpace();
    } else if(ch == STK_UNIVERSAL) {
      // UNIVERSAL command is ignored
      getNch(4);
      putch(0x00);
    } else if(ch == STK_PROG_PAGE) {
      // PROGRAM PAGE - we support flash programming only, not EEPROM
      uint8_t *bufPtr;
      uint16_t addrPtr;

      getLen();
      boot_page_erase((uint16_t)(void*)address);
      bufPtr = buff;
      do *bufPtr++ = getch();
      while (--length);
      verifySpace();
      boot_spm_busy_wait();

      // Copy buffer into programming buffer
      bufPtr = buff;
      addrPtr = (uint16_t)(void*)address;
      ch = SPM_PAGESIZE / 2;
      do {
        uint16_t a;
        a = *bufPtr++;
        a |= (*bufPtr++) << 8;
        boot_page_fill((uint16_t)(void*)addrPtr,a);
        addrPtr += 2;
      } while (--ch);
      
      // Write from programming buffer
      boot_page_write((uint16_t)(void*)address);
      boot_spm_busy_wait();
      boot_rww_enable();
    } else if(ch == STK_READ_PAGE) {
      // READ PAGE - we only read flash
      getLen();
      verifySpace();
      do putch(pgm_read_byte_near(address++));
      while (--length);
    } else if(ch == STK_READ_SIGN) {
      // READ SIGN - return what Avrdude wants to hear
      verifySpace();
      putch(SIGNATURE_0);
      putch(SIGNATURE_1);
      putch(SIGNATURE_2);
    } else if (ch == 'Q') {
      // Adaboot no-wait mod
      watchdogConfig(WATCHDOG_16MS);
      verifySpace();
    } else {
      // This covers the response to commands like STK_ENTER_PROGMODE
      verifySpace();
    }
    putch(STK_OK);
  }
}

void putch(char ch) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = ch;
}

uint8_t getch(void) {
  uint8_t ch;
  watchdogReset();
  while(!(UCSR0A & _BV(RXC0)));
  ch = UDR0;
  return ch;
}

void getNch(uint8_t count) {
  do getch(); while (--count);
  verifySpace();
}

void verifySpace() {
  if (getch() != CRC_EOP) appStart();
  putch(STK_INSYNC);
}

void flash_led(uint8_t count) {
  do {
    TCNT1 = -(F_CPU/(1024*16));
    TIFR1 = _BV(TOV1);
    while(!(TIFR1 & _BV(TOV1)));
    LED_PIN |= _BV(LED);
    watchdogReset();
  } while (--count);
}

uint8_t getLen() {
  getch();
  length = getch();
  return getch();
}

void watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

void watchdogConfig(uint8_t x) {
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = x;
}

void appStart() {
  watchdogConfig(WATCHDOG_OFF);
  __asm__ __volatile__ (
    // Jump to RST vector
    "clr r30\n"
    "clr r31\n"
    "ijmp\n"
  );
}
