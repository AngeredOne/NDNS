#include "Settings.h"
#include "SDLAudioManager.h"
#include "SettingsFields.h"
#include "NDNS.h"
#include "Definitions.h"

Settings::Settings()
{
    InitFieldsMap();
    LoadConfig();
}

void Settings::SetupFromConsole()
{
    NDNS::Get().WriteOutput("Select devices\n", SERVER);

    NDNS::Get().WriteOutput("OUTPUT\n", SERVER);
    for (int i = 0; i < SDL_GetNumAudioDevices(0); ++i)
    {
        std::string temp = std::string(std::to_string(i) + " : ");
        temp += SDL_GetAudioDeviceName(i, 0);
        NDNS::Get().WriteOutput(temp, SERVER);
    }

    NDNS::Get().WriteOutput("Enter option.\n", SERVER);

    int index;
    std::string input = NDNS::Get().GetInput();
    index = atoi(input.c_str());
    auto outputDeviceName = SDL_GetAudioDeviceName(index, 0);
    Settings::Get().SetField(S_OUTPUT_DEVICE, outputDeviceName);
    std::cout << std::endl;

    NDNS::Get().WriteOutput("INPUT\n", SERVER);
    for (int i = 0; i < SDL_GetNumAudioDevices(1); ++i)
    {
        std::string temp = std::string(std::to_string(i) + " :");
        temp += SDL_GetAudioDeviceName(i, 1);
        NDNS::Get().WriteOutput(temp, SERVER);
    }

    NDNS::Get().WriteOutput("Enter option.\n", SERVER);

    input = NDNS::Get().GetInput();
    index = atoi(input.c_str());
    auto inputDeviceName = SDL_GetAudioDeviceName(index, 1);
    Settings::Get().SetField(S_INPUT_DEVICE, inputDeviceName);

    std::cout << std::endl;
}

void Settings::LoadConfig()
{
    if (fs::exists(configPath))
    {

        std::ifstream file;
        file.open(configPath);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                auto d = NDNS::Get().Split(line, ":");
                if (d.size() == 2)
                {
                    auto i = d.begin();
                    auto name = *i;
                    auto value = *(++i);
                    SetField(name, value);
                }
            }
        }
        file.close();
    }
    else
    {
        SaveConfig();
    }
}

void Settings::SaveConfig()
{
    std::ofstream file;
    file.open(configPath, std::ios::trunc);
    for (auto field : allfields)
    {
        file << field.first << ":" << field.second->GetValue() << "\n";
    }
    file.close();
}

void Settings::SetField(std::string fieldname, std::string value)
{
    if (auto field = allfields.find(fieldname); field != allfields.end())
    {
        field->second->ApplyValue(value);
    }
}

std::shared_ptr<SettingsField> Settings::GetField(std::string fieldname)
{
    if (auto field = allfields.find(fieldname); field != allfields.end())
    {
        return field->second;
    }
    return nullptr;
}
void Settings::InitFieldsMap()
{
    allfields.emplace(FieldPair(S_VOLUME_IN, std::make_shared<VolumeControl>()));
    allfields.emplace(FieldPair(S_VOLUME_OUT, std::make_shared<VolumeControl>()));
    allfields.emplace(FieldPair(S_THRESHOLD_IN, std::make_shared<ThresholdControl>()));
    allfields.emplace(FieldPair(S_LAST_IP, std::make_shared<LastIP>()));
    allfields.emplace(FieldPair(S_LAST_NICKNAME, std::make_shared<LastNickname>()));
    allfields.emplace(FieldPair(S_INPUT_DEVICE, std::make_shared<Device>(true)));
    allfields.emplace(FieldPair(S_OUTPUT_DEVICE, std::make_shared<Device>(false)));
}

void Settings::PrintSettings()
{
    NDNS::Get().WriteOutput("<SELECTED SETTINGS>\n", 0);
    for (auto field : allfields)
    {
        NDNS::Get().WriteOutput(field.first + ":" + field.second->GetValue() + "\n", 0);
    }
    NDNS::Get().WriteOutput("<END SETTINGS>\n", 0);
}