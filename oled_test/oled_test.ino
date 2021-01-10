#include "Adafruit_SH1106.h"

Adafruit_SH1106 foo(0);

#define TOPRIGHT 10
#define TOPLEFT 11
#define SIDE 12

void setup() {
  // put your setup code here, to run once:
  foo.begin();
  foo.display();
  delay(1000);
  foo.invertDisplay(1);
  delay(100);
  foo.invertDisplay(0);
  delay(100);
  foo.invertDisplay(1);
  delay(100);
  foo.invertDisplay(0);
  delay(100);
  foo.invertDisplay(1);
  delay(100);
  foo.invertDisplay(0);
  delay(100);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  Serial.begin(9600);
  Serial.write("Hello\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(TOPRIGHT) == HIGH) {
    Serial.write("TOPRIGHT is high\n");
  }
  if (digitalRead(TOPLEFT) == HIGH) {
    Serial.write("TOPLEFT is high\n");
  }
  if (digitalRead(SIDE) == HIGH) {
    Serial.write("SIDE is high\n");
  }
  delay(100);
}
