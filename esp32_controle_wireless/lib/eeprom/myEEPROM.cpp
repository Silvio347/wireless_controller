#include <myEEPROM.h>

EEPROMManager eepromManager; // global instance of EEPROMManager

EEPROMManager::EEPROMManager() {} // Constructor

// Setup EEPROM
void EEPROMManager::setupEEPROM()
{
    if (!EEPROM.begin(LENGHT_EEPROM))
        ESP.restart();
}

// Save string in EEPROM
void EEPROMManager::saveStringEEPROM(int enderecoInicio, String str, int maxLength)
{
    int tamanho = str.length();

    if (enderecoInicio + maxLength > LENGHT_EEPROM)
        return;

    for (int i = 0; i < maxLength; i++)
    {
        if (i < tamanho)
        {
            EEPROM.write(enderecoInicio + i, str[i]);
        }
        else
        {
            EEPROM.write(enderecoInicio + i, '\0');
        }
    }
    EEPROM.commit();
}

// Read string from EEPROM
String EEPROMManager::readStringEEPROM(int enderecoInicio, int maxLength)
{
    String str = "";

    for (int i = 0; i < maxLength; i++)
    {
        char c = EEPROM.read(enderecoInicio + i);
        if (c == '\0')
        {
            break;
        }
        str += c;
    }
    return str;
}

// Save float in EEPROM
void EEPROMManager::saveFloatEEPROM(int enderecoInicio, float valor)
{
    EEPROM.put(enderecoInicio, valor);
    EEPROM.commit();
}

// Read float from EEPROM
float EEPROMManager::readFloatEEPROM(int enderecoInicio)
{
    float valor;
    EEPROM.get(enderecoInicio, valor);
    return valor;
}

// Save parameters in EEPROM
void EEPROMManager::saveParamsEEPROM()
{
    saveStringEEPROM(SSID_ADDRESS, ssid, SSID_MAX_LENGTH);
    saveStringEEPROM(PASSWORD_ADDRESS, password, PASSWORD_MAX_LENGTH);
}

// Read parameters from EEPROM0F
void EEPROMManager::readParamsEEPROM()
{
    ssid = readStringEEPROM(SSID_ADDRESS, SSID_MAX_LENGTH);
    password = readStringEEPROM(PASSWORD_ADDRESS, PASSWORD_MAX_LENGTH);
}