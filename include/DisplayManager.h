#ifndef DisplayManager_h
#define DisplayManager_h
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager
{
public:
    static DisplayManager &Instance()
    {
        static DisplayManager theSingleInstance;
        return theSingleInstance;
    }

public:
    void initLCD();
    void turnOffBackLight();
    void printReadValue(const uint32_t &value);
    void printSubstractValue(const uint32_t &value);
    void printGetFromBank(const uint32_t &value);
    void printEraseValue();
    void printErased();
    void printExchangeInput(const uint32_t &value);
    void printExchange1();
    void printExchange2();
    void printNoMoney();
    void printScanCard();
    void printOk();
    void printMuted();
    void printUnmuted();
    inline void clear(){lcd.clear();};
private:
    DisplayManager()
    {
    }
    DisplayManager(const DisplayManager &root) = delete;
    DisplayManager &operator=(const DisplayManager &) = delete;
    LiquidCrystal_I2C lcd;
};
#endif