#include "NDNS.h"
#include "VoiceClient.h"
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
    if (!direct_c || !direct_c->IsConnected())
    {
        if (tcpThread)
        {
            tcpThread->detach();
            tcpThread = nullptr;
        }

        if (args.find("host") != args.end())
        {
            std::string nick;

            if (args.size() == 0)
            {
                nick = Settings::Get().GetField(S_LAST_NICKNAME)->GetValue();
            }
            else
            {
                nick = args["host"].front();
                Settings::Get().SetField(S_LAST_NICKNAME, nick);
            }

            direct_c = std::make_shared<TCPClient>(nick);

            tcpThread = std::make_shared<std::thread>(&TCPClient::Host, direct_c);
        }
        else
        {
            std::string ip;
            std::string nick;
            if (args.size() == 0)
            {
                ip = Settings::Get().GetField(S_LAST_IP)->GetValue();
                nick = Settings::Get().GetField(S_LAST_NICKNAME)->GetValue();
            }
            else
            {
                ip = args[" "].front();
                nick = args[" "].back();
                Settings::Get().SetField(S_LAST_IP, ip);
                Settings::Get().SetField(S_LAST_NICKNAME, nick);
            }

            direct_c = std::make_shared<TCPClient>(nick);
            tcpThread = std::make_shared<std::thread>(&TCPClient::Connect, direct_c, ip);
        }
    }
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
    if (direct_c && direct_c->IsConnected())
    {
        if (args.find("all") != args.end())
        {
            direct_c->GetVoiceClient()->muteIn = true;
            direct_c->GetVoiceClient()->muteOut = true;
        }
        else if (args.find("input") != args.end())
        {
            auto mute = &direct_c->GetVoiceClient()->muteIn;
            *mute = !*mute;
            if (*mute)
            {
                WriteOutput("Microphone are muted!", SERVER);
            }
            else
            {
                WriteOutput("Microphone are unmuted!", SERVER);
            }
        }
        else if (args.find("output") != args.end())
        {
            auto mute = &direct_c->GetVoiceClient()->muteOut;
            *mute = !*mute;
            if (*mute)
            {
                WriteOutput("Output are muted!", SERVER);
            }
            else
            {
                WriteOutput("Output are unmuted!", SERVER);
            }
        }
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