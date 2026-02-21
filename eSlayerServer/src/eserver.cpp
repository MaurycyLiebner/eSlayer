#include "../include/eSlayerServer/eserver.h"

class eSinglePlayerServer : public eServer {
public:
    std::shared_ptr<eMap>
    requestMap(const std::string& name) override;
};

std::shared_ptr<eMap>
eSinglePlayerServer::requestMap(const std::string& name) {
    return eSlayerMapGenerator::generate(name);
}

std::shared_ptr<eServer>
eSlayerServer::generate(const std::string& name) {
    if(name == "single_player") {
        return std::make_shared<eSinglePlayerServer>();
    }
    return nullptr;
}
