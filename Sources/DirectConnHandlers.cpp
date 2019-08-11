#include "DirectConn.h"
#include "NDNS.h"

void DirectConn::SyncData()
{
}

void DirectConn::SyncPorts()
{
    port_sync ps;
    s_remote->read_some(buffer(&ps, 4));

    auto remoteip = s_remote->remote_endpoint().address();

    s_voice->connect(udp::endpoint(remoteip, ps.a_p));
    s_trans->connect(udp::endpoint(remoteip, ps.v_p));

    StartVoice();
}

void DirectConn::SyncBitrate()
{
}

void DirectConn::StartVoice()
{
    state = MESSAGING;
    
    voiceWriter = Thread_ptr(new std::thread(&DirectConn::RecordVoice, this));
    voiceListener = Thread_ptr(new std::thread(&DirectConn::ListenVoice, this));

    voiceWriter->detach();
    voiceListener->detach();

    translationWriter = Thread_ptr(new std::thread(&DirectConn::RecordTranslation, this));
    translationListener = Thread_ptr(new std::thread(&DirectConn::ListenTranslation, this));

    translationWriter->detach();
    translationListener->detach();
}

void DirectConn::ChatMessage()
{
    std::string message;

    while (message.find(">-<") == std::string::npos)
    {
        int8 *msg = new int8[8];
        s_remote->read_some(buffer(msg, 8));
        message += std::string(msg, 8).c_str();
        delete[] msg;
    }

    message = message.erase(message.find(">-<"), 3);
    NDNS::Get().WriteOutput(message, CHAT);
}