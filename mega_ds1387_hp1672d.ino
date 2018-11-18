// Adapted from https://www.eevblog.com/forum/testgear/hp-167xd-corrupted-lan-board-error-fix/ by Greg "GothAck" Miell 2018

// Write empty DS1387 NVRAM to fix corrupted LAN data due to DS1387 missing battery

// NVRAM 4K RAM content
// First 6 bytes = MAC Address. In this example MAC is 010203040506 
// 01 02 03 04 05 06 C0 00 02 E6 00 00 00 00 00 00
// 00 00 01 00 41 6E 61 6C 79 7A 65 72 20 31 00 00
// 00 00 00 00 00 00 00 00 00 00 00 00 00 2D 01 00
// 00 00 00 00 00 00 00 00 00 00
// MAC Address is printed in a label indicated as LLA

#define OER 8
#define AS0 9
#define AS1 10
#define WER 11

// Number of bytes to write
#define BYTES 58
byte nvram[BYTES] = { 0x00, 0x60, 0xB0, 0x18, 0xB4, 0x49, 0xC0, 0x00, 0x02, 0xE6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x01, 0x00, 0x41, 0x6E, 0x61, 0x6C, 0x79, 0x7A, 0x65, 0x72, 0x20, 0x31, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x01, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Arduino MEGA A0 == 54 -> A7 == 61
#define DATA_PIN_BASE 54

#define PORTF_FOR for(uint8_t i = 0; i < 8; i++) 

#define PORTF_MODE(mode) PORTF_FOR pinMode(DATA_PIN_BASE + i, mode)
#define PORTF_WRITE(value) PORTF_FOR digitalWrite(DATA_PIN_BASE + i, value & (1 << i))
#define PORTF_READBIT(bit) (digitalRead(DATA_PIN_BASE + bit) << bit)
#define PORTF_READ() PORTF_READBIT(0) | PORTF_READBIT(1) | PORTF_READBIT(2) | PORTF_READBIT(3) | PORTF_READBIT(4) | PORTF_READBIT(5) | PORTF_READBIT(6) | PORTF_READBIT(7)

void setup() {
  Serial.begin(9600);
  delay(5000);
  // Define control pins as output
  pinMode(OER, OUTPUT);
  pinMode(AS0, OUTPUT);
  pinMode(AS1, OUTPUT);
  pinMode(WER, OUTPUT);

  // To start, define AD0-AD7 pins as OUTPUT
  // AD0-7 = PORTF [0-7]
  PORTF_MODE(OUTPUT);

  //set control bits as HIGH
  digitalWrite(OER, HIGH);
  digitalWrite(AS0, HIGH);
  digitalWrite(AS1, HIGH);
  digitalWrite(WER, HIGH);
}

void loop() {
  if (Serial.available()) {
    switch(Serial.read()) {
      case 'w':
        Serial.println("Write");
        write();
        break;
       case 'r':
        Serial.println("Read");
        read();
        break;
    }
  }
}

void write() {
  // Write first 58 bytes
  int addr;
  byte data;
  byte addrlow;
  byte addrhigh;
  PORTF_MODE(OUTPUT);

  for (addr=0; addr<BYTES; addr++) {
    addrhigh = addr>>8;
    addrlow = (addr&0x00FF);

    // send ADDR LOW to DS1387
    PORTF_WRITE(addrlow);
    delayMicroseconds(1);
    digitalWrite(AS0, LOW);
    delayMicroseconds(1);
    digitalWrite(AS0, HIGH);
    delayMicroseconds(1);

    // send ADDR HIGH to DS1387
    PORTF_WRITE(addrhigh);
    delayMicroseconds(1);
    digitalWrite(AS1, LOW);
    delayMicroseconds(1);
    digitalWrite(AS1, HIGH);
    delayMicroseconds(1);

    // Write byte
    PORTF_WRITE(nvram[addr]);
    // pulse WER (Write Enable 4K RAM)
//    PORTF_MODE(INPUT);
    delayMicroseconds(1);
    digitalWrite(WER, LOW);
    delayMicroseconds(1);
    Serial.print(PORTF, HEX);
    Serial.print(" ");
//    Serial.println(PORTF, HEX);
    digitalWrite(WER, HIGH);
//    PORTF_MODE(OUTPUT);
    delayMicroseconds(1);
  }
  Serial.println();
}

void read() {
  // Write first 58 bytes
  int addr;
  byte data;
  byte addrlow;
  byte addrhigh;
  bool valid = true;

  for (addr=0; addr<BYTES; addr++) {
    PORTF_MODE(OUTPUT);
    addrhigh = addr>>8;
    addrlow = (addr&0x00FF);

    // send ADDR LOW to DS1387
    PORTF_WRITE(addrlow);
//    PORTB &= ~(_BV(1));
//    PORTB |= _BV(1);
    digitalWrite(AS0, LOW);
    delayMicroseconds(1);
    digitalWrite(AS0, HIGH);
    delayMicroseconds(1);

    // send ADDR HIGH to DS1387
    PORTF_WRITE(addrhigh);
//    PORTB &= ~(_BV(2));
//    PORTB |= _BV(2);
    digitalWrite(AS1, LOW);
    delayMicroseconds(1);
    digitalWrite(AS1, HIGH);
    delayMicroseconds(1);
    PORTF_WRITE(0);

    // Write byte
    //PORTF_WRITE(nvram[addr]);
    // pulse WER (Write Enable 4K RAM)
//    PORTB &= ~(_BV(3));
    PORTF_MODE(INPUT);
    delayMicroseconds(1);
    digitalWrite(OER, LOW);
    delayMicroseconds(1);
    uint8_t value = PORTF_READ();
    if(value != nvram[addr]) {
     Serial.print("Failed addr ");
     Serial.println(addr, HEX);
     valid = false;
    } else {
      Serial.print(". ");
    }
    PORTF_MODE(OUTPUT);
    digitalWrite(OER, HIGH);
//    PORTB |= _BV(3);
    delayMicroseconds(1);
  }
  Serial.println();
  if (valid) {
    Serial.println("RAM Valid!");
  } else {
    Serial.println("ERROR VAILDATING RAM!");
  }
}
