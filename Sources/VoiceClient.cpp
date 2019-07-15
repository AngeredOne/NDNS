#include "VoiceClient.h"
#include "SDLAudioManager.h"

void TCPClient::RestartUDP()
{
    std::string nick = voiceClient->Nick;
    delete voiceClient;
    voiceClient = new VoiceClient();
    voiceClient->Nick = nick;
    port_sync *psyna = new port_sync();
    psyna->a_p = voiceClient->GetVoiceSockets()->voice_socket->local_endpoint().port();
    psyna->v_p = voiceClient->GetVoiceSockets()->video_socket->local_endpoint().port();

    auto data = new int8[4];
    memcpy(data, psyna, 4);
    Send(03, data, 4);
    delete data;
}

void TCPClient::Create(tcp::endpoint ep, bool isHost, std::string nick)
{
    try
    {
        if (isHost)
        {
            //setup local ep + acceptor
            tcp::acceptor *accp = new tcp::acceptor(service, ep);
            TCP_socketptr remote(new tcp::socket(service));
            accp->accept(*remote);
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
        connected = false;
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

void VoiceClient::ListenMicrophone()
{
    SDLAudioManager::Get().Start();
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
