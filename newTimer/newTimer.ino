#include "Adafruit_SH1106.h"

Adafruit_SH1106 disp(0);

#define PIN_BUZZER 9
#define PIN_TOPRIGHT 10
#define PIN_TOPLEFT 11
#define PIN_SIDE 12

#define UI_TYPE 0
#define UI_SET 1
#define UI_RUN 2

#define MODE_COUNTDOWN 0
#define MODE_CHESS 1

typedef struct {
  long remaining;
  long lastDraw;
} timer;

char pinState[13];
char uiState = UI_RUN;
char clockMode = MODE_COUNTDOWN;
char activeClock = 0;
char editDigit = 0;
bool running = false;
bool clockReset = true;

timer clock[2];
long setClock = 60;
long dispClock = 0;
int alertCount = 0;

void setup() {
  // put your setup code here, to run once:
  disp.begin();
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TOPRIGHT, INPUT);
  pinMode(PIN_TOPLEFT, INPUT);
  pinMode(PIN_SIDE, INPUT);
  pinState[PIN_TOPRIGHT] = pinState[PIN_TOPLEFT] = pinState[PIN_SIDE] = 0;
  buzz(PIN_BUZZER, 1000, 10);
  buzz(PIN_BUZZER, 4000, 10);
  delay(20);
  buzz(PIN_BUZZER, 1000, 10);
  buzz(PIN_BUZZER, 4000, 10);
  clock[0].remaining = clock[1].remaining = setClock * 1000;
  render();
}

void buzz(int targetPin, long frequency, long length) {
//  return;
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

int scanButton (int button) {
  if (digitalRead(button) == HIGH) {
    if (pinState[button] == 0) {
      pinState[button] = 1;
      return 1;
    }
  } else {
    pinState[button] = 0;
  }
  return 0;
}

void renderClock (int value, int x, int y, int size) {
  int s = value%60;
  int m = ((value - s)/60) % 100;
  char buffer[6];
  sprintf(buffer, "%02d:%02d", m, s);
  drawString(buffer, x, y, size);
}

void renderEdit (int digit, int x, int y, int size) {
  char buffer[6];
  strcpy(buffer, "     ");
  if (digit < 2) {
    buffer[digit] = '_';
  } else {
    buffer[digit + 1] = '_';
  }
  drawString(buffer, x, y, size);
}

void drawString(const char* text, int x, int y, int size) {
  if (x < 0) {
    int stringWidth = stringWidth = strlen(text) * 6 * size;
    for (; size > 0 && stringWidth > 128; size--) {
      stringWidth = strlen(text) * 6 * size;
    }
    x = (128 - stringWidth)/2;
  }
  for (int ix = 0; ix < strlen(text); ix ++) {
    disp.drawChar(x + (ix * 6 * size), y, text[ix], 1, 0, size);
  }
}

void dumpClock() {
  Serial.print("Active Clock: ");
  Serial.print(clock[activeClock].remaining);
  Serial.print("/");
  Serial.print(clock[activeClock].lastDraw);
  Serial.print("\n");
}

void render() {
  disp.clearDisplay();
  //dumpClock();
  switch (uiState) {
    case UI_TYPE:
      drawString("Mode", -1, 0, 2);
      if (clockMode == MODE_COUNTDOWN) {
        drawString("Countdown", -1, 22, 2);
      } else {
        drawString("Chess", -1, 22, 2);
      }
      break;
    case UI_SET:
      drawString("Set", -1, 0, 2);
      renderEdit(editDigit, -1, 27, 3);
      renderClock(setClock, -1, 22, 3);
      break;
    case UI_RUN:
      if (clockMode == MODE_COUNTDOWN) {
        drawString("Countdown", -1, 0, 2);
        renderClock(clock[activeClock].remaining/1000, -1, 22, 3);
     } else {
        drawString("Chess", -1, 0, 2);
        if (running) {
          if (activeClock == 0) {
            drawString ("<", 3, 0, 2);
          } else {
            drawString (">", 110, 0, 2);
          }
        }
        renderClock(clock[activeClock].remaining/1000, -1, 22, 3);
        renderClock(clock[(activeClock+1)%2].remaining/1000, -1, 50, 2);
      }
      break;
  };
  disp.display();
}

void resetClock (char ix) {
  clock[ix].remaining = setClock * 1000 + 999;
  clock[ix].lastDraw = millis();
  clockReset = true;
  alertCount = 0;
}

int editIncrement[4] = {600, 60, 10, 1};
int editModulus[4] = {6000, 600, 60, 10};

void loop() {
  // put your main code here, to run repeatedly:
  if (scanButton(PIN_TOPRIGHT)) {
    buzz(PIN_BUZZER, 4000, 10);
    if (uiState == UI_TYPE) {
      clockMode = (clockMode + 1) % 2;
    }
    if (uiState == UI_RUN) {
      if (clockMode == MODE_CHESS) {
        activeClock = 0;
      } else {
        if (running || (clock[activeClock].remaining == 0)) {
          resetClock(activeClock);
        }
      }
      clock[activeClock].lastDraw = millis();
      running = true;
      clockReset = false;
    }
    if (uiState == UI_SET) {
      int before = setClock - (setClock % editModulus[editDigit]);
      setClock += editIncrement[editDigit];
      if (setClock - (setClock % editModulus[editDigit]) != before) {
        setClock -= editModulus[editDigit];
      }
      resetClock(0);
      resetClock(1);
    }
    render();
  }
  if (scanButton(PIN_TOPLEFT)) {
    if (uiState == UI_TYPE) {
      clockMode = (clockMode + 1) % 2;
    }
    if (uiState == UI_RUN) {
      if (clockMode == MODE_CHESS) {
        activeClock = 1;
      } else {
        if (running || (clock[activeClock].remaining == 0)) {
          resetClock(activeClock);
        }
      }
      clock[activeClock].lastDraw = millis();
      running = true;
      clockReset = false;
    }
    if (uiState == UI_SET) {
      editDigit = (editDigit + 1) % 4;
    }
    buzz(PIN_BUZZER, 4000, 10);
    render();
  }
  if (scanButton(PIN_SIDE)) {
    if (running) {
      running = false;
    } else {
      if (clockReset) {
        uiState = (uiState + 1) % 3;
      } else {
        clockReset = true;
        resetClock(0);
        resetClock(1);        
      }
    }
    buzz(PIN_BUZZER, 2000, 10);
    render();
  }
  if (running) {
    //dumpClock();
    long now = millis();
    clock[activeClock].remaining -= now - clock[activeClock].lastDraw;
    clock[activeClock].lastDraw = now;
    int r = clock[activeClock].remaining / 1000;
    if (r != dispClock) {
      render();
      if ((r < 11) && (r > 0)) {
        buzz (PIN_BUZZER, 2000, 2);
      }
      dispClock = r;
    }
    if (clock[activeClock].remaining < 1000) {
      clock[activeClock].remaining = 0;
      running = false;
      alertCount = 10;
    }
    //dumpClock();
  }
  if (alertCount > 0) {
    buzz(PIN_BUZZER, 4000, 50);
    buzz(PIN_BUZZER, 3000, 50);
    alertCount -= 1; 
  } else {
    delay(20);
  }
}
