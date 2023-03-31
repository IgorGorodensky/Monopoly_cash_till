#ifndef MoneyManager_h
#define MoneyManager_h
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <UserKeys.h>
#include "DisplayManager.h"
#include "SoundManager.h"
#define RST_PIN A0    // Пин rfid модуля RST
#define SS_PIN 10     // Пин rfid модуля SS
#define DATA_BLOCK 10 //
#define TRAILER_KEY 11
class MoneyManager
{
public:
    static MoneyManager &Instance()
    {
        static MoneyManager theSingleInstance;
        return theSingleInstance;
    }

public:
    enum class RFID_MODE : uint8_t
    {
        read = 1,
        add,  //< внести сумму карту
        substract, //< вычесть сумму
        exchange,  //< обмен
        erase //< стереть карту
    };
    RFID_MODE getCurrentRfIdMode() { return mCurrentRfIdMode; };
    void changeRfIdMode();
    void checkCard();
    void rebootRfId();
    void initRfId();
    void setEraseMode();

private:
    MoneyManager()
    {
        displayMode();
    }
    MoneyManager(const MoneyManager &root) = delete;
    MoneyManager &operator=(const MoneyManager &) = delete;

private:
    int32_t mTmp_money_buffer = 0;
    uint32_t mCurrentUid = {0};
    RFID_MODE mCurrentRfIdMode = RFID_MODE::read;
    MFRC522::MIFARE_Key key;    // Объект ключа
    MFRC522::StatusCode status; // Объект статуса
    MFRC522 rfid;
    bool writeRfIdValue(int32_t value);
    int32_t readRfIdValue();
    void eraseRfIdValue();
    void exchageRfIdValue();
    void substractRfIdValue();
    void addRfIdValue();
    void setRfIdMode(const RFID_MODE &mode);
    void displayMode();
    char inputValue();
    uint32_t readCardId();
};
#endif