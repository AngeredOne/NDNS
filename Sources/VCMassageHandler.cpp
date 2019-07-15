#include "VoiceClient.h"
#include "NDNS.h"

void TCPClient::HandleMessage()
{
    while (connected)
    {
        try
        {
            int16 *code_r = new int16;
            read(*with, buffer(code_r, 2));

            int code = *code_r;
            switch (code)
            {
                //Set conn + nick
            case 1:
            {
                auto data = new int8[16];
                read(*with, buffer(data, 16));
                nick = std::string(std::string(data, 16).c_str());
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
        catch (std::exception &e)
        {
            NDNS::Get().WriteOutput("Client disconnected!", SERVER);
            connected = false;
            
            break;
        }
    }
}