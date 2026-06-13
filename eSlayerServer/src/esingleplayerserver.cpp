#include "esingleplayerserver.h"

bool eSinglePlayerServer::sendMessage(
    const uint32_t clientId, const std::string& text) {
    mMessages.emplace_back(clientId, text);
    return true;
}
