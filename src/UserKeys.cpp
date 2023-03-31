#include "UserKeys.h"
char keyArray[ROWS][COLS] = {
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'},
};
 byte colPins[ROWS] = {9, 8, 7, 6};
 byte rowPins[COLS] = {5, 4, 3, 2};
Keypad customKeyPad = Keypad(makeKeymap(keyArray), rowPins, colPins, ROWS, COLS);
char getCustomKey()
{
    return customKeyPad.getKey();
}
void initCustomKeyPad(){
    customKeyPad.setDebounceTime(10);
  //  customKeyPad.setHoldTime(10);
}