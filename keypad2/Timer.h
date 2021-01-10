#ifndef _TIMER_H
#define _TIMER_H

#include "ArducamSSD1306.h"

class Timer {
    protected:
        char text[5];
        unsigned long lastDraw = 0;
        int x;
        int y;
        int size;
        int drawTimer(ArducamSSD1306& display, const char* timeText, int cursor);
    public:
        Timer(int x, int y, int size);
        int needsRefresh();
        void setTime (const char* time) {strcpy(text, time);};
        const char* getTime () {return text;};
};

class CountdownTimer : public Timer {
  protected:
    int seconds = 0;
    int minutes = 0;
    unsigned long startTime = 0;
  public:
    CountdownTimer(int x, int y, int size);
    int startTimer();
    int resetTimer();
    int remaining();
    int needsRefresh();
    int drawTimer(ArducamSSD1306& display);
};

class InputTimer : public Timer {
  protected:
    int cursor = 0;
  public:
    InputTimer(int x, int y, int size);
    void setTextChar (char c) {if ((cursor >= 0) && (cursor <= 3)) text[cursor] = c;};
    void setCursor (int c) {cursor = c;};
    int getCursor() {return cursor;};
    int needsRefresh();
    int drawTimer(ArducamSSD1306& display);
};


#endif
