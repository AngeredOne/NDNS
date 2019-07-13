#include "NDNS.h"
#include "SDLAudioManager.h"

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
             << "<---===:::::::::::::::::::::===--->\n";

    WriteOutput(greeting.str(), 0);

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioInit(SDL_GetAudioDriver(0));
    SDLAudioManager::Get();

    Command connect{"Connection (/c)",
                    "Setup connection \n -host port - setup host for direct connection.\n ip port - setup direct connection to host.",
                    std::bind(&NDNS::Connection_cmd, this, std::placeholders::_1)};
    commands.insert(std::pair<char, Command>('c', connect));

    //settings = Settings();

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
        else
            inputStorage.push(input);
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
    std::stringstream output;
    for (auto pair : args)
    {
        output << pair.first + " : ";
        for (auto arg : pair.second)
        {
            output << arg << "; ";
        }
        output << "\n";
    }
    WriteOutput(output.str(), DEBUG);
}

void NDNS::Volume_cmd(ArgsMap args)
{
    if (args.find("input") != args.end())
    {
    }
    else if (args.find("output") != args.end())
    {
    }
}

void NDNS::Mute_cmd(ArgsMap args)
{
    if (args.find("all") != args.end())
    {
    }
    else if (args.find("input") != args.end())
    {
    }
    else if (args.find("output") != args.end())
    {
    }
}