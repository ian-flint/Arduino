#ifndef _TIMER_H
#define _TIMER_H

#include "ArducamSSD1306.h"

class Timer {
    private:
        char text[5];
        int cursor;
        int seconds = 0;
        int minutes = 0;
        unsigned long startTime = 0;
        unsigned long lastDraw = 0;
        int x;
        int y;
        int size;
        int drawTimer(ArducamSSD1306& display, const char* timeText);
    public:
        Timer(int x, int y, int size, int cursor);
        int startTimer();
        int resetTimer();
        int needsRefresh();
        int drawTimer(ArducamSSD1306& display);
        int remaining();
        void setCursor (int c) {cursor = c;};
        int getCursor() {return cursor;};
        void setTextChar (char c) {if ((cursor >= 0) && (cursor <= 3)) text[cursor] = c;};
        void setTime (const char* time) {strcpy(text, time);};
        const char* getTime () {return text;};
};
#endif
