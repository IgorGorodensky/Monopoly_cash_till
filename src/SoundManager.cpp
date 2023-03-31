#include "SoundManager.h"
void SoundManager::initBuzzer(const int &buzzer_pin)
{
    mBuzzerPin = buzzer_pin;
    pinMode(OUTPUT, A1);
}

void SoundManager::playSuccessful()
{
    if(isMuted) return;
    tone(mBuzzerPin, 3200);
    delay(5);
    noTone(mBuzzerPin);
}

void SoundManager::playFailed()
{
    if(isMuted) return;
    tone(mBuzzerPin, 100);
    delay(5);
    noTone(mBuzzerPin);
}

void SoundManager::playCheck()
{
    if(isMuted) return;
    tone(mBuzzerPin, 1000);
    delay(5);
    noTone(mBuzzerPin);
}

void SoundManager::mute()
{
    isMuted = !isMuted;
}
