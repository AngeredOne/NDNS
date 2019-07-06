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
        audioStream = std::make_shared<AudioStream>();
        audioStream->Start();
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
        sf::Int16 *bits = new sf::Int16[bitrate];
        UDPEndPoint senderEP;
        std::cout << "";
        socket->receive_from(asio::buffer(bits, bitrate), senderEP);
        audioStream->InterpretSamples(bits, bitrate);
    }
}

void VoiceClient::InterpretSamples(sf::Int16 *samples, std::size_t size)
{
    if (remoteEndPoint)
    {
    //     sf_compressor_state_st cm_state;

    //     sf_simplecomp(&cm_state, 1024, 5, 3, 1, 2, 3, 5);

    //     sf_sample_st *cmp_samples = new sf_sample_st[size];
    //     sf_sample_st *cmp_samples_out = new sf_sample_st[size];

    //     for (int i = 0; i < size; ++i)
    //     {
    //         cmp_samples[i].L = samples[i];
    //         cmp_samples[i].R = samples[i];
    //     }

    //     sf_compressor_process(&cm_state, size, cmp_samples, cmp_samples_out);
    //     sf::Int16* smpls = new sf::Int16[size];
    //    for (int i = 0; i < size; i++)
    //     {
    //         float L = clampf(cmp_samples_out[i].L, -1, 1);
    //         float R = clampf(cmp_samples_out[i].R, -1, 1);
    //         int16_t Lv, Rv;
    //         // once again, int16 samples range from -32768 to 32767, so we need to scale the floating
    //         // point sample by a different factor depending on whether it's negative
    //         if (L < 0)
    //             Lv = (int16_t)(L * 32768.0f);
    //         else
    //             Lv = (int16_t)(L * 32767.0f);
    //         if (R < 0)
    //             Rv = (int16_t)(R * 32768.0f);
    //         else
    //             Rv = (int16_t)(R * 32767.0f);

    //         socket->send_to(asio::buffer(samples, size), *remoteEndPoint);
    //         smpls[i] = (sf::Int16)Rv;
    //     }

        //socket->send_to(asio::buffer(samples, size), *remoteEndPoint);
        audioStream->InterpretSamples(samples, bitrate);
    }
}