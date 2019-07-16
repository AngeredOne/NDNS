/*
@   NDNS | Team Illuminate 2019.

@   Entry point to the programm. NDNS serves to initialize settings, handling command, global TCP connection, works with in\out, managing communication processes.

@   Точка входа в приложение. NDNS служит для инициализации настроек, обработки команд,глобального TCP соединения, происходит работа с инпутом\аутпутом, управляет
@   процессами общения.
 */
#pragma once

#include "Definitions.h"


struct Command
{
    std::string name;
    std::string desc;
    std::function<void(ArgsMap)> handler;
};

enum OUTPUT_CODE
{
    ERROR = 1,
    SERVER = 2,
    CHAT = 3,
    DEBUG = 4
};

class TCPClient;

class NDNS
{
public:
    static NDNS &Get();

    std::string GetInput();
    void WriteOutput(std::string output, int8 code);
    void Start();

    std::list<std::string> Split(std::string input, std::string split, bool inversive = false);
    std::string Reverse(std::string input);

private:
    NDNS();
    NDNS(const NDNS &) = delete;
    NDNS operator=(const NDNS &) = delete;
    void InitCommands();

    //Settings settings;
    std::map<char, Command> commands;

    std::shared_ptr<TCPClient> direct_c = nullptr;

    std::mutex m_input;
    std::queue<std::string> inputStorage;

    void ListenInput();
    ArgsMap *ParseCommand(std::string input);

    Thread_ptr tcpThread = nullptr;

    void Connection_cmd(ArgsMap args);
    void Volume_cmd(ArgsMap args);
    void Mute_cmd(ArgsMap args);
    void Settings_cmd(ArgsMap args);

    APPV app_ver;
};