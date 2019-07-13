/*
@   NDNS | Team Illuminate 2019.

@   Entry point to the programm. NDNS serves to initialize settings, handling command, global TCP connection, works with in\out, managing communication processes.

@   Точка входа в приложение. NDNS служит для инициализации настроек, обработки команд,глобального TCP соединения, происходит работа с инпутом\аутпутом, управляет
@   процессами общения.
 */

#include "Settings.h"
#include <string>
#include <map>
#include <functional>
#include <boost/asio.hpp>
#include <mutex>
#include <iostream>
#include <SDL2/SDL.h>
#include <thread>
#include <list>
#include <queue>

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;

typedef u_int8_t byte;
typedef std::shared_ptr<tcp::socket> TCP_socketptr;
typedef std::shared_ptr<std::thread> Thread_ptr;
typedef std::pair<std::string, std::list<std::string>> ArgsPair;
typedef std::map<ArgsPair::first_type, ArgsPair::second_type> ArgsMap;

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

class NDNS
{
public:

    static NDNS &Get();

    std::string GetInput();
    void WriteOutput(std::string output, byte code);
    void Start();

    std::list<std::string> Split(std::string input, std::string split, bool inversive = false);
    std::string Reverse(std::string input);

private:

    NDNS();
    NDNS(const NDNS &) = delete;
    NDNS operator=(const NDNS &) = delete;

    //Settings settings;
    std::map<char, Command> commands;

    TCP_socketptr globalSocket = nullptr;
    TCP_socketptr directSocket = nullptr;

    std::mutex m_input;
    std::queue<std::string> inputStorage;

    void ListenInput();

    Thread_ptr inputThread = nullptr;

    void Connection_cmd(ArgsMap args);
    void Volume_cmd(ArgsMap args);
    void Mute_cmd(ArgsMap args);
};