
#define SHIFT_DATA 10
#define SHIFT_CLK 16
#define SHIFT_LATCH 14

#define WRITE_EN 15
#define EEPROM_D0 2
#define EEPROM_D7 9

char cmdbuf[10];
int atInd = 0;

void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, address);
  shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, (address >> 8) | (outputEnable ? 0 : 0x80));

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

byte readEEPROM(int address) {
  for (int pin=EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }

  setAddress(address, true);
  byte data = 0;

  for (int pin=EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }

  return data;
}

void writeEEPROM(int address, byte data) {
  for (int pin=EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  setAddress(address, false);

  for (int pin=EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}

void printContents() {
  for (int base = 0; base < 2048; base += 16) {
    byte data[16];

    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%04x: %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
      base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
      data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    Serial.println(buf);
  }
}

int hexstrToInt(String hexstr) {
  int data = 0;

  for (int i = 0; i < hexstr.length(); i += 1) {
    char c = hexstr.charAt(i);
    int val = 0;

    if (c >= 48 && c <= 57) {
        val = c - 48;
    } else if (c >= 65 && c <= 70) {
        val = c - 55;
    }

    data = (data << 4) | val;
  }

  return data;
}

void clearBuffer() {
  for (int i = 0; i < 10; i += 1) {
    cmdbuf[i] = 0;
  }

  atInd = 0;
}

void processCmd(String cmd) {
  if (cmd.length() != 4 && cmd.length() != 7) {
    return; // Invalid command
  }

  int addr = hexstrToInt(cmd.substring(0, 4));

  // Dump EEPROM contents
  if (addr == 0xFFFF) {
    printContents();
  }

  // Write data
  else if (cmd.length() == 7 && cmd[4] == ':') {
    writeEEPROM(addr, hexstrToInt(cmd.substring(5)));
  }

  // Read data
  else if (cmd.length() == 4) {
    char buf[20];
    sprintf(buf, "%04x:%02x", addr, readEEPROM(addr));
    Serial.println(buf);
  }
}

void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\r' || c == '\n') {
      String cmd = String(cmdbuf);
      cmd.trim();
      cmd.toUpperCase();
      processCmd(cmd);
      clearBuffer();
    } else if (atInd == 9) { // End of buffer. Discard
      clearBuffer();
    } else {
      cmdbuf[atInd] = c;
      atInd += 1;
    }
  }
}

