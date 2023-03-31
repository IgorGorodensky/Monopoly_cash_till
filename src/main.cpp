#include <Arduino.h>
#include "UserKeys.h"
#include "MoneyManager.h"
#include "DisplayManager.h"
#include "SoundManager.h"
bool rfidFlag = false;
bool flag = false;
uint32_t btnTimer = 0;
void setup()
{
  Serial.begin(9600);                   // Инициализация Serial
  MoneyManager::Instance().initRfId();  // Инициализация RFID
  DisplayManager::Instance().initLCD(); // Инициализация LCD
  initCustomKeyPad();
}
void loop()
{

  if (millis() - btnTimer > 100)
  {
    char key = getCustomKey();
    if (key == '#')
    {
      MoneyManager::Instance().changeRfIdMode();
    }
    if (key == 'B')
    {
      SoundManager::Instance().mute();
      if (SoundManager::Instance().getMuteStatus())
        DisplayManager::Instance().printMuted();
      else
        DisplayManager::Instance().printUnmuted();
    }
     if (key == 'D')
    {
       MoneyManager::Instance().setEraseMode();
    }
    btnTimer = millis();
  }
  MoneyManager::Instance().checkCard();
  // перезапуск датчика RFID
  static uint32_t rebootTimer = millis(); // Важный костыль против зависания модуля!
  if (millis() - rebootTimer >= 1000)
  {
    //  Таймер с периодом 1000 мс
    rebootTimer = millis(); // Обновляем таймер
    MoneyManager::Instance().rebootRfId();
  }
}
