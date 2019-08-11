#include "NDNS.h"
#include "DirectConn.h"
#include "Settings.h"
#include "SDLAudioManager.h"
#include "SettingsFields.h"

void NDNS::InitCommands()
{

    Command help{"",
                    "Displays all commands",
                    std::bind(&NDNS::Help_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('?', help));

    Command connect{"Connection",
                    "/c ip - setup direct connection to ip.",
                    std::bind(&NDNS::Connection_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('c', connect));

    Command volume{"Volume",
                   "Change volume \n -setup - Setup devices for input and output \n -input [1..100] - Change microphone volume.\n -output [1..100]- Change output volume.\n -threshold [0..100] - change input threshold",
                   std::bind(&NDNS::Volume_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('v', volume));

    Command mute{"Mute",
                 "Mute or unmute \n -input - for microphone.\n -output - for output.",
                 std::bind(&NDNS::Mute_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('m', mute));

    Command settings{"Settings", "-save", std::bind(&NDNS::Settings_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('s', settings));
}

void NDNS::Help_cmd(ArgsMap args)
{
    std::stringstream help_o;
    for (auto cmd : commands)
    {
        help_o << "/" << cmd.first << " : " << cmd.second.name << "\n"
               << cmd.second.desc << "\n\n";
    }

    WriteOutput("\n" + help_o.str(), 0);
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
            return;
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
        Settings::Get().SetField(S_VOLUME_IN, args["input"].front());
    }
    if (args.find("output") != args.end())
    {
        Settings::Get().SetField(S_VOLUME_OUT, args["output"].front());
    }
    if (args.find("setup") != args.end())
    {
        Settings::Get().SetupFromConsole();
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