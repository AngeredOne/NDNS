#include "NDNS.h"
#include "DirectConn.h"
#include "Settings.h"
#include "SDLAudioManager.h"
#include "SettingsFields.h"

void NDNS::InitCommands()
{
    Command connect{"Connection (/c)",
                    "Setup connection \n -host port - setup host for direct connection.\n ip port - setup direct connection to host.",
                    std::bind(&NDNS::Connection_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('c', connect));

    Command volume{"Volume (/v)",
                   "Change volume \n -setup - Setup devices for input and output \n -input [1..100] - Change microphone volume.\n -output [1..100]- Change output volume.\n -threshold [1..100] - change input threshold",
                   std::bind(&NDNS::Volume_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('v', volume));

    Command mute{"Mute (/m)",
                 "Mute or unmute \n -input [0\\1] - Mute microphone.\n -output [0\\1] - Mute output.",
                 std::bind(&NDNS::Mute_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('m', mute));

    Command settings{"Settings (/v)", "", std::bind(&NDNS::Settings_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('s', settings));
}

void NDNS::Connection_cmd(ArgsMap args)
{
    static std::regex ipFormat("\\d{1,3}(\\.\\d{1,3}){3}");
    if (args.size() > 0)
    {
        std::string ip = args[" "].front();
        if (std::regex_match(ip, ipFormat))
        {
            Settings::Get().SetField(S_LAST_IP, ip);
        }
        else
        {
            WriteOutput("Uncorrect format of IP address. Must be like 127.0.0.1", ERROR);
        }
    }
    std::string ip = Settings::Get().GetField(S_LAST_IP)->GetValue();
    WriteOutput("Connecting to " + ip, SERVER);
    DirectConn::Get().Connect(ip);
}

void NDNS::Volume_cmd(ArgsMap args)
{
    if (args.find("input") != args.end())
    {
        Settings::Get().SetField(S_INPUT_DEVICE, args["input"].front());
    }
    if (args.find("output") != args.end())
    {
        Settings::Get().SetField(S_OUTPUT_DEVICE, args["output"].front());
    }
    if (args.find("setup") != args.end())
    {
        Settings::Get().SetupFromConsole();
    }
    if (args.find("threshold") != args.end())
    {
        Settings::Get().SetField(S_THRESHOLD_IN, args["threshold"].front());
    }
}

void NDNS::Mute_cmd(ArgsMap args)
{
    if (args.find("all") != args.end())
    {
        DirectConn::Get().MuteInput();
        DirectConn::Get().MuteOutput();
    }
    else if (args.find("input") != args.end())
    {
        DirectConn::Get().MuteInput();
    }
    else if (args.find("output") != args.end())
    {
        DirectConn::Get().MuteOutput();
    }
}

void NDNS::Settings_cmd(ArgsMap args)
{
    if (args.find("save") != args.end())
    {
        Settings::Get().SaveConfig();
    }
    if (args.find("show") != args.end())
    {
        Settings::Get().PrintSettings();
    }
}