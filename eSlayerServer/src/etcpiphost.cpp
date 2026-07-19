#include "etcpiphost.h"

#include "epacketdata.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/edoors.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eversion.h>

eTcpIpHost::~eTcpIpHost() {
    eTcpIpHost::disconnect(mClientId);
}

bool eTcpIpHost::disconnect(const uint32_t clientId) {
    if(!mInitialized) return false;
    if(mRunning) {
        mRunning = false;
        mThread.join();
    }
    ePacket p;
    p << ePacketType::disconnect;
    for(const auto& c : mClientIdMap) {
        const int tcpClientId = c.first;
        mNet.sendToClient(tcpClientId, p);
    }

    uint32_t time = 0;
    while(!mClientIdMap.empty()) {
        mNet.update();

        eNetPacket pkt;

        while(mNet.pollPacket(pkt)) {
            auto& p = pkt.fPacket;
            const int tcpClientId = pkt.fClientID;
            ePacketType type;
            p >> type;

            if(type == ePacketType::disconnect) {
                mClientIdMap.erase(tcpClientId);
            }
        }

        SDL_Delay(16);
        time += 16;
        if(time > 2000) {
            break;
        }
    }

    mNet.shutdown();

    mInitialized = false;

    return true;
}

bool eTcpIpHost::initialize() {
    eLocalServer::initialize();
    mInitialized = mNet.init();
    if(!mInitialized) {
        failed("Disconnected", "Failed to initialize SDL3_net.");
        return false;
    }
    const bool r = mNet.startServer(4000);
    if(!r) failed("Disconnected", "Failed to create the host server.");

    mRunning = true;
    const float fpsClamp = eRunSettings::sFPS;
    const float by = 25.f/fpsClamp;
    mThread = std::thread([this, fpsClamp, by]() {
        threadWork(fpsClamp, by);
    });

    return r;
}

void eTcpIpHost::increment(const float by) {
    checkMapsReady();
    sendSlain();
}

bool eTcpIpHost::sendMessage(const uint32_t clientId,
                             const std::string& text) {
    std::unique_lock lock(mMutex);
    sendMessageToAll(clientId, text);
    return true;
}

bool eTcpIpHost::requestMap(
    const uint32_t clientId,
    const eMoveToMapData& moveData,
    const eMapReadyAction& func) {
    std::unique_lock lock(mMutex);
    return eLocalServer::requestMap(
        clientId, moveData, func);
}

bool eTcpIpHost::spawn(
    const uint32_t clientId,
    eCharacter& c,
    eTeamId& teamId,
    ePointF& spawnPos,
    const eScreenDimensions& screenDims) {
    std::unique_lock lock(mMutex);
    return eLocalServer::spawn(
        clientId, c, teamId, spawnPos, screenDims);
}

bool eTcpIpHost::requestData(
    const uint32_t clientId,
    eRequestData& data,
    float& resultTime) {
    std::unique_lock lock(mMutex);
    return eLocalServer::requestData(
        clientId, data, resultTime);
}

bool eTcpIpHost::requestEquipment(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::requestEquipment(
        clientId);
}

bool eTcpIpHost::receiveEquipment(
    const uint32_t clientId, eEquipment& data) {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveEquipment(
        clientId, data);
}

bool eTcpIpHost::unblockEquipment(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::unblockEquipment(
        clientId);
}

std::optional<eSlayerQuests>
eTcpIpHost::receiveQuests(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveQuests(
        clientId);
}

bool eTcpIpHost::heardTalk(
    const uint32_t clientId,
    const eConvoId& talk) {
    std::unique_lock lock(mMutex);
    return eLocalServer::heardTalk(
        clientId, talk);
}

bool eTcpIpHost::addedSocket(
    const uint32_t clientId,
    const uint8_t questId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::addedSocket(
        clientId, questId);
}

bool eTcpIpHost::changeState(
    const uint32_t clientId, const eUnitData& u) {
    std::unique_lock lock(mMutex);
    return eLocalServer::changeState(
        clientId, u);
}

bool eTcpIpHost::attack(
    const uint32_t clientId, const eAttackData& target) {
    std::unique_lock lock(mMutex);
    return eLocalServer::attack(
        clientId, target);
}

bool eTcpIpHost::stopAttack(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::stopAttack(clientId);
}

bool eTcpIpHost::createBody(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::createBody(
        clientId);
}

bool eTcpIpHost::setSkillId(
    const uint32_t clientId,
    const eSkillChoice schoice,
    const int skillId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::setSkillId(
        clientId, schoice, skillId);
}

bool eTcpIpHost::triggerDoors(
    const uint32_t clientId,
    const eServerDoors& doors) {
    std::unique_lock lock(mMutex);
    return triggerDoorsAndSend(
        clientId, doors);
}

bool eTcpIpHost::pickupItem(
    const uint32_t clientId,
    const uint32_t itemId,
    const bool drag) {
    std::unique_lock lock(mMutex);
    return eLocalServer::pickupItem(
        clientId, itemId, drag);
}

bool eTcpIpHost::dropItem(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::dropItem(
        clientId);
}

bool eTcpIpHost::rearrangeItems(
    const uint32_t clientId,
    const eEquipment& eq) {
    std::unique_lock lock(mMutex);
    return eLocalServer::rearrangeItems(
        clientId, eq);
}

bool eTcpIpHost::changeAttributes(
    const uint32_t clientId,
    const eAttributes& attrs) {
    std::unique_lock lock(mMutex);
    return eLocalServer::changeAttributes(
        clientId, attrs);
}

bool eTcpIpHost::changeSkillLevels(
    const uint32_t clientId,
    const eSkillLevels& skillLevels) {
    std::unique_lock lock(mMutex);
    return eLocalServer::changeSkillLevels(
        clientId, skillLevels);
}

bool eTcpIpHost::consumePotion(
    const uint32_t clientId,
    const uint32_t itemId,
    const uint32_t unitId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::consumePotion(
        clientId, itemId, unitId);
}

bool eTcpIpHost::pickupBody(
    const uint32_t clientId,
    const uint32_t bodyId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::pickupBody(
        clientId, bodyId);
}

bool eTcpIpHost::buyAction(
    const uint32_t clientId,
    const eBuyAction& a) {
    std::unique_lock lock(mMutex);
    return eLocalServer::buyAction(
        clientId, a);
}

bool eTcpIpHost::sellAction(
    const uint32_t clientId,
    const eSellAction& a) {
    std::unique_lock lock(mMutex);
    return eLocalServer::sellAction(
        clientId, a);
}

bool eTcpIpHost::requestSeller(
    const uint32_t clientId,
    const uint32_t sellerId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::requestSeller(
        clientId, sellerId);
}

bool eTcpIpHost::summonMerc(
    const uint32_t clientId,
    const eMercenary& merc) {
    std::unique_lock lock(mMutex);
    return eLocalServer::summonMerc(
        clientId, merc);
}

std::optional<eFollowersBase>
eTcpIpHost::followersUpdate(const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::followersUpdate(clientId);
}

void eTcpIpHost::sendSlain() {
    auto& ss = eServerArea::sSlain;
    if(ss.empty()) return;
    std::unique_lock lock(mMutex);
    for(const auto s : ss) {
        ePacket p;
        p << ePacketType::slain;
        p << s;
        mNet.broadcast(p);

        auto& ss = eSlayers::sSlayers;
        const auto sit = ss.find(s);
        if(sit != ss.end()) {
            const auto& s = sit->second;
            mSlainUsers.emplace_back(s);
        }
    }
    ss.clear();
}

void eTcpIpHost::checkMapsReady() {
    std::unique_lock lock(mMutex);
    return eLocalServer::checkMapsReady();
}

bool eTcpIpHost::teamAction(
    const uint32_t clientId,
    const eTeamAction& action) {
    std::unique_lock lock(mMutex);
    return eLocalServer::teamAction(
        clientId, action);
}

bool eTcpIpHost::spawnPortal(
    const uint32_t clientId) {
    std::unique_lock lock(mMutex);
    return eLocalServer::spawnPortal(
        clientId);
}

bool eTcpIpHost::equipmentAction(
    const uint32_t clientId,
    const eEquipmentAction& a) {
    std::unique_lock lock(mMutex);
    return eLocalServer::equipmentAction(
        clientId, a);
}

bool eTcpIpHost::triggerObject(
    const uint32_t clientId, const eServerObject& obj) {
    std::unique_lock lock(mMutex);
    return triggerObjectAndSend(clientId, obj);
}

void eTcpIpHost::sendMessageToAll(
    const uint32_t clientId, const std::string& text) {
    ePacket p;
    p << ePacketType::message;
    p << clientId;
    p << text;
    mNet.broadcast(p);

    mMessages.emplace_back(clientId, text);
}

bool eTcpIpHost::handleClientDisconnect(const int tcpClientId) {
    const auto it = mClientIdMap.find(tcpClientId);
    if(it == mClientIdMap.end()) return false;
    const uint32_t charId = it->second;
    mClientIdMap.erase(it);
    eLocalServer::disconnect(charId);
    mNet.removeClientByTcpId(tcpClientId);
    {
        ePacket p;
        p << ePacketType::userLeft;
        p << charId;
        mNet.broadcast(p);
    }
    auto& ss = eSlayers::sSlayers;
    const auto sit = ss.find(charId);
    if(sit != ss.end()) {
        const auto& s = sit->second;
        mLeftUsers.emplace_back(s);
        ss.erase(sit);
    }
    return true;
}

void eTcpIpHost::threadWork(const float fpsClamp, const float by) {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    while(mRunning) {
        const auto fpsStart = high_resolution_clock::now();

        {
            std::unique_lock lock(mMutex);
            eLocalServer::increment(by);
            mNet.update();
            eNetPacket pkt;
            while(mNet.pollPacket(pkt)) {
                processPacket(pkt);
            }

            if(eTeams::version() > mTeamsVersion) {
                ePacket p;
                p << ePacketType::teams;
                eTeams::write(p);
                mNet.broadcast(p);
                mTeamsVersion = eTeams::version();
            }

            if(ePortal::version() > mPortalsVersion) {
                ePacket p;
                p << ePacketType::portals;
                ePortal::write(p);
                mNet.broadcast(p);
                mPortalsVersion = ePortal::version();
            }

            const auto tcpIds = mNet.removeDisconnectedClients();
            for(const int tcpClientId : tcpIds) {
                handleClientDisconnect(tcpClientId);
            }
        }

        const auto fpsEnd = high_resolution_clock::now();
        const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
        const duration<double, std::milli> fpsDuration(1000./fpsClamp);
        const duration<double, std::milli> fpsSleep(fpsDuration - fpsElapsed);
        std::this_thread::sleep_for(fpsSleep);
    }
}

void eTcpIpHost::processPacket(eNetPacket& pkt) {
    const int tcpClientId = pkt.fClientID;
    auto& p = pkt.fPacket;
    ePacketType type;
    p >> type;
    switch(type) {
    case ePacketType::connect: {
        const auto it = mClientIdMap.find(tcpClientId);
        int32_t clientId;
        if(it == mClientIdMap.end()) {
            clientId = connect();
            mClientIdMap[tcpClientId] = clientId;
        } else {
            clientId = it->second;
        }
        ePacket p;
        p << ePacketType::connect;
        p << clientId;
        p << eVersion;
        p << eDifficulties::sDifficulty;
        mNet.sendToClient(tcpClientId, p);
    } break;
    case ePacketType::map: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            eMoveToMapData moveData;
            p >> moveData;
            const uint32_t charId = it->second;
            const auto func = [this, tcpClientId](const eMapData& data) {
                const auto it = mClientIdMap.find(tcpClientId);
                if(it == mClientIdMap.end()) return;
                ePacket p;
                p << ePacketType::map;
                data.write(p);
                mNet.sendToClient(tcpClientId, p);
            };
            eLocalServer::requestMap(charId, moveData, func);
        }
    } break;
    case ePacketType::spawn: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eCharacter c;
            c.read(p);
            eScreenDimensions screenDims;
            screenDims.read(p);
            eTeamId teamId;
            ePointF spawnPos;
            std::vector<eBody> bodies;
            const bool r = eLocalServer::spawnImpl(
                charId, c, teamId, spawnPos, bodies, screenDims);

            if(r) {
                {
                    ePacket p;
                    p << ePacketType::spawn;

                    eSlayers::write(p);

                    const auto& eq = c.equipment();
                    eq.writeIds(p);

                    eTeams::write(p);
                    ePortal::write(p);
                    p << teamId;
                    p << spawnPos;
                    mNet.sendToClient(tcpClientId, p);
                }

                for(const auto& b : bodies) {
                    ePacket p;
                    p << ePacketType::body;
                    b.write(p);
                    mNet.sendToClient(tcpClientId, p);
                }

                {
                    ePacket p;
                    p << ePacketType::userEntered;
                    const auto it = eSlayers::sSlayers.find(charId);
                    if(it != eSlayers::sSlayers.end()) {
                        const auto& slayer = it->second;
                        slayer.write(p);
                        mNet.broadcast(p);

                        mNewUsers.emplace_back(slayer);
                    }
                }
            }
        }
    } break;
    case ePacketType::state: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eUnitData u;
            u.read(p);
            eLocalServer::changeState(charId, u);
        }
    } break;
    case ePacketType::data: {

    } break;
    case ePacketType::userEntered: {

    } break;
    case ePacketType::slain: {

    } break;
    case ePacketType::userLeft: {

    } break;
    case ePacketType::unblockEquipment: {

    } break;
    case ePacketType::equipment: {

    } break;
    case ePacketType::quests: {

    } break;
    case ePacketType::replaceItemId: {

    } break;
    case ePacketType::provideSeller: {

    } break;
    case ePacketType::followers: {

    } break;
    case ePacketType::heardTalk: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eConvoId talk;
            p >> talk;
            eLocalServer::heardTalk(charId, talk);
        }
    } break;
    case ePacketType::addedSocket: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            uint8_t questId;
            p >> questId;
            eLocalServer::addedSocket(charId, questId);
        }
    } break;
    case ePacketType::buyAction: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eBuyAction a;
            p >> a;
            uint32_t newItemId = 0;
            const bool r = eLocalServer::buyActionImpl(
                charId, a, newItemId);
            if(r) {
                if(newItemId != 0) {
                    ePacket p;
                    p << ePacketType::replaceItemId;
                    eReplaceItemId i;
                    i.fSellerId = a.fSellerId;
                    i.fOldItemId = a.fItemId;
                    i.fNewItemId = newItemId;
                    p << i;
                    mNet.sendToClient(tcpClientId, p);
                }
            } else {
                synchronizeEq(charId, tcpClientId);
            }
        }
    } break;
    case ePacketType::sellAction: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eSellAction a;
            p >> a;
            eLocalServer::sellAction(
                charId, a);
        }
    } break;
    case ePacketType::requestSeller: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            uint32_t sellerId;
            p >> sellerId;
            eSeller s;
            const bool r = eLocalServer::requestSellerImpl(
                charId, sellerId, s);
            if(r) {
                ePacket p;
                p << ePacketType::provideSeller;
                s.write(charId, p);
                mNet.sendToClient(tcpClientId, p);
            }
        }
    } break;
    case ePacketType::summonMerc: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eMercenary merc;
            merc.read(p);
            eLocalServer::summonMerc(charId, merc);
        }
    } break;
    case ePacketType::equipmentAction: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eEquipmentAction a;
            a.read(p);
            const bool r = eLocalServer::equipmentAction(charId, a);
            if(!r) {
                synchronizeEq(charId, tcpClientId);
            }
        }
    } break;
    case ePacketType::body: {

    } break;
    case ePacketType::bodyPickedUp: {

    } break;
    case ePacketType::teams: {

    } break;
    case ePacketType::portals: {

    } break;
    case ePacketType::objectStateChanged: {

    } break;
    case ePacketType::doorsStateChanged: {

    } break;
    case ePacketType::message: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            std::string msg;
            p >> msg;
            sendMessageToAll(charId, msg);
        }
    } break;
    case ePacketType::teamsAction: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eTeamAction action;
            p >> action;
            eLocalServer::teamAction(charId, action);
        }
    } break;
    case ePacketType::spawnPortal: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eLocalServer::spawnPortal(charId);
        }
    } break;
    case ePacketType::request: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;

            {
                const auto q = eLocalServer::receiveQuests(charId);
                if(q) {
                    ePacket p;
                    p << ePacketType::quests;
                    q->write(p);
                    mNet.sendToClient(tcpClientId, p);
                }
            }
            {
                const auto f = eLocalServer::followersUpdate(charId);
                if(f) {
                    ePacket p;
                    p << ePacketType::followers;
                    f->write(p);
                    mNet.sendToClient(tcpClientId, p);
                }
            }

            eRequestData data;
            p >> data.fRequestId;
            p >> data.fServerState;
            float time;
            const bool r = eLocalServer::requestData(charId, data, time);
            if(!r) return;
            {
                ePacket p;
                p << ePacketType::data;
                data.write(p);
                mNet.sendToClient(tcpClientId, p);
            }
        }
    } break;
    case ePacketType::attack: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            eAttackData data;
            data.read(p);
            const uint32_t charId = it->second;
            eLocalServer::attack(charId, data);
        }
    } break;
    case ePacketType::stopAttack: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eLocalServer::stopAttack(charId);
        }
    } break;
    case ePacketType::createBody: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eBody body;
            const bool r = eLocalServer::createBodyImpl(
                charId, body);
            if(r) {
                ePacket p;
                p << ePacketType::body;
                body.write(p);
                mNet.sendToClient(tcpClientId, p);
            }
        }
    } break;
    case ePacketType::setSkillId: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eSkillChoice schoice;
            p >> schoice;
            int skillId;
            p >> skillId;
            eLocalServer::setSkillId(
                charId, schoice, skillId);
        }
    } break;
    case ePacketType::disconnect: {
        handleClientDisconnect(tcpClientId);
    } break;
    case ePacketType::triggerObject: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eServerObject obj;
            p >> obj;
            triggerObjectAndSend(charId, obj);
        }
    } break;
    case ePacketType::triggerDoors: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eServerDoors doors;
            doors.read(p);
            triggerDoorsAndSend(charId, doors);
        }
    } break;
    case ePacketType::dropItem: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eLocalServer::dropItem(charId);
        }
    } break;
    case ePacketType::dropGold: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            uint32_t count;
            p >> count;
            eLocalServer::dropGold(charId, count);
        }
    } break;
    case ePacketType::pickupItem: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            uint32_t itemId;
            p >> itemId;
            bool drag;
            p >> drag;
            eEquipmentAction action;
            const bool r = eLocalServer::pickupItemImpl(
                charId, itemId, drag, action);
            ePacket p;
            if(r) {
                p << ePacketType::equipmentAction;
                action.write(p);
            } else {
                p << ePacketType::unblockEquipment;
            }
            mNet.sendToClient(tcpClientId, p);
        }
    } break;
    case ePacketType::pickupBody: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            uint32_t bodyId;
            p >> bodyId;
            bool bodyRemoved;
            eBodyItemsTaken taken;
            const bool r = eLocalServer::pickupBodyImpl(
                charId, bodyId, bodyRemoved, taken);
            if(r) {
                ePacket p;
                p << ePacketType::bodyPickedUp;
                p << bodyRemoved;
                p << bodyId;
                taken.write(p);
                mNet.sendToClient(tcpClientId, p);
            } else {
                ePacket p;
                p << ePacketType::unblockEquipment;
                mNet.sendToClient(tcpClientId, p);
            }
        }
    } break;
    case ePacketType::rearrangeItems: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eEquipment eq;
            eq.read(p);
            eLocalServer::rearrangeItems(
                charId, eq);
        }
    } break;
    case ePacketType::consumePotion: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            uint32_t itemId;
            p >> itemId;
            uint32_t unitId;
            p >> unitId;
            eLocalServer::consumePotion(
                charId, itemId, unitId);
        }
    } break;
    case ePacketType::attributes: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eAttributes attrs;
            p >> attrs;
            eLocalServer::changeAttributes(
                charId, attrs);
        }
    } break;
    case ePacketType::skills: {
        const auto it = mClientIdMap.find(tcpClientId);
        if(it != mClientIdMap.end()) {
            const uint32_t charId = it->second;
            eSkillLevels skillLevels;
            skillLevels.read(p);
            eLocalServer::changeSkillLevels(
                charId, skillLevels);
        }
    } break;
    }
}

bool eTcpIpHost::triggerDoorsAndSend(
    const uint32_t clientId, const eServerDoors& doors) {
    const int mapId = clientMapId(clientId);
    if(mapId < 0) return false;
    if(mapId != doors.fMapId) return false;
    const bool r = eLocalServer::triggerDoors(clientId, doors);
    if(!r) return false;
    ePacket p;
    p << ePacketType::doorsStateChanged;
    doors.write(p);
    sendToMapClients(mapId, p);
    return true;
}

bool eTcpIpHost::triggerObjectAndSend(
    const uint32_t clientId, const eServerObject& obj) {
    const int mapId = clientMapId(clientId);
    if(mapId < 0) return false;
    if(mapId != obj.fMapId) return false;
    eServerObject cobj = obj;
    const bool r = eLocalServer::triggerObjectImpl(clientId, cobj);
    if(!r) return false;
    ePacket p;
    p << ePacketType::objectStateChanged;
    p << cobj;
    sendToMapClients(mapId, p);
    return true;
}

void eTcpIpHost::sendToMapClients(
    const uint8_t mapId, const ePacket& p) {
    for(const auto& it : mClientIdMap) {
        const uint32_t clientId = it.second;
        const int cMapId = clientMapId(clientId);
        if(cMapId != mapId) continue;
        const int tcpClientId = it.first;
        mNet.sendToClient(tcpClientId, p);
    }
}

bool eTcpIpHost::synchronizeEq(
    const uint32_t clientId,
    const int tcpClientId) {
    eEquipment data;
    const bool r = eLocalServer::receiveEquipment(
        clientId, data);
    if(!r) return false;
    ePacket p;
    p << ePacketType::equipment;
    data.write(p);
    mNet.sendToClient(tcpClientId, p);
    return true;
}

std::vector<eSlayer>
eTcpIpHost::receiveNewUsers() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveNewUsers();
}

std::vector<eSlayer>
eTcpIpHost::receiveLeftUsers() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveLeftUsers();
}

std::vector<eSlayer>
eTcpIpHost::receiveSlainUsers() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveSlainUsers();
}

std::vector<eMessage>
eTcpIpHost::receiveMessages() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveMessages();
}

std::vector<eServerObject>
eTcpIpHost::receiveObjectStateChanges() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveObjectStateChanges();
}

std::vector<eServerDoors>
eTcpIpHost::receiveDoorsStateChanges() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveDoorsStateChanges();
}

std::vector<uint32_t>
eTcpIpHost::receiveBodiesPickedUp() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveBodiesPickedUp();
}

std::vector<eBody>
eTcpIpHost::receiveBodiesCreated() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveBodiesCreated();
}

std::vector<eBodyItemsTaken>
eTcpIpHost::receiveBodiesChanged() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveBodiesChanged();
}

std::vector<eEquipmentAction>
eTcpIpHost::receiveEqActions() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveEqActions();
}

std::optional<eSeller>
eTcpIpHost::receiveSeller() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveSeller();
}

std::optional<eReplaceItemId>
eTcpIpHost::receiveReplaceItemId() {
    std::unique_lock lock(mMutex);
    return eLocalServer::receiveReplaceItemId();
}