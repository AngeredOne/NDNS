#include "DirectConn.h"
#include "Settings.h"
#include "SettingsFields.h"
#include "SDLAudioManager.h"
#include "boost/bind.hpp"
#include <chrono>

DirectConn::DirectConn()
{
    handlers.insert(P_HandlerDirect(1, std::bind(&DirectConn::SyncData, this)));
    handlers.insert(P_HandlerDirect(2, std::bind(&DirectConn::SyncPorts, this)));
    handlers.insert(P_HandlerDirect(3, std::bind(&DirectConn::SyncBitrate, this)));
    handlers.insert(P_HandlerDirect(6, std::bind(&DirectConn::ChatMessage, this)));

    connThread = Thread_ptr(new std::thread(&DirectConn::WaitConnection, this));
    connThread->detach();
}

DirectConn &DirectConn::Get()
{
    static DirectConn dc;
    return dc;
}

void DirectConn::WaitConnection()
{
    try
    {
        acceptor = std::make_shared<tcp::acceptor>(tcp_service, tcp::endpoint(tcp::v4(), 25560));
        s_remote = TCP_socketptr(new tcp::socket(tcp_service));

        NDNS::Get().WriteOutput("Host started.", SERVER);
        acceptor->async_accept(*s_remote, boost::bind(&DirectConn::AcceptedSocket, this, boost::placeholders::_1));
        tcp_service.run_one();
        tcp_service.restart();
    }
    catch (const boost::system::system_error &e)
    {
        NDNS::Get().WriteOutput("Host cannot be started: host endpoint(0.0.0.0:25560) is closed.", SERVER);
        return;
    }
}

void DirectConn::AcceptedSocket(const boost::system::error_code &error)
{
    if (!error)
    {
        NDNS::Get().WriteOutput("Connected!", SERVER);
        state = CONNECTED;
        Setup();
    }
    else if (error != boost::asio::error::operation_aborted)
    {
        NDNS::Get().WriteOutput("Cannot accept socket.\nReason: " + error.message(), SERVER);
    }
}

void DirectConn::Connect(std::string rem)
{
    if (state == WAITING)
    {
        if (acceptor)
        {
            // В случае локального дебага на стороне клиента хост не будет запущен и локальное соединение можно будет установить. Иначе - смерть.
            if (rem == "127.0.0.1")
            {
                NDNS::Get().WriteOutput("Cannot establish connection to local host instance. Start other instance of NDNS and try connect.", ERROR);
                return;
            }

            acceptor->close();
            acceptor = nullptr;
        }
        connThread = nullptr;

        try
        {
            s_remote = TCP_socketptr(new tcp::socket(tcp_service));
            s_remote->connect(tcp::endpoint(ip::address::from_string(rem), 25560));

            NDNS::Get().WriteOutput("Connected!", SERVER);
            state = CONNECTED;

            connThread = Thread_ptr(new std::thread(&DirectConn::Setup, this));
            connThread->detach();
        }
        catch (const std::exception &e)
        {
            NDNS::Get().WriteOutput(std::string("Cannot connect to remote host.\nReason: ") + e.what(), ERROR);
        }
    }
    else
    {
        NDNS::Get().WriteOutput("Connection already established.", ERROR);
    }
}

void DirectConn::Setup()
{
    // auto nick = Settings::Get().GetField(S_LAST_NICKNAME)->GetValue();
    // Send(SYNC_DATA, nick.data(), 16);

    s_voice = UDP_socketptr(new udp::socket(udp_service, udp::endpoint(udp::v4(), 0)));
    s_trans = UDP_socketptr(new udp::socket(udp_service, udp::endpoint(udp::v4(), 0)));

    auto psyn = std::make_shared<port_sync>();
    psyn->a_p = s_voice->local_endpoint().port();
    psyn->v_p = s_trans->local_endpoint().port();

    Send(SYNC_UDP_PORT, psyn.get(), 4);

    if (state == CONNECTED)
        HandleConnection();
}

void DirectConn::HandleConnection()
{
    state = ESTABLISHED;
    while (state & (ESTABLISHED | MESSAGING))
    {
        try
        {
            uint8 *raw_code = new uint8;
            s_remote->read_some(buffer(raw_code, 1));
            auto code = static_cast<OPCODE>(*raw_code);
            delete raw_code;

            if (auto hndl = handlers.find(code); hndl != handlers.end())
            {
                hndl->second();
            }
            else
            {
                // write error code
            }
        }
        catch (std::exception &e)
        {

            NDNS::Get().WriteOutput("TCP connection dropped.", SERVER);
            state = ESTABLISHED;
            break;
        }
    }

    Reset();
}

void DirectConn::Reset()
{

    SDLAudioManager::Get().Stop();

    try
    {
        s_voice->shutdown(s_voice->shutdown_both);
        s_voice->close();

        s_trans->shutdown(s_trans->shutdown_both);
        s_trans->close();
    }
    catch (std::exception &e)
    {
    }

    voiceWriter = nullptr;
    voiceListener = nullptr;
    translationWriter = nullptr;
    translationListener = nullptr;

    if (state == MESSAGING)
    {
        NDNS::Get().WriteOutput("Restarting UDP...", SERVER);
        Setup();
        NDNS::Get().WriteOutput("UDP restarted.", SERVER);
    }
    else
    {
        if (state != WAITING)
        {
            state = WAITING;

            s_remote = nullptr;
            acceptor = nullptr;

            connThread = Thread_ptr(new std::thread(&DirectConn::WaitConnection, this));
            connThread->detach();
        }
    }
}

void DirectConn::RecordVoice()
{
    SDLAudioManager::Get().Start();

    while (state == MESSAGING)
    {
        int16* data = nullptr;

        try
        {
            data = SDLAudioManager::Get().RecordAudio();

            if (data != nullptr && !muteIn)
            {
                s_voice->send(buffer(data, AUDIO_BUF * 2));
            }
            std::this_thread::sleep_for(chrono::milliseconds(20));
        }
        catch (const std::exception &e)
        {
            NDNS::Get().WriteOutput("UDP Dropped", SERVER);
            Reset();
        }

        if(data) delete[] data;
    }

    SDLAudioManager::Get().Stop();
}

void DirectConn::ListenVoice()
{
    while (state == MESSAGING)
    {
        int16 *samples = nullptr;

        try
        {
            samples = new int16[AUDIO_BUF];
            s_voice->receive(buffer(samples, AUDIO_BUF * 2));

            if (!muteOut)
                SDLAudioManager::Get().PlayAudio(samples);

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        catch (const std::exception &e)
        {
            NDNS::Get().WriteOutput("UDP Dropped", SERVER);
            Reset();
        }

        if(samples) delete[] samples;
    }
}

void DirectConn::RecordTranslation()
{
    // while (state == MESSAGING)
    //     ;
}

void DirectConn::ListenTranslation()
{
    // while (state == MESSAGING)
    //     ;
}

void DirectConn::MuteInput()
{
    if (state == MESSAGING)
    {
        muteIn = !muteIn;
        if (muteIn)
        {
            NDNS::Get().WriteOutput("Microphone is muted!", SERVER);
        }
        else
        {
            NDNS::Get().WriteOutput("Microphone is unmuted!", SERVER);
        }
    }
}

void DirectConn::MuteOutput()
{
    if (state == MESSAGING)
    {
        muteOut = !muteOut;
        if (muteOut)
        {
            NDNS::Get().WriteOutput("Sound is muted!", SERVER);
        }
        else
        {
            NDNS::Get().WriteOutput("Sound is unmuted!", SERVER);
        }
    }
}