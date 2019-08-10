#ifndef __DEFS__
#define __DEFS__
#pragma once

#include <boost/asio.hpp>
#include <string>
#include <map>
#include <functional>
#include <mutex>
#include <iostream>
#include <SDL2/SDL.h>
#include <thread>
#include <list>
#include <queue>
#include <exception>
#include <fstream>
#include <experimental/filesystem>
#include <regex>
#include <ncurses.h>

namespace fs = std::experimental::filesystem;

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;

// singed 8 bits
typedef char int8;
// signed 16 bits
typedef short int16;
// unsigned 8 bits
typedef unsigned char uint8;
// unsigned 16 bits
typedef unsigned short uint16;

typedef std::shared_ptr<std::thread> Thread_ptr;
typedef std::pair<std::string, std::list<std::string>> ArgsPair;
typedef std::map<ArgsPair::first_type, ArgsPair::second_type> ArgsMap;

// UDP EndPoint type
typedef ip::udp::endpoint UDPEndPoint;
// Shared_ptr on UDP EndPoint
typedef std::shared_ptr<UDPEndPoint> UDPEP_PTR;
// UDP Socket type
typedef ip::udp::socket UDPSocket;
// Boost::asio network in\out stream
typedef io_service nios;
// Shared_ptr for UDPSocket
typedef std::shared_ptr<UDPSocket> UDP_socketptr;
// Shared_ptr for thread
typedef std::shared_ptr<std::thread> Thread_ptr;
typedef std::shared_ptr<tcp::socket> TCP_socketptr;

typedef std::shared_ptr<WINDOW> UIWindow_ptr;

struct APPV
{
    char *appv = "02.r";
    char *v_type = "STAGE";
    char *codename = "OhShitHereWeGoAgain";
    char* date = "11.08.19";
};

#define ERROR_COLOR 1
#define SERVER_COLOR 2
#define CHAT_COLOR 3
#define DEBUG_COLOR 4

#endif // !__DEFS__