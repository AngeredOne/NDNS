#include "NDNS.h"
#include "SDLAudioManager.h"
#include "VoiceClient.h"
#include "Settings.h"

int main(int, char **)
{
    NDNS::Get().Start();
}

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

    InitCommands();

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioInit(SDL_GetAudioDriver(0));
    SDLAudioManager::Get().InitProcessors();

    ListenInput();
}

void NDNS::WriteOutput(std::string output, int8 code)
{
    switch (code)
    {
    case ERROR:
        std::cout << "ERROR:\n\033[0;31m" << output << "\033[0m\n";
        break;
    case SERVER:
        std::cout << "Server: \033[1;32m" << output << "\033[0m\n";
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
                try
                {
                    auto args_str = input.erase(0, 3);
                    auto args = ParseCommand(args_str);
                    command->second.handler(*args);
                    delete args;
                }
                catch (std::exception e)
                {
                    WriteOutput(e.what(), ERROR);
                }
            }
        }
        else if (direct_c)
            direct_c->GetVoiceClient()->SendMessage(input);
    }
}

ArgsMap *NDNS::ParseCommand(std::string input)
{
    ArgsMap* args = new ArgsMap();
    try
    {
        auto posx = input.find('-');

        if (input.length() > 1)
        {
            // If here keys (and mb args) like /c -host 25568 or /m -all
            if (posx != std::string::npos)
            {
                auto split_keys = Split(input, "-", true);
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
                    args->insert(pair);
                }
            }
            // If here only args(no keys) like /c 127.0.0.1 25568
            else
            {
                auto pair = ArgsPair(" ", std::list<std::string>());
                auto argsc = Split(input, " ");
                for (auto arg : argsc)
                    pair.second.push_back(arg);
                args->insert(pair);
            }
        }
    }
    catch (const std::exception &e)
    {
        WriteOutput(std::string("Cannot parse command: \n") + e.what(), ERROR);
    }

    return args;
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