#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include "Audio.h"

using namespace boost;

// Define simple "byte"
typedef int8_t byte;
// UDP EndPoint type
typedef asio::ip::udp::endpoint UDPEndPoint;
// UDP Socket type
typedef asio::ip::udp::socket UDPSocket;
// Boost::asio network in\out stream
typedef asio::io_service nios;
// Shared_ptr for UDPSocket
typedef std::shared_ptr<UDPSocket> Socket_ptr;
// Shared_ptr for thread
typedef std::shared_ptr<std::thread> Thread_ptr;
// Shared_ptr AudioStream
typedef std::shared_ptr<AudioStream> AOStream_ptr;

class VoiceClient : public ISoundInterprer
{
public:
    VoiceClient(std::string endpoint);
    bool Create();
    void Listen();
    void InterpretSamples(sf::Int16 *, std::size_t size);

private:
    Socket_ptr socket;
    std::shared_ptr<UDPEndPoint> remoteEndPoint = nullptr;
    std::shared_ptr<UDPEndPoint> localEndPoint = nullptr;
    Thread_ptr listenerThread;
    nios net_stream;
    bool is_connected = false;
    int bitrate = 2000;
    int port = 25565;
    AOStream_ptr audioStream;
};