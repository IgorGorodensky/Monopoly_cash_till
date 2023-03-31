#include "MoneyManager.h"

void MoneyManager::changeRfIdMode()
{
    uint8_t mode = static_cast<uint8_t>(mCurrentRfIdMode);
    mode++;
    if (mode > static_cast<uint8_t>(RFID_MODE::exchange))
        mode = static_cast<uint8_t>(RFID_MODE::read);
    mCurrentRfIdMode = static_cast<RFID_MODE>(mode);
    if (mCurrentRfIdMode == RFID_MODE::substract)
        substractRfIdValue();
    if (mCurrentRfIdMode == RFID_MODE::add)
        addRfIdValue();
    if (mCurrentRfIdMode == RFID_MODE::exchange)
        exchageRfIdValue();
    displayMode();
}
bool MoneyManager::writeRfIdValue(int32_t value)
{
    /* Аутентификация сектора, указываем блок безопасности #7 и ключ A */
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_KEY, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK)
    {                                 // Если не окэй
        Serial.println("Auth write error"); // Выводим ошибку
        SoundManager::Instance().playFailed();
        return false;
    }
    SoundManager::Instance().playCheck();
    /* Запись блока, указываем блок данных #6 */
    uint8_t dataToWrite[16] = {// Массив на запись в блок
                               0x0, 0x0, 0x0, 0x0,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < static_cast<int>(sizeof(value)); i++)
    {
        dataToWrite[i] = (value & 0xff);
        value >>= 8;
    }
    status = rfid.MIFARE_Write(DATA_BLOCK, dataToWrite, 16); // Пишем массив в блок 6
    if (status != MFRC522::STATUS_OK)
    {                                  // Если не окэй
        Serial.println("Write error"); // Выводим ошибку
        SoundManager::Instance().playFailed();
        return false;
    }
    Serial.println("Write OK"); // Завершаем работу с меткой
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    SoundManager::Instance().playSuccessful();
    return true;
}
int32_t MoneyManager::readRfIdValue()
{
    /* Аутентификация сектора, указываем блок безопасности #7 и ключ A */
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_KEY, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK)
    {                                 // Если не окэй
        Serial.println("Auth read error"); // Выводим ошибку
        SoundManager::Instance().playFailed();
        return 0;
    }
    SoundManager::Instance().playCheck();
    /* Чтение блока, указываем блок данных #6 */
    uint8_t dataBlock[18];                                   // Буфер для чтения
    uint8_t size = sizeof(dataBlock);                        // Размер буфера
    status = rfid.MIFARE_Read(DATA_BLOCK, dataBlock, &size); // Читаем 6 блок в буфер
    if (status != MFRC522::STATUS_OK)
    {                                 // Если не окэй
        Serial.println("Read error"); // Выводим ошибку
        SoundManager::Instance().playFailed();
        return 0;
    }
    int32_t value = 0;
    for (int i = sizeof(value); i >= 0; i--)
    {
        value |= dataBlock[i];
        if (i != 0)
            value <<= 8;
    }
    rfid.PICC_HaltA(); // Завершаем работу с меткой
    rfid.PCD_StopCrypto1();
    SoundManager::Instance().playSuccessful();
    return value;
}
void MoneyManager::eraseRfIdValue()
{
    /* Аутентификация сектора, указываем блок безопасности #7 и ключ A */
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_KEY, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK)
    {                                 // Если не окэй
        Serial.println("Auth error"); // Выводим ошибку
        SoundManager::Instance().playFailed();
        return;
    }
    SoundManager::Instance().playCheck();
    /* Запись блока, указываем блок данных #6 */
    uint8_t dataToWrite[16] = {// Массив на запись в блок
                               0x00, 0x00, 0x0, 0x0,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};
    status = rfid.MIFARE_Write(DATA_BLOCK, dataToWrite, 16); // Пишем массив в блок 6
    if (status != MFRC522::STATUS_OK)
    {                                  // Если не окэй
        Serial.println("Erase error"); // Выводим ошибку
        SoundManager::Instance().playFailed();
        return;
    }
    //mCurrentRfIdMode = RFID_MODE::read;
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    SoundManager::Instance().playSuccessful();
    DisplayManager::Instance().printErased();
    delay(500);
    DisplayManager::Instance().printEraseValue();
}
void MoneyManager::exchageRfIdValue()
{
    mTmp_money_buffer = 0;
    DisplayManager::Instance().printExchangeInput(mTmp_money_buffer);
    while (1)
    {
        char key = inputValue();
        if (key == '#')
            return;
        if (key)
            DisplayManager::Instance().printExchangeInput(mTmp_money_buffer);
        // кнопка получения
        if (key == '*')
            break;
    }

    DisplayManager::Instance().printExchange1();
    int32_t value = 0;
    // цикл чтения
    while (1)
    {
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
        {
            // SoundManager::Instance().playCheck();
            mCurrentUid = readCardId();
            value = readRfIdValue();
            // вычитание из 1 карты
            if (value >= mTmp_money_buffer)
                value -= mTmp_money_buffer;
             else
            {
                eraseRfIdValue();
                // банкрот
                // зачисление на вторую карту
                DisplayManager::Instance().printExchange2();
                while (1)
                {
                    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                    {
                        SoundManager::Instance().playCheck();
                        if (writeRfIdValue(value))
                        {
                            mCurrentRfIdMode = RFID_MODE::read;
                            DisplayManager::Instance().printOk();
                            SoundManager::Instance().playSuccessful();
                            return;
                        }
                    }
                } // конец зачисления на вторую карту
            }
            SoundManager::Instance().playCheck();
            while (1)
            {
                if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                {
                    if (writeRfIdValue(value))
                    {
                        SoundManager::Instance().playSuccessful();
                        break;
                    }
                }
            }
            break;
        }
    } // конец вычитания из первой карты
    DisplayManager::Instance().printExchange2();
    // зачисление на вторую карту
    while (1)
    {
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
        {
            auto value = readRfIdValue();
            if (readCardId() != mCurrentUid)
            {
                while (1)
                {
                    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                        if (writeRfIdValue(mTmp_money_buffer + value))
                        {
                            mCurrentRfIdMode = RFID_MODE::read;
                            DisplayManager::Instance().printOk();
                            SoundManager::Instance().playSuccessful();
                            return;
                        }
                }
            }
        }
    } // конец зачисления на вторую карту
}

void MoneyManager::substractRfIdValue()
{

    mTmp_money_buffer = 0;
    DisplayManager::Instance().printSubstractValue(mTmp_money_buffer);
    while (1)
    {
        char key = inputValue();
        if (key == '#')
            return;
        if (key)
            DisplayManager::Instance().printSubstractValue(mTmp_money_buffer);
        // кнопка получения
        if (key == '*')
        {
            int32_t value = 0;
            DisplayManager::Instance().printScanCard();
            // цикл чтения
            while (1)
            {
                if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                {
                    // SoundManager::Instance().playCheck();
                    value = readRfIdValue();
                    // прибавка денег
                    value -= mTmp_money_buffer;
                    while (1)
                    {
                        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                        {
                            SoundManager::Instance().playCheck();
                            if (writeRfIdValue(value))
                            {
                                mCurrentRfIdMode = RFID_MODE::read;
                                DisplayManager::Instance().printOk();
                                SoundManager::Instance().playSuccessful();
                                delay(500);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void MoneyManager::addRfIdValue()
{
    mTmp_money_buffer = 0;
    DisplayManager::Instance().printGetFromBank(mTmp_money_buffer);
    while (1)
    {
        char key = inputValue();
        if (key == '#')
            return;
        if (key)
            DisplayManager::Instance().printGetFromBank(mTmp_money_buffer);
        // кнопка получения
        if (key == '*')
        {
            int32_t value = 0;
            DisplayManager::Instance().printScanCard();
            // цикл чтения
            while (1)
            {
                if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                {
                    // SoundManager::Instance().playCheck();
                    value = readRfIdValue();
                    // прибавка денег
                    value += mTmp_money_buffer;
                    while (1)
                    {
                        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
                        {
                            //   SoundManager::Instance().playCheck();
                            if (writeRfIdValue(value))
                            {
                                mCurrentRfIdMode = RFID_MODE::read;
                                DisplayManager::Instance().printOk();
                                SoundManager::Instance().playSuccessful();
                                delay(500);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void MoneyManager::setRfIdMode(const RFID_MODE &mode)
{
    mCurrentRfIdMode = mode;
}

void MoneyManager::displayMode()
{
    switch (mCurrentRfIdMode)
    {
    case RFID_MODE::read:
        DisplayManager::Instance().printReadValue(0);
        break;
    case RFID_MODE::add:
        DisplayManager::Instance().printGetFromBank(0);
        break;
    case RFID_MODE::substract:
        DisplayManager::Instance().printSubstractValue(0);
        break;
    case RFID_MODE::erase:
        DisplayManager::Instance().printEraseValue();
        break;
    // case RFID_MODE::exchange:
    //     DisplayManager::Instance().printExchange1();
    //     break;
    default:
        break;
    }
}
char MoneyManager::inputValue()
{
    char key = getCustomKey();
    if (key == '#')
    {
        changeRfIdMode();
        mTmp_money_buffer = 0;
        return key;
    }
    if (key == 'C')
    {
        mTmp_money_buffer = 0;
        return key;
    }
    if (key && (key != 'A') && (key != 'B') && (key != '*') && (key != 'D'))
    {
        char arr[1] = {0};
        arr[0] = key;
        mTmp_money_buffer *= 10;
        mTmp_money_buffer += static_cast<int32_t>(atoi(arr));
        return key;
    }
    return key;
}
uint32_t MoneyManager::readCardId()
{
    uint32_t id = 0;
    for (byte i = 0; i < rfid.uid.size; i++)
    {
        id |= rfid.uid.uidByte[i];
        if (i != 0)
            id <<= 8;
    }
    return id;
}
void MoneyManager::checkCard()
{

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        switch (mCurrentRfIdMode)
        {
        case MoneyManager::RFID_MODE::read:
        {
            auto value = readRfIdValue();
            DisplayManager::Instance().printReadValue(value);
            break;
        }
        case MoneyManager::RFID_MODE::erase:
            eraseRfIdValue();
            break;
        default:
            break;
        }
    }
}
void MoneyManager::rebootRfId()
{
    digitalWrite(RST_PIN, HIGH); // Сбрасываем модуль
    delayMicroseconds(2);        // Ждем 2 мкс
    digitalWrite(RST_PIN, LOW);  // Отпускаем сброс
    rfid.PCD_Init();             // Инициализируем заного
}
void MoneyManager::initRfId()
{
    SPI.begin();
    rfid.PCD_Init(SS_PIN, RST_PIN);
    rfid.PCD_SetAntennaGain(rfid.RxGain_max); // Установка усиления антенны
    rfid.PCD_AntennaOff();                    // Перезагружаем антенну
    rfid.PCD_AntennaOn();                     // Включаем антенну
    for (byte i = 0; i < 6; i++)
    {                          // Наполняем ключ
        key.keyByte[i] = 0xFF; // Ключ по умолчанию 0xFFFFFFFFFFFF
    }
    Serial.println("RfId set up!");
}

void MoneyManager::setEraseMode()
{
    if(mCurrentRfIdMode == RFID_MODE::erase){
        mCurrentRfIdMode = RFID_MODE::read;
    }
    else
    {
        if (mCurrentRfIdMode != RFID_MODE::exchange)
            mCurrentRfIdMode = RFID_MODE::erase;
    }
    displayMode();
}
