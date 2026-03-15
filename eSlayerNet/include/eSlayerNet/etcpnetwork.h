#ifndef ETCPNETWORK_H
#define ETCPNETWORK_H

#include "eslayernetexport.h"

#include "epacket.h"

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

#include <vector>
#include <queue>
#include <mutex>

struct eNetPacket {
    int fClientID;
    ePacket fPacket;
};

struct eClient {
    int fId;
    NET_StreamSocket* fSocket;
    std::vector<uint8_t> fRecvBuffer;
};

class ESLAYERNET_API eTCPNetwork {
public:
    bool init();
    void shutdown();

    bool startServer(const uint16_t port);
    bool connect(const char* host, const uint16_t port);

    void update();

    void sendToServer(const ePacket& p);
    void sendToClient(const int id, const ePacket& p);
    void broadcast(const ePacket& p);

    bool pollPacket(eNetPacket& p);

    static std::vector<std::string> sGetLanIPs();
    static std::string sGetActiveLanIP();
private:
    void acceptClients();
    void receiveFromClients();
    void receiveFromServer();

    void receivePackets(NET_StreamSocket* const sock,
                        const int id,
                        std::vector<uint8_t>& buffer);
    void sendPacket(NET_StreamSocket* const sock,
                    const ePacket& p);
private:
    NET_Server* mServer = nullptr;
    NET_StreamSocket* mClientSocket = nullptr;

    std::vector<eClient> mClients;

    std::queue<eNetPacket> mPacketQueue;
    std::mutex mQueueMutex;

    int mNextClientID = 0;
};

#endif // ETCPNETWORK_H
