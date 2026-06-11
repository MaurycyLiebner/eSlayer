#include "epartywidget.h"

#include "egamewidget.h"
#include "../elabel.h"
#include "../mainMenu/emainmenubutton.h"
#include "../../etext.h"

#include <eSlayerHelpers/eteamid.h>

enum class eRelationship {
    party, hostile, neutral
};

class eTeamWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::vector<std::string>& members,
                    const std::string& button1Text,
                    const eAction& button1Action,
                    const std::string& button2Text,
                    const eAction& button2Action,
                    const eRelationship rel,
                    const bool darker) {
        const uint8_t alpha = darker ? 86 : 128;

        switch(rel) {
        case eRelationship::party:
            mBorderColor = SDL_Color{0, 255, 0, 255};
            mBgColor = SDL_Color{0, 255, 0, alpha};
            break;
        case eRelationship::hostile:
            mBorderColor = SDL_Color{255, 0, 0, 255};
            mBgColor = SDL_Color{255, 0, 0, alpha};
            break;
        case eRelationship::neutral:
            mBorderColor = SDL_Color{0, 0, 255, 255};
            mBgColor = SDL_Color{0, 0, 255, alpha};
            break;
        }

        const auto innerW = new eWidget(window());
        innerW->setNoPadding();
        const auto& res = resolution();
        const int p = res.smallPadding();
        const int w = width();
        const int h = height();
        const int iw = w - 2*p;
        const int ih = h - 2*p;
        innerW->resize(iw, ih);
        addWidget(innerW);
        innerW->align(eAlignment::center);

        const auto namesW = new eWidget(window());
        namesW->setNoPadding();

        for(const auto& m : members) {
            const auto nameLabel = new eLabel(window());
            nameLabel->setText(m);
            nameLabel->setSmallFontSize();
            nameLabel->setNoPadding();
            nameLabel->fitContent();
            namesW->addWidget(nameLabel);
        }

        innerW->addWidget(namesW);
        namesW->setHeight(ih);
        namesW->fitWidth();
        namesW->layoutVertically();

        const auto buttonsW = new eWidget(window());
        buttonsW->setNoPadding();
        buttonsW->setHeight(ih);

        if(button1Action) {
            const auto button = new eMainMenuButton(button1Text, window());
            button->setSmallFontSize();
            button->setPressAction(button1Action);
            buttonsW->addWidget(button);
            button->align(eAlignment::vcenter);
        }

        if(button2Action) {
            const auto button = new eMainMenuButton(button2Text, window());
            button->setSmallFontSize();
            button->setPressAction(button2Action);
            buttonsW->addWidget(button);
            button->align(eAlignment::vcenter);
        }

        buttonsW->stackHorizontally(p);
        buttonsW->fitWidth();
        innerW->addWidget(buttonsW);
        buttonsW->align(eAlignment::right);
    }
protected:
    void paintEvent(ePainter& p) {
        const auto& res = resolution();
        const int lineWidth = res.lineWidth();
        const auto rect = eWidget::rect();
        p.fillRect(rect, mBgColor);
        p.drawRect(rect, mBorderColor, lineWidth);
    }
private:
    SDL_Color mBgColor;
    SDL_Color mBorderColor;
};

void ePartyWidget::initialize(const std::string& name) {
    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    const auto nameLabel = new eLabel(window());
    nameLabel->setText(name);
    nameLabel->fitContent();
    innerW->addWidget(nameLabel);

    const auto& res = resolution();
    const auto mult = res.multiplier();
    mPlayerWidgetWidth = 400*mult;
    mPlayerWidgetHeight = 50*mult;

    mCenterWidget = new eWidget(window());
    mCenterWidget->setNoPadding();
    mCenterWidget->resize(mPlayerWidgetWidth,
                          8*mPlayerWidgetHeight);
    innerW->addWidget(mCenterWidget);

    innerW->stackVertically();
    innerW->fitContent();

    setup(innerW);

    updatePartyWidgets();
}

bool ePartyWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}

void ePartyWidget::paintEvent(ePainter& p) {
    const auto gw = eGameWidget::sInstance;
    const auto clientTeam = gw->team();
    if(eTeams::version() > mTeamsVersion ||
       mClientTeam != clientTeam) {
        mClientTeam = clientTeam;
        mTeamsVersion = eTeams::version();
        updatePartyWidgets();
    }
    eBgWidget::paintEvent(p);
}

void ePartyWidget::updatePartyWidgets() {
    mCenterWidget->removeAllWidgets();
    const auto& teams = eTeams::teams();
    const auto gw = eGameWidget::sInstance;
    const auto clientTeam = gw->team();
    const auto& server = gw->server();
    const int clientId = gw->clientId();
    bool darker = true;
    for(const auto& it : teams) {
        darker = !darker;
        const auto teamId = it.first;
        const auto& team = it.second;
        std::vector<std::string> members;
        const auto& ms = team.fMembers;
        for(const auto m : ms) {
            const auto& name = eGameWidget::sUserNames[m];
            members.emplace_back(name);
        }
        const auto teamW = new eTeamWidget(window());
        const int h = members.size()*mPlayerWidgetHeight;
        teamW->resize(mPlayerWidgetWidth, h);
        std::string button1Text;
        eAction button1Action;
        std::string button2Text;
        eAction button2Action;
        eRelationship rel;
        const bool enemies = eTeams::areEnemies(clientTeam, teamId);
        if(clientTeam == teamId) {
            rel = eRelationship::party;
        } else if(enemies) {
            rel = eRelationship::hostile;
        } else {
            rel = eRelationship::neutral;
        }
        switch(rel) {
        case eRelationship::neutral: {
            const bool thisInvited = eTeams::isInvited(teamId, clientId);
            if(thisInvited) {
                button1Text = eText::text(16, 0); // accept
                button1Action = [clientId, server, teamId]() {
                    eTeamAction action;
                    action.fType = eTeamActionType::acceptInvitation;
                    action.fTeamId = teamId;
                    server->teamAction(clientId, action);
                };
            } else {
                if(ms.size() == 1) {
                    const auto m = *ms.begin();
                    const bool mInvited = eTeams::isInvited(m, clientId);
                    if(mInvited) {
                        button1Text = eText::text(16, 1); // cancel
                        button1Action = [clientId, m, server]() {
                            eTeamAction action;
                            action.fType = eTeamActionType::cancelInvite;
                            action.fInvitedId = m;
                            server->teamAction(clientId, action);
                        };
                    } else {
                        button1Text = eText::text(16, 2); // invite
                        button1Action = [clientId, m, server]() {
                            eTeamAction action;
                            action.fType = eTeamActionType::invite;
                            action.fInvitedId = m;
                            server->teamAction(clientId, action);
                        };
                    }
                }
            }

            button2Text = eText::text(16, 3); // declare
            button2Action = [clientId, teamId, server]() {
                eTeamAction action;
                action.fType = eTeamActionType::makeEnemies;
                action.fTeamId = teamId;
                server->teamAction(clientId, action);
            };
        } break;
        case eRelationship::party: {
            if(ms.size() > 1) {
                button1Text = eText::text(16, 4); // leave
                button1Action = [clientId, server]() {
                    eTeamAction action;
                    action.fType = eTeamActionType::leaveTeam;
                    server->teamAction(clientId, action);
                };
            }
        } break;
        case eRelationship::hostile: {
            button1Text = eText::text(16, 5); // reconcile
            button1Action = [clientId, teamId, server]() {
                eTeamAction action;
                action.fType = eTeamActionType::makeFriends;
                action.fTeamId = teamId;
                server->teamAction(clientId, action);
            };
        } break;
        }
        teamW->initialize(members, button1Text, button1Action,
                          button2Text, button2Action, rel, darker);
        mCenterWidget->addWidget(teamW);
    }
    mCenterWidget->stackVertically();
}
