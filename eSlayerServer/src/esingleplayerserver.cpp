#include "esingleplayerserver.h"

bool eSinglePlayerServer::sendMessage(
    const int clientId, const std::string& text) {
    mMessages.emplace_back(clientId, text);
    return true;
}
