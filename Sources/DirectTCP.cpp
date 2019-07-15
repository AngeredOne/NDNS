#include "VoiceClient.h"
#include "NDNS.h"

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

void TCPClient::WaitSocket()
{
    //setup local ep + acceptor
}

void TCPClient::Host()
{
    tcp::acceptor *accp = new tcp::acceptor(service, tcp::endpoint(tcp::v4(), 25560));
    TCP_socketptr remote(new tcp::socket(service));
    accp->accept(*remote);
    with = remote;
    delete accp;

    if (voiceClient)
        delete voiceClient;

    voiceClient = new VoiceClient();
    connected = true;

    Send(1, (int8 *)nick.data(), 16);

    port_sync *psyna = new port_sync();
    psyna->a_p = voiceClient->GetVoiceSockets()->voice_socket->local_endpoint().port();
    psyna->v_p = voiceClient->GetVoiceSockets()->video_socket->local_endpoint().port();

    auto data = new int8[4];
    memcpy(data, psyna, 4);
    Send(02, data, 4);

    HandleMessage();
}

void TCPClient::Connect(std::string ip)
{
    try
    {
        //setup remote ep + connect
        with = TCP_socketptr(new tcp::socket(service));
        with->connect(tcp::endpoint(ip::address::from_string(ip), 25560));

        if (voiceClient)
            delete voiceClient;
            
        voiceClient = new VoiceClient();
        connected = true;

        //Send "hello!"
        Send(1, (int8 *)nick.data(), 16);

        port_sync *psyna = new port_sync();
        psyna->a_p = voiceClient->GetVoiceSockets()->voice_socket->local_endpoint().port();
        psyna->v_p = voiceClient->GetVoiceSockets()->video_socket->local_endpoint().port();

        auto data = new int8[4];
        memcpy(data, psyna, 4);
        Send(02, data, 4);

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