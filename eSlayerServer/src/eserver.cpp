#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"

std::shared_ptr<eServer>
eSlayerServer::generate(const std::string& name) {
    if(name == "single_player") {
        return std::make_shared<eSinglePlayerServer>();
    }
    return nullptr;
}
