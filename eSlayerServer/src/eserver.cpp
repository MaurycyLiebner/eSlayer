#include "../include/eSlayerServer/eserver.h"

class eSinglePlayerServer : public eSlayerServer::eServer {
public:
    std::shared_ptr<eSlayerMapGenerator::eMap>
    requestMap(const std::string& name) override;
};

std::shared_ptr<eSlayerMapGenerator::eMap>
eSinglePlayerServer::requestMap(const std::string& name) {
    return eSlayerMapGenerator::generate(name);
}

std::shared_ptr<eSlayerServer::eServer>
eSlayerServer::generate(const std::string& name) {
    if(name == "single_player") {
        return std::make_shared<eSinglePlayerServer>();
    }
    return nullptr;
}
