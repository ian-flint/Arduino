#include "Timer.h"

// Countdown Timer
CountdownTimer::CountdownTimer(int x, int y, int size) : Timer (x, y, size) {
}

int CountdownTimer::startTimer() {
    startTime = millis();
    Serial.print("Start time: ");
    Serial.println(startTime);
    Serial.println(text);
    sscanf(text, "%02d%02d", &minutes, &seconds);
    Serial.println(minutes);
    Serial.println(seconds);
    lastDraw = 0;
    return 0;
}

int CountdownTimer::resetTimer() {
    startTime = 0;
    lastDraw = 0;
    return 0;
}

int CountdownTimer::remaining() {
  if (startTime) {
    long curTime = millis();
    long secondsElapsed = (curTime - startTime)/1000;
    long ts = seconds + minutes * 60;
    if ((ts - secondsElapsed) > 0) {
      return ts - secondsElapsed;
    }
    return 0;
  }
  return -1;
}

int CountdownTimer::needsRefresh() {
  if (startTime) {
    long curTime = millis();
    long secondsElapsed = (curTime - startTime)/1000;
    long lastSecondsElapsed = (lastDraw - startTime)/1000;
    if (secondsElapsed != lastSecondsElapsed) {
      return 1;
    }
  }
  return 0;
}

int CountdownTimer::drawTimer(ArducamSSD1306& display) {
    if (!startTime) {
      Timer::drawTimer(display, text, 4);
    } else {
      long curTime = millis();
      long secondsElapsed = (curTime - startTime)/1000;
      long lastSecondsElapsed = (lastDraw - startTime)/1000;
      Serial.println(secondsElapsed);
      lastDraw = curTime;
      char newTime[5];
      long ts = seconds + minutes * 60;
      long rs = ts - secondsElapsed;
      if (rs <= 0) {
        Timer::drawTimer(display, "0000", 4);
      } else {
        long rm = rs/60;
        rs -= rm * 60;
        sprintf(newTime, "%02d", rm);
        sprintf(newTime+2, "%02d", rs);
        Timer::drawTimer(display, newTime, 4);
      }
    }
    return 0;
}

// Input Timer
InputTimer::InputTimer(int x, int y, int size) : Timer (x, y, size) {
}

int InputTimer::needsRefresh() {
  if (lastDraw) {
    long curTime = millis();
    if ((int)(curTime / 500) == (int)(lastDraw/500)) {
      return 0;
    }
  }
  return 1;
}

int InputTimer::drawTimer(ArducamSSD1306& display) {
    lastDraw = millis();
    long int showCursor = (lastDraw/500)%2;
    if (showCursor) {
      Timer::drawTimer(display, text, cursor);
    } else {
      Timer::drawTimer(display, text, 4);
    }
}

// Timer
Timer::Timer(int x, int y, int size) {
    this->x = x;
    this->y = y;
    this->size = size;
    strcpy(text, "0000");
}

int Timer::drawTimer(ArducamSSD1306& display, const char* timeText, int cursor) {
  display.drawChar(x, y, cursor==0?'_':timeText[0], 1, 0, size);
  display.drawChar(x+6*size, y, cursor==1?'_':timeText[1], 1, 0, size);
  display.drawChar(x+12*size, y, ':', 1, 0, size);
  display.drawChar(x+18*size, y, cursor==2?'_':timeText[2], 1, 0, size);
  display.drawChar(x+24*size, y, cursor==3?'_':timeText[3], 1, 0, size);
}
