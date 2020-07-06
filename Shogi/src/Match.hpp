#pragma once
#include "Kyokumen.hpp"


class Match : public MyApp::Scene {
private:
    void ConnectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;

    void DisconnectReturn() override;

    void CreateRoomReturn(int localPlayerNr,
                          const ExitGames::Common::Hashtable& roomProperties,
                          const ExitGames::Common::Hashtable& playerProperties,
                          int errorCode,
                          const ExitGames::Common::JString& errorString) override;

    void JoinRandomRoomReturn(int localPlayerNr,
                              const ExitGames::Common::Hashtable& roomProperties,
                              const ExitGames::Common::Hashtable& playerProperties,
                              int errorCode,
                              const ExitGames::Common::JString& errorString) override;

    void JoinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override;

    void LeaveRoomEventAction(int playerNr, bool isInactive) override;

    void CustomEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

public:
    Match(const InitData& init);

    void update() override;

    void draw() const override;
};
