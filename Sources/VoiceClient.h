#pragma once
#include "Definitions.h"

struct port_sync
{
    uint16 a_p;
    uint16 v_p;
};

struct VoiceSockets
{
    Socket_ptr voice_socket;
    Socket_ptr video_socket;

    UDPEP_PTR voice_remoteEP;
    UDPEP_PTR video_remoteEP;

    nios n_io_stream;

    VoiceSockets();
    void RegEP(uint16 a_port, uint16 v_port, std::string remote_endpoint);
};

class VoiceClient
{
public:
    VoiceClient();
    bool Create(uint16 audio_port, uint16 video_port, std::string remote_ip);

    void ListenAudio();
    void ListenMicrophone();

    void SendAudio(int16 *samples, size_t len);

    bool inChat = false;
    bool muteIn = false;
    bool muteOut = false;

    std::string Nick = "noname";

    const std::shared_ptr<VoiceSockets> GetVoiceSockets() { return client_sockets; };
    const bool IsConnected() { return is_connected; };

private:
    std::shared_ptr<VoiceSockets> client_sockets = nullptr;

    Thread_ptr chatThread;
    Thread_ptr voiceThread;
    Thread_ptr microphoneThread;
    std::string rep_str;

    bool is_connected = false;
    int bitrate = 1024;
    int port = 25565;
};

class TCPClient
{

public:
    void Create(tcp::endpoint ep, bool isHost, std::string nick);
    void HandleMessage();
    void Send(int16 code, int8 *data, size_t size);
    void RestartUDP();

    const bool IsConnected() { return connected; };

    VoiceClient *GetVoiceClient();

private:
    io_service service;
    bool connected;
    TCP_socketptr with;
    VoiceClient *voiceClient;
};