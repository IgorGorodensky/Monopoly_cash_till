#ifndef SoundManager_h
#define SoundManager_h
#include <Arduino.h>
class SoundManager
{
public:
    static SoundManager &Instance()
    {
        static SoundManager theSingleInstance;
        return theSingleInstance;
    }

public:
    void initBuzzer(const int &buzzer_pin = A1);
    void playSuccessful();
    void playFailed();
    void playCheck();
    void mute();
    inline bool getMuteStatus()const{return isMuted;};
private:
    SoundManager()
    {
        initBuzzer();
    }
    SoundManager(const SoundManager &root) = delete;
    SoundManager &operator=(const SoundManager &) = delete;
    private:
    int mBuzzerPin = A1;
    bool isMuted = true;
};
#endif