#ifndef ESINGLEPLAYERSERVER_H
#define ESINGLEPLAYERSERVER_H

#include "elocalserver.h"

class eSinglePlayerServer : public eLocalServer {
public:
    using eLocalServer::eLocalServer;

    bool sendMessage(const int clientId,
                     const std::string& text) override;
};

#endif // ESINGLEPLAYERSERVER_H
