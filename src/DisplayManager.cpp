#include "DisplayManager.h"
void DisplayManager::initLCD()
{
    lcd.initialize(0x27, 16, 2);
    lcd.init();
    lcd.backlight(); // Включаем подсветку дисплея
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Welcome to");
    lcd.setCursor(3, 1);
    lcd.print("Monopoly!");
    delay(2000);
    printReadValue(0);
}

void DisplayManager::turnOffBackLight()
{
    lcd.noBacklight();
}

void DisplayManager::printReadValue(const uint32_t &value)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Total:");
    lcd.setCursor(0, 1);
    lcd.print(value);
}

void DisplayManager::printSubstractValue(const uint32_t &value)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Put to bank:");
    lcd.setCursor(0, 1);
    lcd.print(value);
}

void DisplayManager::printGetFromBank(const uint32_t &value)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Get from bank:");
    lcd.setCursor(0, 1);
    lcd.print(value);
}

void DisplayManager::printEraseValue()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Warning!");
    lcd.setCursor(2, 1);
    lcd.print("Erasing mode!");
}

void DisplayManager::printErased()
{
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Card erased!");
}

void DisplayManager::printExchangeInput(const uint32_t &value)
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Exchange");
    lcd.setCursor(0, 1);
    lcd.print(value);
}

void DisplayManager::printExchange1()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Exchange");
    lcd.setCursor(2, 1);
    lcd.print("Scan card #1");
}

void DisplayManager::printExchange2()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Exchange");
    lcd.setCursor(2, 1);
    lcd.print("Scan card #2");
}

void DisplayManager::printNoMoney()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("No money!");
}

void DisplayManager::printScanCard()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Scan card");
}

void DisplayManager::printOk()
{
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Completed!");
}

void DisplayManager::printMuted()
{
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Muted!");
    delay(400);
}

void DisplayManager::printUnmuted()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Unmuted!");
    delay(400);
}
