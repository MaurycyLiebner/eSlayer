#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"

#include <eSlayerNet/etcpnetwork.h>

#include <iostream>
#include <ostream>

const int TCP_IP_PORT = 4000;
const int PACKET_CHAT = 0;

std::shared_ptr<eServer> eSlayerServer::generate(
    const eServerData& data) {
    if(data.fName == "single_player") {
        return std::make_shared<eSinglePlayerServer>();
    } else if(data.fName == "tcp_ip_host") {
        std::cout << "Host initialization..." << std::endl;

        eTCPNetwork net;

        net.init();
        net.startServer(TCP_IP_PORT);

        while(true) {
            net.update();

            eNetPacket pkt;

            while(net.pollPacket(pkt)) {
                uint16_t type;
                pkt.fPacket >> type;

                if(type == PACKET_CHAT) {
                    std::string text;
                    pkt.fPacket >> text;

                    std::cout << "Client "
                              << pkt.fClientID
                              << ": " << text << std::endl;

                    ePacket reply(PACKET_CHAT);
                    reply << std::string("Hello client!");

                    net.broadcast(reply);
                }
            }

            SDL_Delay(16);
        }
        return std::make_shared<eSinglePlayerServer>();
    } else if(data.fName == "tcp_ip_join") {
        eTCPNetwork net;

        net.init();
        net.connect(data.fIp.data(), TCP_IP_PORT);

        ePacket p(PACKET_CHAT);
        p << std::string("Hello server!");

        net.sendToServer(p);

        while(true) {
            net.update();

            eNetPacket pkt;

            while(net.pollPacket(pkt)) {
                uint16_t type;
                pkt.fPacket >> type;

                if(type == PACKET_CHAT) {
                    std::string text;
                    pkt.fPacket >> text;

                    std::cout << "Server: "
                              << text << std::endl;
                }
            }

            SDL_Delay(16);
        }
        return std::make_shared<eSinglePlayerServer>();
    }
    return nullptr;
}
