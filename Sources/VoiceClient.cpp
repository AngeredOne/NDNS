#include "VoiceClient.h"
#include "SDLAudioManager.h"

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
