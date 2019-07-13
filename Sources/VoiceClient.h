#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include "SDLAudioManager.h"

using namespace boost;

// UDP EndPoint type
typedef asio::ip::udp::endpoint UDPEndPoint;
// Shared_ptr on UDP EndPoint
typedef std::shared_ptr<UDPEndPoint> UDPEP_PTR;
// UDP Socket type
typedef asio::ip::udp::socket UDPSocket;
// Boost::asio network in\out stream
typedef asio::io_service nios;
// Shared_ptr for UDPSocket
typedef std::shared_ptr<UDPSocket> Socket_ptr;
// Shared_ptr for thread
typedef std::shared_ptr<std::thread> Thread_ptr;

struct NDNS_Client
{
    Socket_ptr chat_socket;
    Socket_ptr voice_socket;
    Socket_ptr video_socket;
    UDPEP_PTR chat_remoteEP;
    UDPEP_PTR voice_remoteEP;
    UDPEP_PTR video_remoteEP;
    nios n_io_stream;

    NDNS_Client(uint16_t c_port, uint16_t a_port, uint16_t v_port, std::string remote_endpoint)
    {
        auto chat_ep  = std::make_shared<UDPEndPoint>(asio::ip::udp::v4(), c_port);
        auto voice_ep = std::make_shared<UDPEndPoint>(asio::ip::udp::v4(), a_port);
        auto video_ep = std::make_shared<UDPEndPoint>(asio::ip::udp::v4(), v_port);

        chat_remoteEP  = std::make_shared<UDPEndPoint>(asio::ip::address::from_string(remote_endpoint), c_port);
        voice_remoteEP = std::make_shared<UDPEndPoint>(asio::ip::address::from_string(remote_endpoint), a_port);
        video_remoteEP = std::make_shared<UDPEndPoint>(asio::ip::address::from_string(remote_endpoint), v_port);

        chat_socket = std::make_shared<asio::ip::udp::socket>(n_io_stream, *chat_ep);
        voice_socket = std::make_shared<asio::ip::udp::socket>(n_io_stream, *voice_ep);
        video_socket = std::make_shared<asio::ip::udp::socket>(n_io_stream, *video_ep);
    }
};

class VoiceClient 
{
public:
    VoiceClient(std::string endpoint);
    bool Create(std::string rep);

    void ListenAudio();
    void ListenChat();

    void SendAudio(Sint8* bytes, int len);
    void SendMessage(std::string msg);

    bool inChat = false;

private:
    std::shared_ptr<NDNS_Client> client_sockets = nullptr;
    Thread_ptr chatThread;
    Thread_ptr voiceThread;
    bool is_connected = false;
    int bitrate = 1024;
    int port = 25565;
};

//195.46.123.219