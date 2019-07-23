#include "Definitions.h"

enum ConnState
{
    WAITING = 1,     // Ожидается подключение
    CONNECTED = 2,   // TCP соединение произведено - доступен чат, открываются локальные сокеты UDP
    ESTABLISHED = 4, // UDP соединение установлено - доступен звонок и трансляция экрана
    MESSAGING = 8    // В процессe общения по UDP
};

enum OPCODE : uint8
{
    SYNC_DATA = 1,     // Синхронизация пользовательской даты(такой как никнейм, аватар и прочее)
    SYNC_UDP_PORT = 2, // Синхронизация портов UDP для включения аудиообщения\трансляции экрана
    SYNC_BITRATE,      // Синхронизация буфферов звука a.k.a глубина дискретизации кратный 2 байтам(размер сэмпла)
    START_VOICE,       // Запустить звукопередачу
    START_TRANSLATION,  // Запустить трансляцию экрана
    CHATMSG 
};

struct port_sync
{
    uint16 a_p;
    uint16 v_p;
};

typedef std::pair<uint8, std::function<void()>> P_HandlerDirect;

class DirectConn
{
public:
    static DirectConn &Get();

    void Connect(std::string remote_ip);

    template <typename T>
    void Send(uint8 code, T *data, size_t length)
    {
        if (state & (CONNECTED | ESTABLISHED | MESSAGING) && s_remote != nullptr && data != nullptr)
        {
            int8 *raw_data = new int8[length + 1];

            memcpy(raw_data, &code, 1);
            memcpy(&raw_data[1], data, length);

            s_remote->write_some(buffer(raw_data, length + 1));
            delete raw_data;
        }
    };

    void MuteInput();
    void MuteOutput();

private:
    DirectConn();
    DirectConn(const DirectConn &) = delete;
    DirectConn operator=(const DirectConn &) = delete;

    std::shared_ptr<tcp::acceptor> acceptor = nullptr;
    io_service tcp_service;
    io_service udp_service;

    void WaitConnection();
    void AcceptedSocket(const boost::system::error_code &error);
    void HandleConnection();

    void Setup();
    void Reset();

    TCP_socketptr s_remote = nullptr; // remote tcp socket

    UDP_socketptr s_voice = nullptr; // voice udp socket
    UDP_socketptr s_trans = nullptr; // translation udp socket

    Thread_ptr connThread = nullptr; // Thread for waiter(host) or for connect handler.

    Thread_ptr voiceWriter = nullptr;
    Thread_ptr voiceListener = nullptr;
    Thread_ptr translationWriter = nullptr;
    Thread_ptr translationListener = nullptr;

    void ListenVoice();
    void RecordVoice();

    void ListenTranslation();
    void RecordTranslation();

    std::map<uint8, std::function<void()>> handlers;

    ConnState state = WAITING;

    bool muteIn = false;
    bool muteOut = false;

    // Handlers

    void SyncData();
    void SyncPorts();
    void SyncBitrate();
    void StartVoice();
    void ChatMessage();

    // END Handlers
};