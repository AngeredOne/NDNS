#pragma once
#include "NDNS.h"

class SettingsField;

const std::string S_VOLUME_IN = "volume-input";
const std::string S_VOLUME_OUT = "volume-output";
const std::string S_THRESHOLD_IN = "threshold-input";
const std::string S_LAST_IP = "last-ip";
const std::string S_LAST_NICKNAME = "last-nickname";
const std::string S_INPUT_DEVICE = "device-input";
const std::string S_OUTPUT_DEVICE = "device-output";
const int SOUND_BUF = 1024;


class Settings
{
public:
    static Settings &Get()
    {
        static Settings instance;
        return instance;
    }
    void SetupFromConsole();
    void LoadConfig();
    void SaveConfig();
    void PrintSettings();
    void SetField(std::string fieldname, std::string value);
    std::shared_ptr<SettingsField> GetField(std::string fieldname);

private:
    std::map<std::string, std::shared_ptr<SettingsField>> allfields;
    const std::string configPath = "ndns.cfg";

    Settings();
    Settings(Settings const &) = delete;
    Settings &operator=(Settings const &) = delete;

    void InitFieldsMap();
};