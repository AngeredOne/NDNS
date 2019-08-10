#include "NDNS.h"
#include "SDLAudioManager.h"
#include "DirectConn.h"
#include "Settings.h"
#include "SettingsFields.h"

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

    initscr();

    settingsWindow = UIWindow_ptr(newwin(8, COLS, 0, 0));
    globalWindow = UIWindow_ptr(newwin(LINES - 10, COLS, 9, 0));
    inputWindow = UIWindow_ptr(newwin(1, COLS, LINES - 1, 0));

    start_color();

    init_pair(ERROR_COLOR, COLOR_RED, 0);
    init_pair(SERVER_COLOR, COLOR_GREEN, 0);
    init_pair(CHAT_COLOR, COLOR_CYAN, 0);
    init_pair(DEBUG_COLOR, COLOR_WHITE, 0);

    // Print greeting
    std::stringstream greeting;
    greeting << "<---===:::::::::::::::::::::===--->\n"
             << "<---===       N_D_N_S       ===--->\n"
             << "<---=== NO DISCORD NO SKYPE ===--->\n"
             << "<---===:::::::::::::::::::::===--->\n\n";

    WriteOutput(greeting.str(), 0);

    std::stringstream appv;
    appv << "APPLICATION INFO: \n"
         << "Version: " << app_ver.appv << "\n"
         << "Type: " << app_ver.v_type << "\n"
         << "Codename: " << app_ver.codename << "\n\n";

    WriteOutput(appv.str(), 0);

    InitCommands();

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioInit(SDL_GetAudioDriver(0));
    SDLAudioManager::Get().InitProcessors();
    DirectConn::Get();

    ListenInput();
}

void NDNS::WriteOutput(std::string output, int8 code)
{
    std::string output_s;
    int color = 0;

    switch (code)
    {
    case ERROR:
        color = ERROR_COLOR;
        output_s = "ERROR:\n" + output + "\n";
        break;
    case SERVER:
        color = SERVER_COLOR;
        output_s = "Server: " + output + "\n";
        break;
    case CHAT:
        color = CHAT_COLOR;
        output_s = ">>> " + output + "\n";
        break;
    case DEBUG:
        color = DEBUG_COLOR;
        output_s = "DEBUG:\n" + output + "\n";
        break;
    default:
        output_s = output;
        break;
    };

    wattron(globalWindow.get(), COLOR_PAIR(color));
    waddstr(globalWindow.get(), output_s.data());
    wattroff(globalWindow.get(), COLOR_PAIR(color));
    wrefresh(globalWindow.get());
}

void NDNS::ListenInput()
{
    curs_set(0);
    while (true)
    {
        std::string input;
        char input_raw[256];

        wgetstr(inputWindow.get(), input_raw);
        wclear(inputWindow.get());
        wrefresh(inputWindow.get());

        if (input_raw)
            input = std::string(input_raw);

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
            else
            {
                WriteOutput("Unrecognized command. Type /? to get all commands.", SERVER);
            }
        }
        else
        {
            input = Settings::Get().GetField(S_LAST_NICKNAME)->GetValue() + ": " + input + ">-<";
            DirectConn::Get().Send(OPCODE::CHATMSG, input.data(), input.size());
        }
    }
}

ArgsMap *NDNS::ParseCommand(std::string input)
{
    ArgsMap *args = new ArgsMap();
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
