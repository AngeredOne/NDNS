#include "VoiceClient.h"
#include "NDNS.h"

void TCPClient::HandleMessage()
{
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
        {
            auto data = new int16[2];
            read(*with, buffer(data, 4));

            auto ports = reinterpret_cast<port_sync *>(data);
            voiceClient->Create(ports->a_p, ports->v_p, with->remote_endpoint().address().to_string());
        }

        break;

        case 3:
        {
            auto data = new int16[2];
            read(*with, buffer(data, 4));

            auto ports = reinterpret_cast<port_sync *>(data);
            if (!voiceClient->IsConnected())
                voiceClient->Create(ports->a_p, ports->v_p, with->remote_endpoint().address().to_string());
            else
                voiceClient->GetVoiceSockets()->RegEP(ports->a_p, ports->v_p, with->remote_endpoint().address().to_string());
        }
        case 11:
        {
            std::string message;

            while (message.find(">-<") == std::string::npos)
            {
                char *msg = new char[8];
                with->read_some(buffer(msg, 8));
                message += std::string(msg, 8).c_str();
                delete msg;
            }

            message = voiceClient->Nick + ": " + message.erase(message.find(">-<"), 3);
            NDNS::Get().WriteOutput(message, CHAT);
            message = "";
        }
        break;
        }
    }
}