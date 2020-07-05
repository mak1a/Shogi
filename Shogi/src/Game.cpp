﻿
#include "Game.hpp"

Ban::Ban(const array<array<uint32, 9>, 9>& iniKyokumen_, const array<uint32, 40>& motigomas_, const Turn& turn_, const double shogiBan_, const double komaDai_) noexcept
: m_shogiBan(s3d::Arg::center(s3d::Scene::CenterF()), shogiBan_)
, m_komaDaiSelf(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, (shogiBan_ / 2 - komaDai_) + komaDai_ / 2)), komaDai_)
, m_komaDaiEnemy(s3d::Arg::center(s3d::Scene::CenterF().movedBy(-(shogiBan_ / 2 + 10 + komaDai_ / 2), -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))), komaDai_)
, m_buttonWaited(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2) - 1.5 * (shogiBan_ / 9))),
                 s3d::Vec2(komaDai_, shogiBan_ / 12))
, m_buttonQuit(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))),
               s3d::Vec2(komaDai_, shogiBan_ / 12))
, m_turn(turn_)
, m_isBehind((turn_ == Turn::Enemy))
, m_kyokumen(0, iniKyokumen_, motigomas_) {
    // １マスの大きさ
    const double squareSize = shogiBan_ / 9;

    // 盤面に駒を設置
    for (size_t y = 0; y < 9; ++y) {
        for (size_t x = 0; x < 9; ++x) {
            m_boardSquares << KomaSquare(m_shogiBan.tl().x + x * squareSize, m_shogiBan.tl().y + y * squareSize, squareSize, iniKyokumen_[y][x], s3d::Point(9 - x, y + 1));
        }
    }

    m_havingSelfKoma.resize(7);
    m_havingEnemyKoma.resize(7);

    /// <summary>
    /// 持ち駒の初期化
    /// </summary>
    for (uint32 type{Sfu}; type <= Shi; ++type) {
        for (uint32 i{}; i < motigomas_[type]; ++i) {
            m_havingSelfKoma[type - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[type - Self - 1], m_komaDaiSelf.w / 4, type, KomaState::Dai, s3d::Point(0, 0));
        }
    }
    for (uint32 type{Efu}; type <= Ehi; ++type) {
        for (uint32 i{}; i < motigomas_[type]; ++i) {
            m_havingEnemyKoma[type - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[type - Enemy - 1], m_komaDaiEnemy.w / 4, type, KomaState::Dai, s3d::Point(0, 0));
        }
    }

    // カスタム設定の場合、初期状態で詰みの可能性が存在するのでその確認
    if (m_kyokumen.MakeLegalMoves((turn_ == Turn::Player) ? Self : Enemy) <= 0) {
        m_turn = Turn::Tsumi;
        m_winner = (turn_ == Turn::Player) ? Winner::Enemy : Winner::Player;
        return;
    }

    m_stackKyokumens.emplace(m_kyokumen);
    m_stackBoradSquares.emplace(m_boardSquares);
    m_stackHavingSelf.emplace(m_havingSelfKoma);
    m_stackHavingEnemy.emplace(m_havingEnemyKoma);
    m_stackPlacedPart.emplace(m_placedPart);
}

// GameクラスのUpdate()で呼び出すメンバ関数
void Ban::Update() {
    if (m_buttonWaited.mouseOver() || m_buttonQuit.mouseOver()) {
        s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
    }

    if (!m_holdHand.has_value() && m_buttonQuit.leftClicked()) {
        if (GetTurn() == Turn::Player) {
            m_winner = Winner::Enemy;
        }
        else {
            m_winner = Winner::Player;
        }
        m_turn = Turn::Tsumi;
        return;
    }

    if (!m_holdHand.has_value() && GetTurn() == Turn::Player && m_buttonWaited.leftClicked()) {
        RetractingMove();
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
            if ((GetTurn() == Turn::Player && square.GetKomaType() > Enemy) || (GetTurn() == Turn::Enemy && square.GetKomaType() <= Enemy)) {
                return;
            }

            m_holdHand.emplace(square);
            square.ChangeKomaType(Emp);
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
            return;
        }

        // 置く場所に何もなかったら、持ってる駒を置く
        Te te{static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10),
              static_cast<uint32>(square.GetKomaCoodinate().y + square.GetKomaCoodinate().x * 10), m_holdHand.value().GetKomaType()};

        if (te.GetFrom() >= 11 && GetTurn() == Turn::Player && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()]
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
                if (s3d::System::ShowMessageBox(U"成りますか？", s3d::MessageBoxButtons::YesNo) == s3d::MessageBoxSelection::Yes) {
                    te.SetPromote(1);
                    if (m_kyokumen.IsIllegal(te)) {
                        return;
                    }
                    m_holdHand.value().PromoteKoma();
                }
            }
        }
        if (te.GetFrom() >= 11 && GetTurn() == Turn::Enemy && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()]
            && ((te.GetFrom() % 10) >= 7 || (te.GetTo() % 10) >= 7)) {
            if (m_holdHand.value().GetKomaType() == Eke && (te.GetTo() % 10) >= 8) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    // Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else if ((m_holdHand.value().GetKomaType() == Efu || m_holdHand.value().GetKomaType() == Eky) && (te.GetTo() % 10) >= 9) {
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
                if (s3d::System::ShowMessageBox(U"成りますか？", s3d::MessageBoxButtons::YesNo) == s3d::MessageBoxSelection::Yes) {
                    te.SetPromote(1);
                    if (m_kyokumen.IsIllegal(te)) {
                        return;
                    }
                    m_holdHand.value().PromoteKoma();
                }
            }
        }
        if (m_kyokumen.IsIllegal(te)) {
            // Print << m_kyokumen.GetTeValids().size();
            return;
        }

        // 置く場所に何もなかったら、持ってる駒を置く
        square.ChangeKomaType(m_holdHand.value().GetKomaType());

        // 置いた駒の場所を更新する
        m_placedPart.reset(square);

        m_kyokumen.Move(((GetTurn() == Turn::Player) ? Self : Enemy), te);
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
        if (!m_komaDaiSelf.leftClicked() && !m_komaDaiEnemy.leftClicked()) {
            m_holdHand.value().setCenter(s3d::Cursor::PosF());
            return;
        }

        if (m_komaDaiSelf.leftClicked()) {
            m_havingSelfKoma[m_holdHand.value().GetKomaType() - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[m_holdHand.value().GetKomaType() - Self - 1], m_komaDaiSelf.w / 4,
                                                                                        m_holdHand.value().GetKomaType(), KomaState::Dai, s3d::Point(0, 0));
        }
        else {
            m_havingEnemyKoma[m_holdHand.value().GetKomaType() - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[m_holdHand.value().GetKomaType() - Enemy - 1],
                                                                                          m_komaDaiEnemy.w / 4, m_holdHand.value().GetKomaType(), KomaState::Dai, s3d::Point(0, 0));
        }

        m_holdHand.reset();
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
            return;
        }
    }

    // 敵の駒台から駒を取る処理
    for (auto& havingEnemyKoma : m_havingEnemyKoma) {
        for (auto [i, koma] : s3d::IndexedRef(havingEnemyKoma)) {
            if (koma.mouseOver()) {
                s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
            }
            if (!koma.leftClicked()) {
                continue;
            }
            m_holdHand.emplace(koma);
            havingEnemyKoma.remove_at(i);
            return;
        }
    }
}

void Ban::Draw() const {
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

void Ban::AddHoldKoma(KomaSquare& koma_) {
    if ((koma_.GetKomaType() <= Enemy && m_holdHand.value().GetKomaType() <= Enemy) || (koma_.GetKomaType() > Enemy && m_holdHand.value().GetKomaType() > Enemy)) {
        return;
    }

    Te te{static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10),
          static_cast<uint32>(koma_.GetKomaCoodinate().y + koma_.GetKomaCoodinate().x * 10), m_holdHand.value().GetKomaType(), koma_.GetKomaType()};

    if (te.GetFrom() >= 11 && GetTurn() == Turn::Player && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()]
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
            if (s3d::System::ShowMessageBox(U"成りますか？", s3d::MessageBoxButtons::YesNo) == s3d::MessageBoxSelection::Yes) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
        }
    }
    if (te.GetFrom() >= 11 && GetTurn() == Turn::Enemy && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()]
        && ((te.GetFrom() % 10) >= 7 || (te.GetTo() % 10) >= 7)) {
        if (m_holdHand.value().GetKomaType() == Eke && (te.GetTo() % 10) >= 8) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                // Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else if ((m_holdHand.value().GetKomaType() == Efu || m_holdHand.value().GetKomaType() == Eky) && (te.GetTo() % 10) >= 9) {
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
            if (s3d::System::ShowMessageBox(U"成りますか？", s3d::MessageBoxButtons::YesNo) == s3d::MessageBoxSelection::Yes) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
        }
    }
    if (m_kyokumen.IsIllegal(te)) {
        return;
    }

    m_kyokumen.Move(((GetTurn() == Turn::Player) ? Self : Enemy), te);

    uint32 komaType = koma_.GetKomaType();
    koma_.ChangeKomaType(m_holdHand.value().GetKomaType());

    // 置いた駒の場所を更新する
    m_placedPart.reset(koma_);

    m_holdHand.reset();
    ChangeCurrentTurn();

    // 敵駒にする場合
    if (komaType <= Enemy) {
        komaType = ((komaType - Self + Enemy) & ~Promote);
        m_havingEnemyKoma[komaType - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[komaType - Enemy - 1], m_komaDaiEnemy.w / 4, static_cast<KomaType>(komaType), KomaState::Dai,
                                                              s3d::Point(0, 0));
        return;
    }

    // 自駒にする場合
    komaType = ((komaType - Enemy + Self) & ~Promote);
    m_havingSelfKoma[komaType - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[komaType - Self - 1], m_komaDaiSelf.w / 4, static_cast<KomaType>(komaType), KomaState::Dai,
                                                        s3d::Point(0, 0));
    return;
}

Game::Game(const InitData& init) : IScene(init), m_ban(getData().GetBoard(), getData().motigomas, getData().firstMove) {}

void Game::update() {
    if (m_ban.GetTurn() != Turn::Tsumi) {
        m_ban.Update();
    }
    else {
        result();
    }
}

void Game::draw() const {
    m_ban.Draw();

    if (m_ban.GetTurn() == Turn::Tsumi) {
        if (m_ban.GetWinner() == Winner::Player) {
            s3d::FontAsset(U"Result")(U"勝利").drawAt(s3d::Scene::CenterF().movedBy(0, -100), s3d::Palette::Red);
        }
        else if (m_ban.GetWinner() == Winner::Enemy) {
            s3d::FontAsset(U"Result")(U"敗北").drawAt(s3d::Scene::CenterF().movedBy(0, -100), s3d::Palette::Blue);
        }
        else {
            s3d::FontAsset(U"Result")(U"千日手です").drawAt(s3d::Scene::CenterF().movedBy(0, -100), s3d::Palette::Black);
        }
        s3d::FontAsset(U"Explain")(U"画面をクリックでタイトルに戻る").drawAt(s3d::Scene::CenterF().movedBy(0, 50), s3d::Palette::Darkred);
    }
}

void Game::result() {
    if (s3d::MouseL.down()) {
        changeScene(State::Title, s3d::Seconds(1));
    }
}
