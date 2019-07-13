#include "VoiceClient.h"
#include "compressor.c"

VoiceClient::VoiceClient(std::string endpoint)
{
    if (Create(endpoint))
    {
        chatThread = std::make_shared<std::thread>(&VoiceClient::ListenChat, this);
        voiceThread = std::make_shared<std::thread>(&VoiceClient::ListenAudio, this);
    }
}

bool VoiceClient::Create(std::string rep)
{
    try
    {
        client_sockets = std::make_shared<NDNS_Client>(25565, 25566, 25567, rep);
        is_connected = true;
        std::cout << "NDNS connetction established.\n";
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
    while (is_connected)
    {
        std::string message;
        while (message.find("<|-|>") == std::string::npos)
        {
            char *msg = new char[32];
            UDPEndPoint senderEP;
            client_sockets->chat_socket->receive_from(asio::buffer(msg, 32), senderEP);
            message += std::string(std::string(msg, 32).c_str());
            delete msg;
        }
        message = message.replace(message.find("<|-|>"), 5, "");
        std::cout << "\033[0;35m" << ">>> " << message << " \033[0m" << std::endl;
    }
}

void VoiceClient::ListenAudio()
{
    while (is_connected)
    {
        Sint8 *bits = new Sint8[bitrate];
        UDPEndPoint senderEP;
        client_sockets->voice_socket->receive_from(asio::buffer(bits, bitrate), senderEP);
        SDLAudioManager::Get().PlayAudio(bits, bitrate);
        delete bits;
    }
}

void VoiceClient::SendAudio(Sint8 *data, int len)
{
    if (client_sockets->voice_remoteEP && data)
    {
        client_sockets->voice_socket->send_to(asio::buffer(data, len), *client_sockets->voice_remoteEP);
    }
}

void VoiceClient::SendMessage(std::string msg)
{
    if (client_sockets->chat_remoteEP && msg != "")
    {
        msg += "<|-|>";
        client_sockets->chat_socket->send_to(asio::buffer(msg.data(), msg.size()), *client_sockets->chat_remoteEP);
    }
}