#include "epartywidget.h"

#include "egamewidget.h"
#include "../elabel.h"
#include "../mainMenu/emainmenubutton.h"

#include <eSlayerHelpers/eteamid.h>

enum class eRelationship {
    party, hostile, neutral
};

class ePlayerWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& name,
                    const std::string& button1Text,
                    const eAction& button1Action,
                    const std::string& button2Text,
                    const eAction& button2Action,
                    const eRelationship rel) {
        switch(rel) {
        case eRelationship::party:
            mBorderColor = SDL_Color{0, 255, 0, 255};
            mBgColor = SDL_Color{0, 255, 0, 128};
            break;
        case eRelationship::hostile:
            mBorderColor = SDL_Color{255, 0, 0, 255};
            mBgColor = SDL_Color{255, 0, 0, 128};
            break;
        case eRelationship::neutral:
            mBorderColor = SDL_Color{255, 255, 255, 255};
            mBgColor = SDL_Color{0, 0, 0, 0};
            break;
        }

        const auto innerW = new eWidget(window());
        innerW->setNoPadding();
        const auto& res = resolution();
        const int p = res.smallPadding();
        const int w = width();
        const int h = height();
        innerW->resize(w - 2*p, h - 2*p);
        addWidget(innerW);
        innerW->align(eAlignment::center);

        const auto nameLabel = new eLabel(window());
        nameLabel->setText(name);
        nameLabel->setSmallFontSize();
        nameLabel->setNoPadding();
        nameLabel->fitContent();
        innerW->addWidget(nameLabel);
        nameLabel->align(eAlignment::vcenter);

        if(button1Action) {
            const auto button = new eMainMenuButton(button1Text, window());
            button->setSmallFontSize();
            button->setPressAction(button1Action);
            innerW->addWidget(button);
            button->align(eAlignment::vcenter);
        }

        if(button2Action) {
            const auto button = new eMainMenuButton(button2Text, window());
            button->setSmallFontSize();
            button->setPressAction(button2Action);
            innerW->addWidget(button);
            button->align(eAlignment::vcenter);
        }

        innerW->stackHorizontally(p);
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
    for(const auto& it : teams) {
        const auto teamId = it.first;
        const auto& team = it.second;
        for(const auto m : team.fMembers) {
            const auto& name = eGameWidget::sUserNames[m];
            const auto playerW = new ePlayerWidget(window());
            playerW->resize(mPlayerWidgetWidth, mPlayerWidgetHeight);
            if(m == clientId) {
                playerW->initialize(name, "", nullptr, "", nullptr,
                                    eRelationship::neutral);
            } else {
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
                    const bool thisInvited = eTeams::isInvited(clientId, m);
                    if(thisInvited) {
                        button1Text = "Accept";
                        button1Action = [clientId, m, server, teamId]() {
                            eTeamAction action;
                            action.fType = eTeamActionType::acceptInvitation;
                            action.fTeamId = teamId;
                            server->teamAction(clientId, action);
                        };
                    } else {
                        const bool mInvited = eTeams::isInvited(m, clientId);
                        if(mInvited) {
                            button1Text = "Cancel Invite";
                            button1Action = [clientId, m, server]() {
                                eTeamAction action;
                                action.fType = eTeamActionType::cancelInvite;
                                action.fInvitedId = m;
                                server->teamAction(clientId, action);
                            };
                        } else {
                            button1Text = "Invite";
                            button1Action = [clientId, m, server]() {
                                eTeamAction action;
                                action.fType = eTeamActionType::invite;
                                action.fInvitedId = m;
                                server->teamAction(clientId, action);
                            };
                        }
                    }

                    button2Text = "Declare";
                    button2Action = [clientId, teamId, server]() {
                        eTeamAction action;
                        action.fType = eTeamActionType::makeEnemies;
                        action.fTeamId = teamId;
                        server->teamAction(clientId, action);
                    };
                } break;
                case eRelationship::party: {
                    button1Text = "Leave";
                    button1Action = [clientId, server]() {
                        eTeamAction action;
                        action.fType = eTeamActionType::leaveTeam;
                        server->teamAction(clientId, action);
                    };
                } break;
                case eRelationship::hostile: {
                    button1Text = "Reconcile";
                    button1Action = [clientId, teamId, server]() {
                        eTeamAction action;
                        action.fType = eTeamActionType::makeFriends;
                        action.fTeamId = teamId;
                        server->teamAction(clientId, action);
                    };
                } break;
                }
                playerW->initialize(name, button1Text, button1Action,
                                    button2Text, button2Action, rel);
            }
            mCenterWidget->addWidget(playerW);
        }
    }
    mCenterWidget->stackVertically();
}
