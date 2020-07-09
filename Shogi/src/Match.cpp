#include "Match.hpp"

Match::Match(const InitData& init)
: IScene(init)
, m_hirate(s3d::Image(s3d::Resource(U"textures/match/kiban.jpg")).scale(700, 700))
, m_ban(s3d::Arg::center(s3d::Scene::CenterF()), 700, 700)
, m_gauss(s3d::Arg::center(s3d::Scene::CenterF()), 700, 700)
, rtA(s3d::Size(700, 700))
, rtB(s3d::Size(700, 700))
, rtA4(s3d::Size(700, 700) / 4)
, rtB4(s3d::Size(700, 700) / 4)
, m_search(U"対戦相手を探しています...") {
    // 背景色を設定
    s3d::Scene::SetBackground(s3d::Palette::Darkgreen);

    Connect();
    s3d::Print(U"接続中...");

    GetClient().fetchServerTimestamp();

    s3d::Shader::GaussianBlur(m_hirate, rtB, rtA);
    s3d::Shader::Downsample(rtA, rtA4);
    s3d::Shader::GaussianBlur(rtA4, rtB4, rtA4);
}

void Match::update() {
    s3d::Shader::GaussianBlur(m_hirate, rtB, rtA);
    s3d::Shader::Downsample(rtA, rtA4);
    s3d::Shader::GaussianBlur(rtA4, rtB4, rtA4);

    if (getData().photonState == PhotonState::None) {
        return;
    }

    if (s3d::KeySpace.down()) {
        Disconnect();
    }
}

void Match::draw() const {
    m_gauss(rtA4).draw();

    s3d::FontAsset(U"Warning")(m_search).drawAt(s3d::Scene::CenterF().movedBy(0, -150), s3d::Palette::Lightblue);
}

void Match::ConnectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) {
    if (errorCode) {
        s3d::Print(U"接続出来ませんでした");
        return;
    }

    s3d::Print(U"接続しました");
    GetClient().opJoinRandomRoom(getData().GetCustomProperties(), 2);
}

void Match::DisconnectReturn() {
    s3d::Print(U"切断しました");
    // 背景色を設定
    s3d::Scene::SetBackground(s3d::ColorF(0.2, 0.8, 0.4));
    changeScene(State::Title);
}

void Match::CreateRoomReturn(int localPlayerNr,
                             const ExitGames::Common::Hashtable& roomProperties,
                             const ExitGames::Common::Hashtable& playerProperties,
                             int errorCode,
                             const ExitGames::Common::JString& errorString) {
    if (errorCode) {
        s3d::Print(U"部屋を作成出来ませんでした");
        return;
    }

    s3d::Print(U"部屋を作成しました！");
    getData().photonState = PhotonState::Master;
}

void Match::JoinRandomRoomReturn(int localPlayerNr,
                                 const ExitGames::Common::Hashtable& roomProperties,
                                 const ExitGames::Common::Hashtable& playerProperties,
                                 int errorCode,
                                 const ExitGames::Common::JString& errorString) {
    if (errorCode) {
        s3d::Print(U"部屋が見つかりませんでした");

        CreateRoom(L"", getData().GetCustomProperties(), 2);

        s3d::Print(U"部屋を作成します...");
        return;
    }

    s3d::Print(U"部屋に接続しました!");
    getData().photonState = PhotonState::Join;
    m_search = U"対戦相手がセッティングしてます...";
}

void Match::JoinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) {
    if (GetClient().getLocalPlayer().getNumber() == player.getNumber()) {
        return;
    }

    s3d::Print(U"対戦相手が入室しました。");
    // 背景色を設定
    s3d::Scene::SetBackground(s3d::ColorF(0.2, 0.8, 0.4));
    changeScene(State::Select);
}

void Match::LeaveRoomEventAction(int playerNr, bool isInactive) {
    s3d::Print(U"対戦相手が退室しました。");
    getData().photonState = PhotonState::Master;
}

void Match::CustomEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) {
    s3d::Print(U"受信しました");
    if (eventCode != 2) {
        return;
    }
    auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, int32>>(eventContent).getDataCopy();
    getData().firstMove = static_cast<Turn>(!*dic.getValue(1));
    getData().elegance = static_cast<Elegance>(!*dic.getValue(2));
    getData().handicap = static_cast<Handicap>(*dic.getValue(3));

    // 背景色を設定
    s3d::Scene::SetBackground(s3d::ColorF(0.2, 0.8, 0.4));
    changeScene(State::Game);
}
