/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

unsigned char pins[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A0
};

void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(A0, INPUT);
  
  Serial.begin(9600);
}

unsigned int lastReading = 0;

void loop() {
  // read the state of the switch into a local variable:
  unsigned int reading =
    (digitalRead(2)?0:1) +
    (digitalRead(3)?0:2) +
    (digitalRead(4)?0:4) +
    (digitalRead(5)?0:8) +
    (digitalRead(6)?0:16) +
    (digitalRead(7)?0:32) +
    (digitalRead(8)?0:64) +
    (digitalRead(9)?0:256) +
    (digitalRead(10)?0:512) +
    (digitalRead(11)?0:1024) +
    (digitalRead(12)?0:2048) +
    (analogRead(A0)>512?0:4096) +
    32768; // For byte alignment
  
  if (reading != lastReading) {
    Serial.write((const uint8_t *) &reading, sizeof(reading));
    lastReading = reading;
  }
}

