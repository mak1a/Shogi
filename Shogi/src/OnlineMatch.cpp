
#include "OnlineMatch.hpp"

OnlineMatch::OnlineMatch(const InitData& init, const double shogiBan_, const double komaDai_)
: IScene(init)
, m_shogiBan(s3d::Arg::center(s3d::Scene::CenterF()), shogiBan_)
, m_komaDaiSelf(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, (shogiBan_ / 2 - komaDai_) + komaDai_ / 2)), komaDai_)
, m_komaDaiEnemy(s3d::Arg::center(s3d::Scene::CenterF().movedBy(-(shogiBan_ / 2 + 10 + komaDai_ / 2), -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))), komaDai_)
, m_buttonWaited(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2) - 1.5 * (shogiBan_ / 9))),
                 s3d::Vec2(komaDai_, shogiBan_ / 12))
, m_buttonQuit(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))),
               s3d::Vec2(komaDai_, shogiBan_ / 12))
, m_turn(getData().firstMove)
, m_isBehind((getData().firstMove == Turn::Enemy))
, m_kyokumen(0, getData().GetBoard(), getData().motigomas)
, m_isUseMessageBox(false)
, m_isUseMessageWindow(false)
, m_messageState(MessageState::None) {
    // １マスの大きさ
    const double squareSize = shogiBan_ / 9;

    // 盤面に駒を設置
    for (size_t y = 0; y < 9; ++y) {
        for (size_t x = 0; x < 9; ++x) {
            m_boardSquares << KomaSquare(m_shogiBan.tl().x + x * squareSize, m_shogiBan.tl().y + y * squareSize, squareSize, getData().GetBoard()[y][x], s3d::Point(9 - x, y + 1));
        }
    }

    m_havingSelfKoma.resize(7);
    m_havingEnemyKoma.resize(7);

    /// <summary>
    /// 持ち駒の初期化
    /// </summary>
    for (uint32 type{Sfu}; type <= Shi; ++type) {
        for (uint32 i{}; i < getData().motigomas[type]; ++i) {
            m_havingSelfKoma[type - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[type - Self - 1], m_komaDaiSelf.w / 4, type, KomaState::Dai, s3d::Point(0, 0));
        }
    }
    for (uint32 type{Efu}; type <= Ehi; ++type) {
        for (uint32 i{}; i < getData().motigomas[type]; ++i) {
            m_havingEnemyKoma[type - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[type - Enemy - 1], m_komaDaiEnemy.w / 4, type, KomaState::Dai, s3d::Point(0, 0));
        }
    }

    // カスタム設定の場合、初期状態で詰みの可能性が存在するのでその確認
    if (m_kyokumen.MakeLegalMoves((getData().firstMove == Turn::Player) ? Self : Enemy) <= 0) {
        m_turn = Turn::Tsumi;
        m_winner = (getData().firstMove == Turn::Player) ? Winner::Enemy : Winner::Player;
        return;
    }

    m_stackKyokumens.emplace(m_kyokumen);
    m_stackBoradSquares.emplace(m_boardSquares);
    m_stackHavingSelf.emplace(m_havingSelfKoma);
    m_stackHavingEnemy.emplace(m_havingEnemyKoma);
    m_stackPlacedPart.emplace(m_placedPart);

    s3d::ClearPrint();
}

void OnlineMatch::EnemyUpdate(const Te& te_) {
    m_kyokumen.Move(Enemy, te_);

    if (te_.GetFrom() > 10) {
        m_boardSquares[(9 - te_.GetFrom() / 10) + ((te_.GetFrom() % 10) - 1) * 9].ChangeKomaType(Empty);
    }
    else {
        uint32 komaType{te_.GetKoma() - Enemy - 1};
        m_havingEnemyKoma[komaType].pop_back();
    }

    if (m_boardSquares[(9 - te_.GetTo() / 10) + ((te_.GetTo() % 10) - 1) * 9].GetKomaType() != Empty) {
        uint32 komaType = m_boardSquares[(9 - te_.GetTo() / 10) + ((te_.GetTo() % 10) - 1) * 9].GetKomaType();
        m_boardSquares[(9 - te_.GetTo() / 10) + ((te_.GetTo() % 10) - 1) * 9].ChangeKomaType(te_.GetKoma());

        // 置いた駒の場所を更新する
        m_placedPart.reset(m_boardSquares[(9 - te_.GetTo() / 10) + ((te_.GetTo() % 10) - 1) * 9]);

        komaType = ((komaType - Self + Enemy) & ~Promote);
        m_havingEnemyKoma[komaType - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[komaType - Enemy - 1], m_komaDaiEnemy.w / 4, static_cast<KomaType>(komaType), KomaState::Dai,
                                                              s3d::Point(0, 0));
    }
    else {
        m_boardSquares[(9 - te_.GetTo() / 10) + ((te_.GetTo() % 10) - 1) * 9].ChangeKomaType(te_.GetKoma());

        // 置いた駒の場所を更新する
        m_placedPart.reset(m_boardSquares[(9 - te_.GetTo() / 10) + ((te_.GetTo() % 10) - 1) * 9]);
    }

    ChangeCurrentTurn();
}

void OnlineMatch::AskPromoteKoma() {
    if (m_messageBox.Select() == YesNoSelection::None) {
        return;
    }

    m_isUseMessageBox = false;
    m_messageState = MessageState::None;

    if (m_messageBox.Select() == YesNoSelection::Yes) {
        m_te.SetPromote(1);
        if (m_kyokumen.IsIllegal(m_te)) {
            // Print << m_kyokumen.GetTeValids().size();
            return;
        }
    }
    m_holdHand.reset();

    m_kyokumen.Move(Self, m_te);
    SendOpponent(m_te);

    if (m_te.GetFrom() > 10) {
        m_boardSquares[(9 - m_te.GetFrom() / 10) + ((m_te.GetFrom() % 10) - 1) * 9].ChangeKomaType(Empty);
    }
    else {
        uint32 komaType{m_te.GetKoma() - Self - 1};
        m_havingSelfKoma[komaType].pop_back();
    }

    if (m_boardSquares[(9 - m_te.GetTo() / 10) + ((m_te.GetTo() % 10) - 1) * 9].GetKomaType() != Empty) {
        uint32 komaType = m_boardSquares[(9 - m_te.GetTo() / 10) + ((m_te.GetTo() % 10) - 1) * 9].GetKomaType();
        m_boardSquares[(9 - m_te.GetTo() / 10) + ((m_te.GetTo() % 10) - 1) * 9].ChangeKomaType(m_te.GetKoma());

        // 置いた駒の場所を更新する
        m_placedPart.reset(m_boardSquares[(9 - m_te.GetTo() / 10) + ((m_te.GetTo() % 10) - 1) * 9]);

        komaType = ((komaType - Enemy + Self) & ~Promote);
        m_havingSelfKoma[komaType - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[komaType - Self - 1], m_komaDaiSelf.w / 4, static_cast<KomaType>(komaType), KomaState::Dai,
                                                            s3d::Point(0, 0));
    }
    else {
        m_boardSquares[(9 - m_te.GetTo() / 10) + ((m_te.GetTo() % 10) - 1) * 9].ChangeKomaType(m_te.GetKoma());

        // 置いた駒の場所を更新する
        m_placedPart.reset(m_boardSquares[(9 - m_te.GetTo() / 10) + ((m_te.GetTo() % 10) - 1) * 9]);
    }
    ChangeCurrentTurn();
}

void OnlineMatch::AskQuitGame() {
    if (m_messageBox.Select() == YesNoSelection::None) {
        return;
    }

    m_isUseMessageBox = false;
    m_messageState = MessageState::None;

    if (m_messageBox.Select() == YesNoSelection::No) {
        return;
    }

    m_turn = Turn::Tsumi;
    m_winner = Winner::Enemy;
    ExitGames::Common::Dictionary<nByte, bool> dic;
    dic.put(1, true);
    GetClient().opRaiseEvent(true, dic, 4);
}

void OnlineMatch::AskWaited() {
    if (GetTurn() == Turn::Player && m_messageState == MessageState::Waited) {
        if (!m_messageWindow.SelectOK()) {
            return;
        }

        m_isUseMessageWindow = false;
        m_messageState = MessageState::None;
        return;
    }

    if (m_messageBox.Select() == YesNoSelection::None) {
        return;
    }

    m_isUseMessageBox = false;
    m_messageState = MessageState::None;

    ExitGames::Common::Dictionary<nByte, bool> dic;

    if (m_messageBox.Select() == YesNoSelection::No) {
        dic.put(1, false);
        GetClient().opRaiseEvent(true, dic, 5);
        return;
    }

    RetractingMove();
    dic.put(1, true);
    GetClient().opRaiseEvent(true, dic, 5);
}

// GameクラスのUpdate()で呼び出すメンバ関数
void OnlineMatch::SelfUpdate() {
    if (m_buttonWaited.mouseOver() || m_buttonQuit.mouseOver()) {
        s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
    }

    if (!m_holdHand.has_value() && m_buttonQuit.leftClicked()) {
        m_isUseMessageBox = true;
        m_messageState = MessageState::Quit;
        return;
    }

    if (!m_holdHand.has_value() && m_buttonWaited.leftClicked()) {
        if (m_stackKyokumens.size() <= 2) {
            return;
        }
        // m_isUseMessageBox = true;
        // m_messageState = MessageState::Waited;

        // RetractingMove();
        m_isUseMessageWindow = true;
        m_messageWindow.SetLabel(U"待ったを申請中です。\n暫くお待ちください...");
        ExitGames::Common::Dictionary<nByte, bool> dic;
        dic.put(1, true);
        GetClient().opRaiseEvent(true, dic, 3);
        return;
    }

    // 盤面上の処理
    for (auto& square : m_boardSquares) {
        // 盤面から駒を選んで手に持つ処理
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                if (square.mouseOver() && square.GetKomaType() != Empty) {
                    s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
                }
                continue;
            }
            if (square.GetKomaType() == Empty) {
                return;
            }

            // もしも自分の駒でないもしくは自分のターンでない場合、手に持つ事はできない
            if ((GetTurn() == Turn::Player && square.GetKomaType() > Enemy) || (GetTurn() == Turn::Enemy)) {
                return;
            }

            m_holdHand.emplace(square);
            square.ChangeKomaType(Emp);
            AddCandidateHand();
            return;
        }

        // クリックしてない場合は次の処理に
        if (!square.leftClicked()) {
            m_holdHand.value().setCenter(s3d::Cursor::PosF());
            continue;
        }

        // 置く場所に駒が存在してた場合、
        // その駒を駒台に置くかどうかを見極める
        if (square.GetKomaType() != Empty) {
            AddHoldKoma(square);
            return;
        }

        if (!m_holdHand.value().IsChangeCoodinate(square)) {
            square.ChangeKomaType(m_holdHand.value().GetKomaType());
            m_holdHand.reset();
            m_candidateHands.clear();
            return;
        }

        // 置く場所に何もなかったら、持ってる駒を置く
        Te te{static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10),
              static_cast<uint32>(square.GetKomaCoodinate().y + square.GetKomaCoodinate().x * 10), m_holdHand.value().GetKomaType()};

        if (te.GetFrom() >= 11 && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()]
            && ((te.GetFrom() % 10) <= 3 || (te.GetTo() % 10) <= 3)) {
            if (m_holdHand.value().GetKomaType() == Ske && (te.GetTo() % 10) <= 2) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    // Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else if ((m_holdHand.value().GetKomaType() == Sfu || m_holdHand.value().GetKomaType() == Sky) && (te.GetTo() % 10) <= 1) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    // Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else {
                if (m_kyokumen.IsIllegal(te)) {
                    // Print << m_kyokumen.GetTeValids().size();
                    return;
                }

                m_isUseMessageBox = true;
                m_messageState = MessageState::Promote;
                m_te = te;

                return;
            }
        }
        if (m_kyokumen.IsIllegal(te)) {
            return;
        }

        square.ChangeKomaType(m_holdHand.value().GetKomaType());

        // 置いた駒の場所を更新する
        m_placedPart.reset(square);

        m_kyokumen.Move(Self, te);
        SendOpponent(te);
        m_holdHand.reset();

        ChangeCurrentTurn();
        return;
    }

    // 駒台から取った駒を元の駒台の位置に戻す処理
    // いわゆるキャンセル処理
    if (m_holdHand.has_value()) {
        // 持ってる駒が盤面から取った駒なら、早期リターン
        if (m_holdHand.value().GetKomaState() == KomaState::Ban) {
            return;
        }
        // 何もクリックしてなかったら、処理リターン
        if (!m_komaDaiSelf.leftClicked()) {
            m_holdHand.value().setCenter(s3d::Cursor::PosF());
            return;
        }

        m_havingSelfKoma[m_holdHand.value().GetKomaType() - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[m_holdHand.value().GetKomaType() - Self - 1], m_komaDaiSelf.w / 4,
                                                                                    m_holdHand.value().GetKomaType(), KomaState::Dai, s3d::Point(0, 0));

        m_holdHand.reset();
        m_candidateHands.clear();
        return;
    }

    // プレイヤーの駒台から駒を取る処理
    for (auto& havingSelfKoma : m_havingSelfKoma) {
        for (auto [i, koma] : s3d::IndexedRef(havingSelfKoma)) {
            if (koma.mouseOver()) {
                s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
            }
            if (!koma.leftClicked()) {
                continue;
            }
            m_holdHand.emplace(koma);
            havingSelfKoma.remove_at(i);
            AddCandidateHand();
            return;
        }
    }
}

void OnlineMatch::Draw() const {
    m_shogiBan.draw(s3d::Palette::Burlywood);
    m_komaDaiSelf.draw(s3d::Palette::Burlywood);
    m_komaDaiEnemy.draw(s3d::Palette::Burlywood);
    m_buttonWaited.draw(s3d::Palette::White);
    m_buttonWaited.drawFrame(1.0, s3d::Palette::Black);
    m_buttonQuit.draw(s3d::Palette::White);
    m_buttonQuit.drawFrame(1.0, s3d::Palette::Black);

    s3d::FontAsset(U"Menu")(U"待った").drawAt(m_buttonWaited.center(), s3d::Palette::Black);
    s3d::FontAsset(U"Menu")(U"投了").drawAt(m_buttonQuit.center(), s3d::Palette::Black);

    // 駒を置いた場所を少し赤くする
    if (m_placedPart.has_value()) {
        m_placedPart.value().draw(s3d::ColorF(s3d::Palette::Red, 0.5f));
    }

    for (const auto& candidateHand : m_candidateHands) {
        candidateHand.draw(s3d::ColorF(s3d::Palette::Greenyellow, 0.2 + 0.5 * s3d::Periodic::Sine0_1(s3d::SecondsF(1.2))));
    }

    for (const auto& square : m_boardSquares) {
        square.drawFrame(2, s3d::Palette::Black);

        square.Draw();
    }

    for (const auto& selfDai : m_havingSelfKoma) {
        for (const auto [i, koma] : s3d::IndexedRefReversed(selfDai)) {
            koma.Draw(s3d::Vec2(i * 10, 0));
        }
    }
    for (const auto& enemyDai : m_havingEnemyKoma) {
        for (const auto [i, koma] : s3d::IndexedRefReversed(enemyDai)) {
            koma.Draw(s3d::Vec2(static_cast<int32>(i) * -10, 0));
        }
    }

    if (m_holdHand.has_value()) {
        m_holdHand.value().Draw();
    }
}

void OnlineMatch::AddHoldKoma(KomaSquare& koma_) {
    if ((koma_.GetKomaType() <= Enemy && m_holdHand.value().GetKomaType() <= Enemy) || (koma_.GetKomaType() > Enemy && m_holdHand.value().GetKomaType() > Enemy)) {
        return;
    }

    Te te{static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10),
          static_cast<uint32>(koma_.GetKomaCoodinate().y + koma_.GetKomaCoodinate().x * 10), m_holdHand.value().GetKomaType(), koma_.GetKomaType()};

    if (te.GetFrom() >= 11 && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()]
        && ((te.GetFrom() % 10) <= 3 || (te.GetTo() % 10) <= 3)) {
        if (m_holdHand.value().GetKomaType() == Ske && (te.GetTo() % 10) <= 2) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                // Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else if ((m_holdHand.value().GetKomaType() == Sfu || m_holdHand.value().GetKomaType() == Sky) && (te.GetTo() % 10) <= 1) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                // Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else {
            if (m_kyokumen.IsIllegal(te)) {
                return;
            }

            m_isUseMessageBox = true;
            m_messageState = MessageState::Promote;
            m_te = te;

            return;
        }
    }
    if (m_kyokumen.IsIllegal(te)) {
        return;
    }

    m_kyokumen.Move(Self, te);
    SendOpponent(te);

    uint32 komaType = koma_.GetKomaType();
    koma_.ChangeKomaType(m_holdHand.value().GetKomaType());

    // 置いた駒の場所を更新する
    m_placedPart.reset(koma_);

    m_holdHand.reset();

    // 敵駒にする場合
    if (komaType <= Enemy) {
        komaType = ((komaType - Self + Enemy) & ~Promote);
        m_havingEnemyKoma[komaType - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[komaType - Enemy - 1], m_komaDaiEnemy.w / 4, static_cast<KomaType>(komaType), KomaState::Dai,
                                                              s3d::Point(0, 0));
    }
    else {
        // 自駒にする場合
        komaType = ((komaType - Enemy + Self) & ~Promote);
        m_havingSelfKoma[komaType - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[komaType - Self - 1], m_komaDaiSelf.w / 4, static_cast<KomaType>(komaType), KomaState::Dai,
                                                            s3d::Point(0, 0));
    }

    ChangeCurrentTurn();
}

void OnlineMatch::SendOpponent(const Te& te_) {
    ExitGames::Common::Dictionary<nByte, int32> dic;
    dic.put(1, (te_.GetFrom() > 10) ? 110 - te_.GetFrom() : 0);
    dic.put(2, 110 - te_.GetTo());
    dic.put(3, (te_.GetKoma() & ~Self | Enemy));
    dic.put(4, te_.GetCapture());
    dic.put(5, te_.GetPromote());
    dic.put(6, te_.GetKind());
    dic.put(7, te_.GetValue());

    GetClient().opRaiseEvent(true, dic, 1);
}

void OnlineMatch::update() {
    if (m_isUseMessageBox) {
        switch (m_messageState) {
        case MessageState::Promote:
            AskPromoteKoma();
            break;
        case MessageState::Quit:
            AskQuitGame();
            break;
        case MessageState::Waited:
            AskWaited();
            break;
        default:
            break;
        }
        return;
    }

    if (m_isUseMessageWindow) {
        AskWaited();
        return;
    }

    switch (GetTurn()) {
    case Turn::Player:
        SelfUpdate();
        break;
    case Turn::Enemy:
        break;
    case Turn::Tsumi:
        result();
        break;
    default:
        break;
    }
}

void OnlineMatch::draw() const {
    Draw();

    if (m_isUseMessageBox) {
        switch (m_messageState) {
        case MessageState::Promote:
            m_messageBox.Draw(U"成りますか？");
            break;
        case MessageState::Quit:
            m_messageBox.Draw(U"投了しますか？");
            break;
        case MessageState::Waited:
            m_messageBox.Draw(U"相手の「待った」を\n　承認しますか？");
            break;
        default:
            break;
        }
    }

    if (m_isUseMessageWindow) {
        m_messageWindow.Draw();
    }

    if (GetTurn() == Turn::Tsumi) {
        if (GetWinner() == Winner::Player) {
            s3d::FontAsset(U"Result")(U"勝利").drawAt(s3d::Scene::CenterF().movedBy(0, -100), s3d::Palette::Red);
        }
        else if (GetWinner() == Winner::Enemy) {
            s3d::FontAsset(U"Result")(U"敗北").drawAt(s3d::Scene::CenterF().movedBy(0, -100), s3d::Palette::Blue);
        }
        else {
            s3d::FontAsset(U"Result")(U"千日手です").drawAt(s3d::Scene::CenterF().movedBy(0, -100), s3d::Palette::Black);
        }
        s3d::FontAsset(U"Explain")(U"画面をクリックでタイトルに戻る").drawAt(s3d::Scene::CenterF().movedBy(0, 50), s3d::Palette::Darkred);
    }
}

void OnlineMatch::result() {
    if (s3d::MouseL.down()) {
        Disconnect();
    }
}

void OnlineMatch::CustomEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) {
    if (eventCode == 3) {
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, bool>>(eventContent).getDataCopy();
        m_isUseMessageBox = true;
        m_messageState = MessageState::Waited;
        // RetractingMove();
        return;
    }
    if (eventCode == 4) {
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, bool>>(eventContent).getDataCopy();
        m_turn = Turn::Tsumi;
        m_winner = Winner::Player;
        return;
    }
    if (eventCode == 5) {
        auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, bool>>(eventContent).getDataCopy();
        if (*dic.getValue(1)) {
            RetractingMove();
            m_messageWindow.SetLabel(U"待ったが承認されました", true);
            m_messageState = MessageState::Waited;
            return;
        }

        m_messageWindow.SetLabel(U"待ったが拒否されました", true);
        m_messageState = MessageState::Waited;
        return;
    }

    if (eventCode != 1) {
        return;
    }

    auto dic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<nByte, int32>>(eventContent).getDataCopy();
    Te te{static_cast<uint32>(*dic.getValue(1)),
          static_cast<uint32>(*dic.getValue(2)),
          static_cast<uint32>(*dic.getValue(3)),
          static_cast<uint32>(*dic.getValue(4)),
          static_cast<uint32>(*dic.getValue(5)),
          static_cast<uint32>(*dic.getValue(6)),
          *dic.getValue(7)};
    EnemyUpdate(te);
}

void OnlineMatch::LeaveRoomEventAction(int playerNr, bool isInactive) {
    s3d::Print(U"対戦相手が退室しました。");
    getData().photonState = PhotonState::Master;

    if (GetTurn() == Turn::Tsumi) {
        return;
    }

    Disconnect();
}

void OnlineMatch::DisconnectReturn() {
    s3d::Print(U"切断しました");
    changeScene(State::Title, s3d::Seconds(1));
}
