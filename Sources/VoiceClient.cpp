#include "VoiceClient.h"
#include "compressor.c"

VoiceClient::VoiceClient(std::string endpoint)
{
    localEndPoint = std::make_shared<UDPEndPoint>(asio::ip::udp::v4(), port);
    remoteEndPoint = std::make_shared<UDPEndPoint>(asio::ip::make_address(endpoint), port);
    if (Create())
    {
        listenerThread = std::make_shared<std::thread>(&VoiceClient::Listen, this);
    }
}

bool VoiceClient::Create()
{
    try
    {
        socket = std::make_shared<asio::ip::udp::socket>(net_stream, *localEndPoint);

        is_connected = true;
        std::cout << "VoiceClient ready.\n";
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Cannot start VoiceClient:\n"
                  << e.what() << '\n';
        return false;
    }
}

void VoiceClient::Listen()
{
    while (is_connected)
    {
        Uint8 *bits = new Uint8[bitrate];
        UDPEndPoint senderEP;
        std::cout << "";
        socket->receive_from(asio::buffer(bits, bitrate), senderEP);
        SDLAudioManager::Get().PlayAudio(bits, bitrate);
        delete bits;
        //
    }
}

void VoiceClient::SendAudio(Uint8 *data, int len)
{
    if (remoteEndPoint && data)
    {
        socket->send_to(asio::buffer(data, len), *remoteEndPoint);
    }
}