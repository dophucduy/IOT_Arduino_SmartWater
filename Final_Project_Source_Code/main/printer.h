#include <string.h>
#include <LiquidCrystal_I2C.h>

#ifndef PRINTER_H
#define PRINTER_H


class Printer {
public:
    char ** buffer;
    char ** oldBuffer;
    int x;
    int y;

    Printer();

    ~Printer();

    void clearLine(int line);

    void clear();

    void setCursor(int x, int y);

    void print(String str);

    void updateFrame(LiquidCrystal_I2C lcd);

    boolean bufferIsDifferent(int x, int y);
};

#endif