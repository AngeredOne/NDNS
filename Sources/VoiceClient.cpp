#include "VoiceClient.h"
#include "SDLAudioManager.h"

void TCPClient::Create(tcp::endpoint ep, bool isHost, std::string nick)
{
    try
    {
        if (isHost)
        {
            //setup local ep + acceptor
            tcp::acceptor *accp = new tcp::acceptor(service, ep);
            TCP_socketptr remote(new tcp::socket(service));
            NDNS::Get().WriteOutput("Pipa", DEBUG);
            accp->accept(*remote);
            NDNS::Get().WriteOutput("Pepa", DEBUG);
            with = remote;
            delete accp;
        }
        else
        {
            //setup remote ep + connect
            with = TCP_socketptr(new tcp::socket(service));
            with->connect(ep);
        }

        voiceClient = new VoiceClient();
        connected = true;

        //Send "hello!"
        Send(1, (int8 *)nick.data(), 16);

        NDNS::Get().WriteOutput("Connection established!\nChat:\n", SERVER);
        HandleMessage();
    }
    catch (const std::exception &e)
    {
        NDNS::Get().WriteOutput(e.what(), ERROR);
    }
}

void TCPClient::Send(int16 code, int8 *data, size_t size)
{
    if (connected)
    {
        int8 *bytes = new int8[size + 2];

        memcpy(bytes, &code, 2);
        memcpy(&bytes[2], data, size);

        with->write_some(buffer(bytes, size + 2));
        delete bytes;
    }
}

void TCPClient::HandleMessage()
{

    struct port_sync
    {
        uint16 a_p;
        uint16 v_p;
    };

    while (connected)
    {
        int16 *code_r = new int16;
        read(*with, buffer(code_r, 2));

        int code = *code_r;
        switch (code)
        {
            //Set conn + nick
        case 1:
            if (voiceClient)
            {
                auto data = new int8[16];
                read(*with, buffer(data, 16));
                voiceClient->Nick = std::string(std::string(data, 16).c_str());

                port_sync *psyna = new port_sync();
                psyna->a_p = voiceClient->GetVoiceSockets()->voice_socket->local_endpoint().port();
                psyna->v_p = voiceClient->GetVoiceSockets()->video_socket->local_endpoint().port();

                data = new int8[4];
                memcpy(data, psyna, 4);
                Send(02, data, 4);
            }
            break;

        case 2:

            auto data = new int16[2];
            read(*with, buffer(data, 4));

            auto ports = reinterpret_cast<port_sync *>(data);
            voiceClient->Create(ports->a_p, ports->v_p, with->remote_endpoint().address().to_string());

            break;
        }
    }
}

VoiceClient *TCPClient::GetVoiceClient()
{
    return voiceClient;
}

VoiceClient::VoiceClient()
{
    client_sockets = std::make_shared<VoiceSockets>();
}

bool VoiceClient::Create(uint16 a_port, uint16 v_port, std::string rep)
{
    try
    {
        client_sockets->RegEP(a_port, v_port, rep);
        is_connected = true;

        voiceThread = std::make_shared<std::thread>(&VoiceClient::ListenAudio, this);
        microphoneThread = std::make_shared<std::thread>(&VoiceClient::ListenMicrophone, this);

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Cannot establish connection:\n"
                  << e.what() << '\n';
        return false;
    }
}

void VoiceClient::ListenChat()
{
    // while (is_connected)
    // {
    //     std::string message;
    //     while (message.find("<|-|>") == std::string::npos)
    //     {
    //         char *msg = new char[32];
    //         UDPEndPoint senderEP;
    //         client_sockets->chat_socket->receive_from(buffer(msg, 32), senderEP);
    //         message += std::string(std::string(msg, 32).c_str());
    //         delete msg;
    //     }
    //     message = Nick + ": " + message.erase(message.find("<|-|>"), 5);
    //     NDNS::Get().WriteOutput(message, CHAT);
    // }
}

void VoiceClient::ListenAudio()
{
    while (is_connected)
    {
        int16 *bits = new int16[bitrate];
        UDPEndPoint senderEP;
        client_sockets->voice_socket->receive_from(buffer(bits, bitrate), senderEP);
        if (!muteOut)
        {
            SDLAudioManager::Get().PlayAudio(bits, bitrate);
        }
        delete bits;
    }
}

void VoiceClient::SendAudio(int16 *data, size_t len)
{
    if (client_sockets->voice_remoteEP && data)
    {
        if (!muteIn)
        {
            client_sockets->voice_socket->send_to(buffer(data, len), *client_sockets->voice_remoteEP);
        }
        delete data;
    }
}

void VoiceClient::SendMessage(std::string msg)
{
    // if (client_sockets->chat_remoteEP && msg != "")
    // {
    //     msg += "<|-|>";
    //     client_sockets->chat_socket->send_to(buffer(msg.data(), msg.size()), *client_sockets->chat_remoteEP);
    // }
}

void VoiceClient::ListenMicrophone()
{
    SDLAudioManager::Get().StartRecord();
    while (is_connected)
    {
        int buf = 1024;
        auto *data = SDLAudioManager::Get().RecordAudio(buf);
        SendAudio(data, buf * sizeof(decltype(*data)));
    }
}

VoiceSockets::VoiceSockets()
{
    voice_socket = std::make_shared<ip::udp::socket>(n_io_stream, udp::endpoint(udp::v4(), 0));
    video_socket = std::make_shared<ip::udp::socket>(n_io_stream, udp::endpoint(udp::v4(), 0));
}

void VoiceSockets::RegEP(uint16 a_port, uint16 v_port, std::string remote_endpoint)
{
    voice_remoteEP = std::make_shared<UDPEndPoint>(ip::address::from_string(remote_endpoint), a_port);
    video_remoteEP = std::make_shared<UDPEndPoint>(ip::address::from_string(remote_endpoint), v_port);
}
