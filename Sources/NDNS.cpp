#include "NDNS.h"
#include "SDLAudioManager.h"
#include "VoiceClient.h"

NDNS::NDNS()
{
}

NDNS &NDNS::Get()
{
    static NDNS inst;
    return inst;
}

void NDNS::Start()
{
    // Print greeting
    std::stringstream greeting;
    greeting << "<---===:::::::::::::::::::::===--->\n"
             << "<---===       N_D_N_S       ===--->\n"
             << "<---=== NO DISCORD NO SKYPE ===--->\n"
             << "<---===:::::::::::::::::::::===--->\n\n";

    WriteOutput(greeting.str(), 0);

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioInit(SDL_GetAudioDriver(0));
    SDLAudioManager::Get().InitProcessors();

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

    inputThread = Thread_ptr(new std::thread(&NDNS::ListenInput, this));
}

void NDNS::WriteOutput(std::string output, byte code)
{
    switch (code)
    {
    case ERROR:
        std::cout << "ERROR:\n\033[0;31m" << output << "\033[0m\n";
        break;
    case SERVER:
        std::cout << "Server:\n\033[1;32m" << output << "\033[0m\n";
        break;
    case CHAT:
        std::cout << ">>> \033[0;35m" << output << "\033[0m\n";
        break;
    case DEBUG:
        std::cout << "DEBUG:\n\033[1;37m" << output << "\033[0m\n";
        break;

    default:
        std::cout << output;
        break;
    };
}

void NDNS::ListenInput()
{
    while (true)
    {
        std::string input;
        getline(std::cin, input);
        if (input[0] == '/')
        {
            if (auto command = commands.find(input[1]); command != commands.end())
            {
                ArgsMap args;
                try
                {
                    auto args_str = input.erase(0, 3);

                    auto posx = args_str.find('-');

                    if (args_str.length() > 1)
                    {

                        // If here keys (and mb args) like /c -host 25568 or /m -all
                        if (posx != std::string::npos)
                        {
                            auto split_keys = Split(args_str, "-", true);
                            for (auto key_block : split_keys)
                            {
                                auto split_args = Split(key_block, " ");
                                auto pair = ArgsPair(split_args.front(), std::list<std::string>());
                                if (split_args.size() > 1)
                                {
                                    auto args_begin = split_args.begin();
                                    args_begin++;
                                    for (auto argit = args_begin; argit != split_args.end(); ++argit)
                                        if (*argit != " " && *argit != "")
                                            pair.second.push_back(*argit);
                                }
                                args.insert(pair);
                            }
                        }
                        // If here only args(no keys) like /c 127.0.0.1 25568
                        else
                        {
                            auto pair = ArgsPair(" ", std::list<std::string>());
                            auto argsc = Split(args_str, " ");
                            for (auto arg : argsc)
                                pair.second.push_back(arg);
                            args.insert(pair);
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    WriteOutput(e.what(), ERROR);
                }

                command->second.handler(args);
            }
        }
        else if (direct_c)
            direct_c->GetVoiceClient()->SendMessage(input);
    }
}

//Reads input from storage.
std::string NDNS::GetInput()
{
    if (!inputStorage.empty())
    {
        auto input = inputStorage.front();
        inputStorage.pop();
        return input;
    }
    else
        return "";
}

std::string NDNS::Reverse(std::string input)
{
    std::string str;
    for (auto it = input.rbegin(); it != input.rend(); ++it)
    {
        str += *it;
    }
    return str;
}

std::list<std::string> NDNS::Split(std::string input, std::string split, bool inversive)
{
    std::string str;

    if (inversive)
    {
        str = Reverse(input);
    }
    else
        str = input;

    std::list<std::string> splitarr;
    while (str.find(split) != std::string::npos)
    {
        auto split_ind = str.find(split);
        auto arg = str.substr(0, split_ind);
        if (inversive)
            arg = Reverse(arg);
        splitarr.push_back(arg);
        str = str.erase(0, arg.size() + 1);
    }

    if (str.size() > 0)
    {
        splitarr.push_back(str);
    }
    return splitarr;
}

void NDNS::Connection_cmd(ArgsMap args)
{
    if (!direct_c)
    {
        direct_c = std::make_shared<TCPClient>();
        if (args.find("host") != args.end())
        {
            int port = atoi(args["host"].front().c_str());
            tcp::endpoint ep(tcp::v4(), port);
            tcpThread = std::make_shared<std::thread>(&TCPClient::Create, direct_c, ep, true);
        }
        else
        {
            std::string ip = args[" "].front();
            // int port = atoi(args[" "].back().c_str());
            // tcp::endpoint ep(ip::address::from_string(ip), port);
            // tcpThread = std::make_shared<std::thread>(&TCPClient::Create, direct_c, ep, false);
            vc = new VoiceClient(ip);
        }
    }
}

void NDNS::Volume_cmd(ArgsMap args)
{
    if (args.find("input") != args.end())
    {
        auto perc = atoi(args["input"].front().c_str());
        Settings::Get().input->SetVolume(perc);
    } 
    if (args.find("output") != args.end())
    {
        auto perc = atoi(args["output"].front().c_str());
        Settings::Get().output->SetVolume(perc);
    } 
    if (args.find("setup") != args.end())
    {
        Settings::Get().SetupFromConsole();
        SDLAudioManager::Get().InitProcessors();
    } 
    if (args.find("threshold") != args.end()) {
        auto perc = atoi(args["threshold"].front().c_str());
        Settings::Get().input->SetThreshold(perc);
    }
}

void NDNS::Mute_cmd(ArgsMap args)
{
    // if (args.find("all") != args.end())
    // {

    //     direct_c->GetVoiceClient()->muteIn = true;
    //     direct_c->GetVoiceClient()->muteOut = true;
    // }
    // else if (args.find("input") != args.end())
    // {
    //     bool mute = atoi(args["input"].front().c_str());
    //     direct_c->GetVoiceClient()->muteIn = mute;
    // }
    // else if (args.find("output") != args.end())
    // {
    //     bool mute = atoi(args["output"].front().c_str());
    //     direct_c->GetVoiceClient()->muteOut = mute;
    // }
    if (args.find("all") != args.end())
    {

        vc->muteIn = true;
        vc->muteOut = true;
    }
    else if (args.find("input") != args.end())
    {
        bool mute = atoi(args["input"].front().c_str());
        vc->muteIn = mute;
    }
    else if (args.find("output") != args.end())
    {
        bool mute = atoi(args["output"].front().c_str());
        vc->muteOut = mute;
    }
}