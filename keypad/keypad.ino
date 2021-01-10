#include "Timer.h"
#include "ArducamSSD1306.h"

ArducamSSD1306 oled(0);
Timer timer(38, 0, 2, 0);
Timer countdown(23, 32, 3, 4);

void draw () {
  oled.clearDisplay();
  timer.drawTimer(oled);
  countdown.drawTimer(oled);
  oled.display();    
}

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  oled.begin();
  timer.setTime("0100");
  countdown.setTime("0100");
  draw();
}

char mapping[4][4] = {{'1', '2', '3', '\n'},
                      {'4', '5', '6', '^'},
                      {'7', '8', '9', 'v'},
                      {'c', '0', '<', '>'}};
char state[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}


void processInput (char digit) {
  int cursor = timer.getCursor();
  if (digit >= '0' && digit <= '9') {
    if (cursor == 2) {
      if (digit > '5') {
        return;
      }
    }
    if (cursor < 4) {
      timer.setTextChar(digit);
      timer.setCursor(cursor + 1);
    }
  } else if (digit == 'c') {
    if (countdown.remaining() == -1) {
      timer.setTime("0100");
      timer.setCursor(0);
    }
    countdown.setTime(timer.getTime());
    countdown.resetTimer();
  } else if (digit == '<') {
    if (cursor > 0) {
      timer.setCursor(cursor- 1);
    }
  } else if (digit == '>') {
    if (cursor < 4) {
      timer.setCursor(cursor + 1);
    }
  } else if (digit == '\n') {
    timer.setCursor(4);
    countdown.setTime(timer.getTime());
    countdown.startTimer();
  }
  draw();
}

void scan() {
  int out;
  int in;
  for (out = 2; out <= 5; out++) {
    for (in = 6; in <= 9; in ++) {
      digitalWrite(out, HIGH);
      if (digitalRead(in) == HIGH) {
        state[in-6][out-2] += 1;
        if ((state[in-6][out-2] == 1) || (state[in-6][out-2] > 30)) {
          char digit = mapping[in - 6][out-2];
          Serial.print(digit);
          processInput(digit);
          if (digit == '\n') {
            buzz(10, 4000, 25);
          } else {
            buzz(10, 3000, 5);
            buzz(10, 5000, 5);
          }
        }
        digitalWrite(in, LOW);
      } else {
        if (state[in-6][out-2]) {
          state[in-6][out-2] = 0;
        }
      }
      digitalWrite(out, LOW);
    }
  }
}

void loop() {
  scan();
  delay(10);
  if (countdown.needsRefresh()) {
    draw();
    int r = countdown.remaining();
    if (r == 10) {
      buzz (10, 2000, 50);
    } else if (r < 10) {
      buzz (10, 2000, 2);
      for (int ix = 0; ix < 10 - r; ix ++) {
        delay(5);
        buzz (10, 3500 + ix * 100, 2);
      }
    }
  }
  if (countdown.remaining() == 0) {
    countdown.resetTimer();
    for (int ix = 0; ix < 10; ix++) {
      buzz(10, 4000, 50);
      buzz(10, 3000, 50);
    }
  }
}
