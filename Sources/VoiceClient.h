#pragma once
#include "NDNS.h"


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
        auto chat_ep = std::make_shared<UDPEndPoint>(ip::udp::v4(), c_port);
        auto voice_ep = std::make_shared<UDPEndPoint>(ip::udp::v4(), a_port);
        auto video_ep = std::make_shared<UDPEndPoint>(ip::udp::v4(), v_port);

        chat_remoteEP = std::make_shared<UDPEndPoint>(ip::address::from_string(remote_endpoint), c_port);
        voice_remoteEP = std::make_shared<UDPEndPoint>(ip::address::from_string(remote_endpoint), a_port);
        video_remoteEP = std::make_shared<UDPEndPoint>(ip::address::from_string(remote_endpoint), v_port);

        chat_socket = std::make_shared<ip::udp::socket>(n_io_stream, *chat_ep);
        voice_socket = std::make_shared<ip::udp::socket>(n_io_stream, *voice_ep);
        video_socket = std::make_shared<ip::udp::socket>(n_io_stream, *video_ep);
    }
};

class VoiceClient
{
public:
    VoiceClient(std::string endpoint);
    bool Create(std::string rep);

    void ListenAudio();
    void ListenChat();
    void ListenMicrophone();

    void SendAudio(short *bytes, int len);
    void SendMessage(std::string msg);

    bool inChat = false;
    bool muteIn = false;
    bool muteOut = false;

private:
    std::shared_ptr<NDNS_Client> client_sockets = nullptr;

    Thread_ptr chatThread;
    Thread_ptr voiceThread;
    Thread_ptr microphoneThread;

    bool is_connected = false;
    int bitrate = 1024;
    int port = 25565;
};

class TCPClient
{

public:
    void Create(tcp::endpoint ep, bool isHost = false);
    void HandleMessage();
    void Send(char *data, int size);

    VoiceClient *GetVoiceClient();

private:
    io_service service;
    bool connected;
    TCP_socketptr with;
    VoiceClient *voiceClient;
};