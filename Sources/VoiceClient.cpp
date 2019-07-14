#include "VoiceClient.h"
#include "NDNS.h"

void TCPClient::Create(tcp::endpoint ep, bool isHost)
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
        }
        else
        {
            //setup remote ep + connect
            with = TCP_socketptr(new tcp::socket(service));
            with->connect(ep);
            Send(new char[2]{0, 0}, 2);
            voiceClient = new VoiceClient(with->remote_endpoint().address().to_string());
        }
        connected = true;
        HandleMessage();
    }
    catch (const std::exception &e)
    {
        NDNS::Get().WriteOutput(e.what(), ERROR);
    }
}

void TCPClient::Send(char *data, int size)
{
    if (connected)
    {
        with->write_some(buffer(data, size));
    }
}

void TCPClient::HandleMessage()
{
    while (connected)
    {
        char *data = new char[2];
        read(*with, buffer(data, 2));
        int code = atoi(data);
        switch (code)
        {
            //Set conn + nick
        case 0:
            if (!voiceClient)
            {
                voiceClient = new VoiceClient(with->remote_endpoint().address().to_string());
            }
            break;

        default:
            break;
        }
    }
}

VoiceClient *TCPClient::GetVoiceClient()
{
    return voiceClient;
}

VoiceClient::VoiceClient(std::string endpoint)
{
    if (Create(endpoint))
    {
        chatThread = std::make_shared<std::thread>(&VoiceClient::ListenChat, this);
        voiceThread = std::make_shared<std::thread>(&VoiceClient::ListenAudio, this);
        microphoneThread = std::make_shared<std::thread>(&VoiceClient::ListenMicrophone, this);
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
        std::cout << "\033[0;35m"
                  << ">>> " << message << " \033[0m" << std::endl;
    }
}

void VoiceClient::ListenAudio()
{
    while (is_connected)
    {
        Sint16 *bits = new Sint16[bitrate];
        UDPEndPoint senderEP;
        client_sockets->voice_socket->receive_from(asio::buffer(bits, bitrate), senderEP);
        if (!muteOut)
        {
            SDLAudioManager::Get().PlayAudio(bits, bitrate);
        }
        delete bits;
    }
}

void VoiceClient::SendAudio(short *data, int len)
{
    if (client_sockets->voice_remoteEP && data)
    {
        if (!muteIn)
        {
            client_sockets->voice_socket->send_to(asio::buffer(data, len), *client_sockets->voice_remoteEP);
        }
        delete data;
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

void VoiceClient::ListenMicrophone()
{
    SDLAudioManager::Get().StartRecord();
    while (is_connected)
    {
        int buf = 1024;
        Sint16 *data = SDLAudioManager::Get().RecordAudio(buf);
        SendAudio(data, buf * 2);
    }
}